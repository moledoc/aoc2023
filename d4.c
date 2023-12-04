#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

void zero(int *arr, size_t arr_len) {
	for (int i=0; i<arr_len; ++i) {
		arr[i] = 0;
	}
}

void skip(lex_token ***ts) {
	while (*(**ts)->v != ':') { // Skip 'Card <nr>:'
		++(*ts);
	}
}

void find_some_nrs(lex_token **toks, int *win_nr_size, int *line_count) {
	int lines = 0;
	skip(&toks);
	int biggest = 0;
	while (*(*toks)->v != EOF) {
		if (*(*toks)->v == '\n') {
			++lines;
			skip(&toks);
		}
		if ((*toks)->t == LEX_INT) {
			int v = atoi((*toks)->v);
			biggest = biggest < v ? v : biggest;
		}
		++toks;
	}
	if (line_count != NULL) {
		*line_count = lines + 1; // +1 for last line
	}
	if (win_nr_size != NULL) {
		*win_nr_size = biggest+1; // account for idx being -1 from len. use +1 so we can comfortably use idx's without indexing outside of arr.
	}
}

int e1(lex_token **tokens) {
	int win_nr_size;
	find_some_nrs(tokens, &win_nr_size, NULL);
	int win_nrs[win_nr_size];

	int sum = 0;
	while (1) { // handle line by line
		skip(&tokens); // skip 'Card <nr>:'
		zero(win_nrs, win_nr_size);
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

int e2(lex_token **tokens, size_t token_count) {
	int cards_count;
	int win_nr_size;
	find_some_nrs(tokens, &win_nr_size, &cards_count);

	int win_nrs[win_nr_size];
	int copies[cards_count];
	zero(copies, cards_count);

	int j = 0;
	while (1) { // handle line by line
		skip(&tokens); // skip 'Card <nr>:'
		zero(win_nrs, win_nr_size);
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
		// add to copies
		for (int k=0; k<match_count; ++k) {
			copies[j+1+k] += 1 + copies[j]; // win copies from 1 original and copies[j] copies
		}
		// add current line copies to cards;
		cards_count += copies[j];
		++j;
		// check end of file
		if (*(*tokens)->v == EOF) {
			break;
		}
	}
	return cards_count;
}

int main(void) {
	size_t token_count = 0;
	char *fname = "./inputs/d4.in";
	LEX_FNAME = fname;
	FILE *fptr = fopen(fname, "r");
	lex_token **tokens = lex_tokenize(fptr, &token_count);
	fclose(fptr);

	printf("e1: %d\n", e1(tokens));
	printf("e2: %d\n", e2(tokens, token_count));

	// lex_print(tokens, token_count);
	lex_free(tokens, token_count);
}
	