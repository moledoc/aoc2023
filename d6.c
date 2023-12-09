#include <math.h>
#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

int RACE_COUNT=4;

void zero(unsigned long long arr[RACE_COUNT][2]) {
	for (int i=0; i<RACE_COUNT; ++i) {
		for (int j=0; j<2; ++j) {
			arr[i][j] = 0;
		}
	}
}

void prep(lex_token **tokens, unsigned long long races[RACE_COUNT][2]) {
	zero(races);
	int offset = 0;
	int line = 0;
	int race = 0;
	while (*(tokens[offset])->v != EOF) {
		if (*(tokens[offset])->v == '\n') {
			++line;
			race = 0;
		}
		if (tokens[offset]->t == LEX_INT) {
			races[race][line] = strtoull(tokens[offset]->v, NULL, 10);
			++race;
		}
		++offset;
	}
}

int e1(unsigned long long races[RACE_COUNT][2]) {
	int err_marg=1;
	for (int i=0; i<RACE_COUNT; ++i) {
		if (races[i][0] == 0 && races[i][1] == 0) {
			break;
		}

		long double sqrt_a = pow(races[i][0],2)-4*races[i][1];
		long double sqrt_v = sqrt(sqrt_a);
		
		// extract common calculations
		long double xa = (long double)races[i][0]/(long double)2;
		long double sv2 = sqrt_v/(long double)2;

		int low = floor(xa-sv2);
		int up = ceil(xa+sv2-1); // -1 for error correction

		err_marg *= up-low;
	}
	return err_marg;
}

int main(void) {
	char *fname = "./inputs/d6.in";
	LEX_FNAME = fname;
	for (int i=0; i<2; ++i) {
		FILE *fptr = fopen(fname, "r");
		size_t token_count = 0;
		LEX_SKIP_WHITESPACE_CHAR=i;
		lex_token **tokens = lex_tokenize(fptr, &token_count);
		fclose(fptr);

		unsigned long long races[RACE_COUNT][2];
		prep(tokens, races);
		printf("e%d: %d\n",i+1, e1(races));
		lex_free(tokens, token_count);
	}
}