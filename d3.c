#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

// top (nw n ne)
//  left *  right (w * e)
// bottom (sw s se)

int e1(lex_token **grid, int rows, int cols) {
	void zero_v(lex_token *tok) {
		char *zero = malloc(1*sizeof(char));
		*zero = '0';
		char *me = tok->v;
		free(me);
		tok->v = zero;
	}
	int rc = rows*cols;
	int sum = 0;
	int j;
	for (int i=0; i<rc; ++i) {
		if (grid[i]->t == LEX_INT || *grid[i]->v == '.') {
			continue;
		}
		// mark values as "0" to not count one part multiple times
		// left
		if ((j=i-1) > 0 && grid[j]->t == LEX_INT) {
			sum += atoi(grid[j]->v);
			zero_v(grid[j]);
		}
		// right
		if ((j=i+1) < rc && grid[j]->t == LEX_INT) {
			sum += atoi(grid[j]->v);
			zero_v(grid[j]);
		}
		// top
		for (int k=1; k>=-1; --k) {
			if ((j=i-cols+k) > 0 && grid[j]->t == LEX_INT) {
				sum += atoi(grid[j]->v);
				zero_v(grid[j]);
			}
		}
		// bottom
		for (int k=1; k>=-1; --k) {
			if ((j=i+cols+k) < rc && grid[j]->t == LEX_INT) {
				sum += atoi(grid[j]->v);
				zero_v(grid[j]);
			}
		}
	}
	return sum;
}

int e2(lex_token **grid, int rows, int cols) {
	void zero_v(lex_token *tok) {
		char *zero = malloc(1*sizeof(char));
		*zero = '0';
		char *me = tok->v;
		free(me);
		tok->v = zero;
	}
	int rc = rows*cols;
	int sum = 0;
	int j;
	int adj_parts = 0;
	for (int i=0; i<rc; ++i) {
		if (grid[i]->t == LEX_INT || *grid[i]->v == '.') {
			continue;
		}
		int mul = 1;
		// mark values as "0" to not count part multiple times
		// left
		if ((j=i-1) > 0 && grid[j]->t == LEX_INT) {
			int v = atoi(grid[j]->v);
			mul *= v ? v : 1;
			if (v) {
				++adj_parts;
			}
			zero_v(grid[j]);
		}
		// right
		if ((j=i+1) < rc && grid[j]->t == LEX_INT) {
			int v = atoi(grid[j]->v);
			mul *= v ? v : 1;
			if (v) {
				++adj_parts;
			}
			zero_v(grid[j]);
		}
		// top
		for (int k=1; k>=-1; --k) {
			if ((j=i-cols+k) > 0 && grid[j]->t == LEX_INT) {
				int v = atoi(grid[j]->v);
				mul *= v ? v : 1;
				if (v) {
					++adj_parts;
				}
				zero_v(grid[j]);
			}
		}
		// bottom
		for (int k=1; k>=-1; --k) {
			if ((j=i+cols+k) < rc && grid[j]->t == LEX_INT) {
				int v = atoi(grid[j]->v);
				mul *= v ? v : 1;
				if (v) {
					++adj_parts;
				}
				zero_v(grid[j]);
			}
		}
		if (adj_parts == 2) {
			sum += mul;
		}
		adj_parts = 0;
	}
	return sum;
}

int main(void) {
	size_t token_count = 0;
	char *fname = "./inputs/d3.in";
	FILE *fptr = fopen(fname, "r");
	LEX_FNAME = fname;
	lex_token **tokens = lex_tokenize(fptr, &token_count);
	fclose(fptr);

	int rows = 0;
	int cols = 0;
	for (int i=0; i<token_count; ++i){
		if (*tokens[i]->v == '\n' || *tokens[i]->v == EOF) {
			++rows;
			continue;
		}
		if (rows == 0) {
			cols += tokens[i]->vlen;
		}
	}
	printf("rows: %d cols: %d\n", rows, cols);

	// mk grid
	lex_token **grid = malloc(rows * cols * sizeof(lex_token *));
	int r = 0;
	int c = 0;
	for (int i=0; i<token_count; ++i){
		lex_token *tok = tokens[i];
		if (*tok->v == '\n' || *tok->v == EOF) {
			++r;
			c = 0;
			continue;
		}
		for (int j=0; j<tok->vlen; ++j) {
			grid[r*cols + (c+j)] = tok;
		}
		c += tok->vlen;
	}

	// print grid
	void print_grid(lex_token **grid, int rows, int cols) {
		for (int i=0; i<rows*cols; ++i) {
			if (i % cols == 0) {
				putchar('\n');
			}
			printf("%s ", grid[i]->v);
		}
	}
	// print_grid(grid, rows, cols);

	// printf("e1: %d\n", e1(grid, rows, cols));
	printf("e2: %d\n", e2(grid, rows, cols));

	// lex_print(tokens, token_count);
	lex_free(tokens, token_count);
	free(grid);
}