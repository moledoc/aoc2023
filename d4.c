#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

int e1(lex_token **tokens, size_t token_count) {
	int sum = 0;
	void skip(lex_token ***ts) {
		while (*(**ts)->v != ':') { // Skip 'Card <nr>:'
			++(*ts);
		}
	}
	int biggest_nr(lex_token **toks) {
		skip(&toks);
		int biggest = 0;
		while (*(*toks)->v != EOF) {
			if (*(*toks)->v == '\n') {
				skip(&toks);
			}
			if ((*toks)->t == LEX_INT) {
				int v = atoi((*toks)->v);
				biggest = biggest < v ? v : biggest;
			}
			++toks;
		}
		return biggest+1; // account for idx being -1 from len. use +1 so we can comfortably use idx's without indexing outside of arr.
	}
	void zero(int *arr, size_t arr_len) {
		for (int i=0; i<arr_len; ++i) {
			arr[i] = 0;
		}
	}
	int bgst = biggest_nr(tokens);
	// printf("biggest: %d\n", bgst);
	int win_nrs[bgst];
	while (1) { // handle line by line
		skip(&tokens); // skip 'Card <nr>:'
		zero(win_nrs, bgst);
		// collect winning nrs
		while (*(*tokens)->v != '|') {
			if ((*tokens)->t == LEX_INT) {
				int v = atoi((*tokens)->v);
				win_nrs[v] = 1;
			}
			++tokens;
		}
		int match_count = 0;
		// check nrs
		while (*(*tokens)->v != '\n' && *(*tokens)->v != EOF) {
			if ((*tokens)->t == LEX_INT) {
				int v = atoi((*tokens)->v);
				match_count += win_nrs[v];
			}
			++tokens;
		}
		// add to sum
		if (match_count != 0) {
			sum += 1 << (match_count-1);
		}
		// check end of file
		if (*(*tokens)->v == EOF) {
			break;
		}
	}

	return sum;
}

int main(void) {
	size_t token_count = 0;
	char *fname = "./inputs/d4.in";
	LEX_FNAME = fname;
	FILE *fptr = fopen(fname, "r");
	lex_token **tokens = lex_tokenize(fptr, &token_count);
	fclose(fptr);

	printf("e1: %d\n", e1(tokens, token_count));
	// printf("e2: %d\n", e2(tokens, token_count));

	// lex_print(tokens, token_count);
	lex_free(tokens, token_count);
}
	