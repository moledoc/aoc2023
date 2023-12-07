#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

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

void play_print(play *ps[7]) {
	for (int i=0; i<7; ++i) {
		play *ps_i = ps[i];
		while (ps_i) {
			play *me = ps_i;
			ps_i = ps_i->next;
			printf("typ: %d, hand: %s, bid: %d\n", me->typ, me->hand, me->bid);
		}
	}
}

void zero(int *arr, size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}


int e1(play *ps[7]) {
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
	// printf("read: %d, count: %d, content: %s\n", read_bytes, count, buf);
	buf[count*sizeof(char)] = '\n'; // EOF->'\n'

	play *ps[7];
	for (int i=0;i<7;++i) {
		ps[i] = NULL;
	}
	int label_count = 15;
	for (int i=0; i<count; ++i) {
		int typ[label_count]; // +1 extra for '1' and +1 for accounting diff between len and idx
		zero(typ, label_count);

		char hand[6];
		hand[5] = '\0';

		play *p = calloc(1, sizeof(play));
	
		int j=0;
		for (; j<5; ++j) {
			char c = buf[i+j];
			switch (c) {
			case 'T':
				c='9'+1; break;
			case 'J':
				c='9'+2; break;
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
		// find p->typ
		for (int k=0; k<label_count; ++k) {
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

		i+=j+1; // +1 to skip space

		// extract bid
		j=i; // store bid beginning idx
		// find end of line
		while(buf[i] != '\n' && buf[i] != EOF) {
			++i;
		}
		char bid_str[i-j+1];
		bid_str[i-j] = '\0';
		for (int k=j; k<i; ++k) {
			bid_str[k-j]=buf[k];
		}
		p->bid = atoi(bid_str);

		// rank play
		ps[p->typ] = rank(ps[p->typ], p);
	}

	// play_print(ps);
	printf("e1: %llu\n", e1(ps));

	play_free(ps);

}