#include <stdio.h>
#include <stdlib.h>

// eeem... not good

// general approach: ordered linked list per each hand type.

typedef struct play {
	int typ;
	char hand[6];
	int bid;
	struct play *next;
} play;

play *rank(play *ps, play *p) {
	if (ps == NULL) {
		return p;
	}
	play *cur = ps;
	play *prev = ps;
	do {
		if (cur == NULL) { // insert to tail
			prev->next = p;
			break;
		}
		int i=0;
		while (p->hand[i] == cur->hand[i]) {
				++i;
		}
		if (p->hand[i] < cur->hand[i]) {
			if (prev == cur) { // insert to head
				p->next = cur;
				ps = p;
			} else { // insert to middle
				prev->next=p;
				p->next=cur;
			}
			break;
		}
		prev = cur;
		cur = cur->next;
	} while(1);
	return ps;
}

void play_free(play *ps[7]) {
	for (int i=0; i<7; ++i) {
		play *ps_i = ps[i];
		while (ps_i) {
			play *me = ps_i;
			ps_i = ps_i->next;
			if (me) {
				free(me);
				me = NULL;
			}
		}
	}
}

void zero(int *arr, size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}

int ex(char *buf, size_t count, int e) {
	play *ps[7];
	for (int i=0;i<7;++i) {
		ps[i] = NULL;
	}

	int label_count = 15; // +1 extra for '1' and +1 for accounting diff between len and idx
	for (int i=0; i<count; ++i) {
		int typ[label_count];
		zero(typ, label_count);

		char hand[6];
		hand[5] = '\0';

		play *p = calloc(1, sizeof(play));
		// translate labels to more convenient chars to work with
		for (int j=0; j<5; ++j) {
			char c = buf[i+j];
			switch (c) {
			case 'T':
				c='9'+1; break;
			case 'J':
				if (e==1) {
					c='9'+2; break;
				} else if (e == 2) {
					c='1'; break;
				}
			case 'Q':
				c='9'+3; break;
			case 'K':
				c='9'+4; break;
			case 'A':
				c='9'+5; break;
			}
			p->hand[j] = c;
			++typ[c-'0'];
		}

		// 0:highest; 1:one pair; 2:two pairs; 3:three of a kind; 4:full house; 5:four of a kind; 6:five of a kind;
		// handle jokers for ex2 - inc the largest count by joker amount. Prefer larger value
		if (e == 2 && typ[1] > 0) {
			int biggest_count = 0;
			int idx = 1;
			for (int p=2; p<label_count; ++p) {
				if (typ[p] >= biggest_count) { // >= so that J substitutes the biggest label
					biggest_count =  typ[p];
					idx = p;
				}
			}
			int joker_count = typ[1];
			typ[1] = 0;
			typ[idx] += joker_count;
		}
		// find p->typ
		for (int k=1; k<label_count; ++k) {
			switch (typ[k]) {
			case 1:
				continue; break;
			case 2:
				if (p->typ != 0) {
					p->typ+=1; // damn, what a bug this was (previously was ++p->typ)
				} else {
					p->typ = 1;
				}
				break;
			case 3:
				if (p->typ != 0) {
					p->typ+=3; // damn, what a bug this was (previously was ++p->typ)
				} else {
					p->typ = 3;
				}
				break;
			case 4:
				p->typ=5; break;
			case 5:
				p->typ=6; break;
			}
		}

		i+=5+1; // +5 for hand, +1 to skip space

		// extract bid
		int bid_s=i; // store bid beginning idx
		// find end of line
		while(buf[i] != '\n' && buf[i] != EOF) {
			++i;
		}
		char bid_str[i-bid_s+1];
		bid_str[i-bid_s] = '\0';
		for (int k=bid_s; k<i; ++k) {
			bid_str[k-bid_s]=buf[k];
		}
		p->bid = atoi(bid_str);

		// rank play
		ps[p->typ] = rank(ps[p->typ], p);
	}

	// calculate winnings
	int rank = 1;
	int winnings = 0;
	for (int i=0; i<7; ++i) {
		play *ps_i = ps[i];
		while (ps_i) {
			int j=0;
			winnings += rank * ps_i->bid;
			++rank;
			ps_i = ps_i->next;
		}
	}

	play_free(ps);
	return winnings;
}



int main(void) {
	char *fname = "./inputs/d7.in";
	FILE *fptr = fopen(fname, "r");
	fseek(fptr, 0, SEEK_END);
	long count = ftell(fptr);
	rewind(fptr);
	char buf[count*sizeof(char)+1]; // +1 for EOF
	size_t read_bytes = fread(buf, sizeof(char), count, fptr);
	fclose(fptr);
	buf[count*sizeof(char)] = '\n'; // EOF->'\n'

	for (int e=1; e<3; ++e){
		printf("e%d: %d\n", e, ex(buf, count, e));
	}
}