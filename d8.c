#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

typedef struct node {
	lex_token *tok;
	lex_token *lr[2];
} node;

int mystrcomp(const char *s1, const char *s2) {
	char *s1c = (char *)s1;
	char *s2c = (char *)s2;
	while (*s1c != '\0' && *s2c != '\0') {
		if (*s1c != *s2c && *s1c != '.') {
			break;
		}
		++s1c;
		++s2c;
	}
	return *s1c == *s2c;
}

int e1(node ****trie, lex_token *instr, char *start, char *end) {
	int steps = 0;
	// set starting point
	node *cur_node = trie[start[0]-'A'][start[1]-'A'][start[2]-'A'];
	lex_token *cur_tok = cur_node->tok;
	while (1) {
		// helper vars: node and instruction
		char *cur_tok_v = cur_tok->v;
		int ins = instr->v[steps%instr->vlen]-'0'; // instructions are in form '0' and '1';
		// next token
		cur_tok = trie[cur_tok_v[0]-'A'][cur_tok_v[1]-'A'][cur_tok_v[2]-'A']->lr[ins];
		++steps;
		if (mystrcomp(end, cur_tok->v)) {
			break;
		}
		// find next node
		cur_node = trie[cur_tok_v[0]-'A'][cur_tok_v[1]-'A'][cur_tok_v[2]-'A'];
	}
	return steps;
}

unsigned long min(unsigned long a, unsigned long b) {
	return a<b ? a : b;
}

unsigned long max(unsigned long a, unsigned long b) {
	return a>b ? a : b;
}

unsigned long gcd(unsigned long a, unsigned long b) {
	unsigned long min_ab = min(a,b);
	if (min_ab == 0) {
		return max(a,b);
	}
	unsigned long a1 = max(a,b)%min_ab;
	return gcd(a1, min_ab);
}

unsigned long lcm(unsigned long a, unsigned long b) {
	unsigned long gcd_ab = gcd(a,b);
	return a*b/gcd_ab;
}

unsigned long e2(node ****trie, lex_token *instr) {
	// find starting points
	char **cur = calloc(10, sizeof(char *));
	int cur_count = 0;
	for (int i=0; i<26; ++i) {
		if (trie[i] == NULL) continue;
		for (int j=0; j<26; ++j) {
			if (trie[i][j] == NULL) continue;
			for (int k=0; k<26; ++k) {
				if (trie[i][j][k] == NULL) continue;
				if (trie[i][j][k]->tok == NULL) continue;
				if (trie[i][j][k]->tok->v[2] == 'A') {
					cur[cur_count] = trie[i][j][k]->tok->v;
					++cur_count;
				}
			}
		}
	}

	// find each starting point steps
	// then find least common multiplier (lcm)
	unsigned long steps = (unsigned long)e1(trie, instr, cur[0], "..Z");
	for (int i=1; i<cur_count; ++i) {
		steps = lcm(steps, (unsigned long)e1(trie, instr, cur[i], "..Z"));
	}

	free(cur);
	return steps;
}

int main(void) {
	char *fname = "./inputs/d8.in";
	LEX_FNAME = fname;
	LEX_SKIP_WHITESPACE_CHAR = 1;
	FILE *fptr = fopen(fname, "r");
	size_t token_count = 0;
	lex_token **tokens = lex_tokenize(fptr, &token_count);

	// initialize trie
	node ****trie = calloc(26, sizeof(node ***));
	for (int i=0; i<26; ++i) {
		trie[i] = calloc(26, sizeof(node **));
		for (int j=0; j<26; ++j) {
			trie[i][j] = calloc(26, sizeof(node *));
			for (int k=0; k<26; ++k) {
				trie[i][j][k] = calloc(26, sizeof(node));
			}
		}
	}

	// populate trie
	for (int i=3; i<token_count; i+=8) { // i=3 - skip '<instructions>\n\n'; i+=8 <- LEX_SKIP_WHITESPACE=1, so skipping '=(<node>,<node>)\n'
		node *n = calloc(1, sizeof(node));
		if (tokens[i]->vlen <= 1) {
			continue;
		}
		n->tok = tokens[i];
		n->lr[0] = tokens[i+3];
		n->lr[1] = tokens[i+5];
		char v0 = n->tok->v[0]-'A';
		char v1 = n->tok->v[1]-'A';
		char v2 = n->tok->v[2]-'A';
		trie[v0][v1][v2] = n;
	}

	// convert instruction
	lex_token *instr = tokens[0];
	for (int i=0; i<instr->vlen; ++i) {
		instr->v[i] = instr->v[i] == 'L' ? '0' : '1';
	}

	printf("e1: %d\n", e1(trie, instr, "AAA", "ZZZ"));
	printf("e2: %llu\n", e2(trie, instr));

	// WOULDBENICE: free nodes in trie

	lex_free(tokens, token_count);
}