#define LEX_IMPLEMENTATION
#define TOOLBOX_IMPLEMENTATION
#include "./utils/lex.h"
#include "./utils/handy.h"

#define SET_COUNT 10

typedef struct {
	int i;
	int sets[SET_COUNT][3];
} game;

int getset(lex_token **toks, int set[3]) {
	if (*(*toks)->v == EOF) {
		return 0;
	}
	if (*(*toks)->v == '\n') {
		return 1;
	}
	int offset = 0;
	char c;
	do {
		offset += 2; // skip '<symbol>  '
		int count = atoi((*(toks+offset))->v);
		offset += 2; // skip '<nr> '
		int pos;
		if (mystrcomp((*(toks+offset))->v, "red")) {
			pos = 0;
		} else if (mystrcomp((*(toks+offset))->v, "green")) {
			pos = 1;
		} else if (mystrcomp((*(toks+offset))->v, "blue")) {
			pos = 2;
		}
		++offset;
		set[pos] = count;
	} while ((c = *(*(toks+offset))->v) != ';' && c != '\n' && c != EOF);
	return offset;
}

void set_printer(int set[3]) {
	for (int k=0; k<3; ++k) {
		int count = set[k];
		switch (k) {
		case 0:
			printf("%d red", count);
			break;
		case 1:
			printf("%d green", count);
			break;
		case 2:
			printf("%d blue", count);
			break;
		}
		if (k<2) printf(", ");
		else printf("; ");
	}
}
void gs_printer(game *gs, int nr_of_games) {
	for (int i = 0; i<nr_of_games; ++i) {
		printf("Game %d: ", i+1);
		for(int j=0; j<SET_COUNT; ++j) {
			set_printer(gs[i].sets[j]);
		}
		putchar('\n');
	}
}

int e1(game *gs, int nr_of_games) {
	int is_valid(int set[3]) {
		int possible[3] = {12,13,14};
		for (int i=0; i<3; ++i) {
			if (set[i] > possible[i]) {
				return 0;
			}
		}
		return 1;
	}
	int sum = 0;
	for (int i=0; i<nr_of_games; ++i) {
		game g = gs[i];
		int valid_count = 0;
		for (int j=0; j<SET_COUNT; ++j) {
			valid_count += is_valid(gs[i].sets[j]);
		}
		if (valid_count == SET_COUNT) {
			sum += g.i;
		}
	}
	return sum;
}

int main(void) {

	size_t token_counter = 0;
	char *fname = "./inputs/d2.in";
	FILE *fptr = fopen(fname, "r");
	LEX_FNAME=fname;
	lex_token **tokens = lex_tokenize(fptr, &token_counter);
	fclose(fptr);

	int nr_of_games = 0;
	for (int i=0; i<token_counter; ++i) {
		lex_token *tok = tokens[i];
		if (tok->t == LEX_SYMBOL && tok->vlen == 1 && 
			(*tok->v == '\n' || *tok->v == EOF)) {
			++nr_of_games;
		}
	}
	printf("nr of games: %d\n", nr_of_games);
	game gs[nr_of_games];
	int offset = 0;
	for (int i=0; i<nr_of_games; ++i) {
		game g;
		g.i = i+1;
		for (int kk=0; kk<SET_COUNT; ++kk) {
			for (int k=0; k<3; ++k) {
				g.sets[kk][k] = 0;
			}
		}
		offset += 3; // skip 'Game <nr>' tokens
		int intern_offset;
		int j=0;
		do {

			intern_offset = getset(tokens+offset, g.sets[j]);
			offset += intern_offset;
			++j;
		} while(intern_offset > 0 && *(*(tokens+offset))->v != '\n');
		gs[i] = g;
		++offset; // skip '\n'
	}
	// gs_printer(gs, nr_of_games);
	lex_free(tokens, token_counter);

	printf("e1: %d\n", e1(gs, nr_of_games));
	return 0;
}