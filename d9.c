#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

size_t LINE_LEN=0;

	void print_grid(int grid[LINE_LEN][LINE_LEN]) {
		for (int i=0; i<LINE_LEN; ++i) {
			for (int j=0; j<LINE_LEN; ++j) {
				printf("%d ", grid[i][j]);
			}
			putchar('\n');
		}
		printf("----------------------\n");
	}


int predict(int grid[LINE_LEN][LINE_LEN], int offset) {
	// print_grid(grid);
	int zero_line = 1;
	for (int j=0; j<LINE_LEN-offset-1; ++j) { // last elem has no further elems to diff with
		int v = grid[offset][j+1] - grid[offset][j];
		grid[offset+1][j] = v;
		if (v != 0) {
			zero_line = 0;
		}
	}
	// print_grid(grid);
	if (zero_line) {
		return grid[offset][LINE_LEN-offset-1];
	}
	return grid[offset][LINE_LEN-offset-1] + predict(grid, offset+1);
}

long e1(lex_token **tokens, size_t token_count) {
	size_t line_count = 1; // read last line up front - don't have to handle EOF char then
	
	for (int i=0; i<token_count; ++i) {
		if (*tokens[i]->v != '\n') {
			continue;
		}
		if (LINE_LEN == 0) {
			LINE_LEN = i;
		}
		++line_count;
	}


	int grid[LINE_LEN][LINE_LEN];
	// zero
	void zero(int grid[LINE_LEN][LINE_LEN]) {
	for (int i=0; i<LINE_LEN; ++i) {
		for (int j=0; j<LINE_LEN+1; ++j) {
			grid[i][j] = 0;
		}
	}
	}

	long history = 0;
	// analyse
	for (int i=0; i<line_count; ++i) { // i+=LINE_LEN+2 to jump over newline char and land on next line token
		zero(grid);
		for (int j=0; j<LINE_LEN; ++j) {
			int v = atoi(tokens[i*(LINE_LEN+1)+j]->v); // +1 to jump over newline char
			grid[0][j] = v;
		}
		long pred = predict(grid, 0);
		// printf("%d predict: %ld\n", i+1, pred);
		history += pred;
	}
	return history;
}

int main(void) {
	char *fname = "./inputs/d9.in";
	LEX_FNAME = fname;
	LEX_SKIP_WHITESPACE_TOKEN = 1;
	FILE *fptr = fopen(fname, "r");
	size_t token_count = 0;
	lex_token **tokens = lex_tokenize(fptr, &token_count);


	printf("e1: %ld\n", e1(tokens, token_count));

	// lex_print(tokens, token_count);
	lex_free(tokens, token_count);

}