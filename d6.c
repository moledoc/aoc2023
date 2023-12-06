#include <math.h>
#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

int RACE_COUNT=4;

void zero(int arr[RACE_COUNT][2]) {
	for (int i=0; i<RACE_COUNT; ++i) {
		for (int j=0; j<2; ++j) {
			arr[i][j] = 0;
		}
	}
}

int e1(int races[RACE_COUNT][2]) {
	int err_marg=1;
	for (int i=0; i<RACE_COUNT; ++i) {
		if (races[i][0] == 0 && races[i][1] == 0) {
			break;
		}
		float sqrt_a = pow(races[i][0],2)-4*races[i][1];
		float sqrt_v = sqrt(sqrt_a);
		int low = (int)(-races[i][0]+sqrt_v)/(-2);
		int up = (int)(-races[i][0]-sqrt_v)/(-2);
		int correction = races[i][0]-up-low-1; // account for trunc residual and 'greater than' condition
		// printf("---- %d: %d %d\n", up-low+correction, up, low);
		err_marg *= up-low+correction;
	}
	return err_marg;
}

int main(void) {
	char *fname = "./inputs/d6.in";
	LEX_FNAME = fname;
	FILE *fptr = fopen(fname, "r");
	size_t token_count = 0;
	lex_token **tokens = lex_tokenize(fptr, &token_count);
	fclose(fptr);

	int races[RACE_COUNT][2];
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
			races[race][line] = atoi(tokens[offset]->v);
			++race;
		}
		++offset;
	}
	for (int i=0;i<RACE_COUNT;++i) {
		// printf("%d %d\n", races[i][0], races[i][1]);
	}
	printf("e1: %d\n", e1(races));
// 	printf("e2: %d\n", e2(races));

	lex_free(tokens, token_count);
}