#pragma once

#ifndef LEX_H_
#define LEX_H_

#include <stdio.h>

enum LEX_TOKEN_TYPE {
	LEX_SYMBOL = 0,
	LEX_WORD,
	LEX_INT,
	LEX_CHAR,
	LEX_STRING,
	LEX_TOKEN_COUNT
};

static const char *LEX_TOKEN_NAMES[] = {
	"LEX_SYMBOL",
	"LEX_WORD",
	"LEX_INT",
	"LEX_CHAR",
	"LEX_STRING"
};

typedef struct {
	enum LEX_TOKEN_TYPE t;
	char *v;
	int vlen;
	char *fname;
	int row;
	int col;
} lex_token;

void lex_free(lex_token **tokens, size_t n);
void lex_string(lex_token *token);
void lex_print(lex_token **token, int token_count);
char *lex_bufferize(FILE *stream, size_t *buf_counter);
lex_token **lex_tokenize_buf(char *buf, size_t buf_counter, size_t *token_counter);
lex_token **lex_tokenize(FILE *stream, size_t *token_counter);

#endif // LEX_H_

#ifdef LEX_IMPLEMENTATION

#include <stdlib.h>
#include <sys/stat.h>

#define LEX_SIZE 256*sizeof(char)
char *LEX_FNAME = "stdin";
int LEX_SKIP_WHITESPACE_CHAR=0;
int LEX_SKIP_WHITESPACE_TOKEN=0;

void lex_free(lex_token **tokens, size_t n) {
	for (int i=0; i<n; ++i) {
		free(tokens[i]->v);
		free(tokens[i]);
	}
	free(tokens);
}

void lex_string(lex_token *token) {
	printf("%s:%d:%d %s(%s)(%d)\n", token->fname, token->row, token->col, LEX_TOKEN_NAMES[token->t], token->v, token->vlen);
}

void lex_print(lex_token **token, int token_count) {
	for (int i=0; i< token_count; ++i) {
		lex_string(token[i]);
	}
}

char *lex_bufferize(FILE *stream, size_t *buf_counter) {
	struct stat stats;
	fstat(fileno(stream), &stats);
	int stats_mode = stats.st_mode;
	char *buf;
	if (S_ISREG(stats_mode) || S_ISFIFO(stats_mode)) {
		size_t size = LEX_SIZE;
		buf = calloc(LEX_SIZE, sizeof(char));
		char c;
		size_t i = 0;
		while (c = fgetc(stream)) {
			if (LEX_SKIP_WHITESPACE_CHAR && (c == ' ' || c == '\t')) {
				continue;
			}
			buf[i] = c;
			++i;
			if (i >= size) {
				size += LEX_SIZE;
				buf = realloc(buf, size);
			}
			if ( c == EOF) {
				break;
			}
		}
		*buf_counter = i;
		buf = realloc(buf, i*sizeof(char));
	} else {
		fprintf(stderr, "%s:%d: [ERROR]: bufferizing failed: unsupported stream provided to tokenizer\n", __FILE__, __LINE__);
		exit(1);
	}
	return buf;		
}

lex_token **lex_tokenize_buf(char *buf, size_t buf_counter, size_t *token_counter) {
	if (LEX_TOKEN_COUNT != sizeof(LEX_TOKEN_NAMES)/sizeof(*LEX_TOKEN_NAMES)) {
		return NULL;
	}
	if (buf == NULL) {
		fprintf(stderr, "%s:%d: [ERROR]: lex_tokenize failed: lex_bufferize returned NULL buffer\n",__FILE__, __LINE__);
		exit(1);
	} 

	int row = 1;
	int col = 1;
	size_t tok_count = 0;
	size_t str_size = LEX_SIZE;
	lex_token **tokens = calloc(buf_counter, sizeof(lex_token *));
	for (int i = 0; i<buf_counter; ++i) {
		if (LEX_SKIP_WHITESPACE_TOKEN && (buf[i] == ' ' || buf[i] == '\t')) {
			continue;
		}
		lex_token *new = calloc(1, sizeof(lex_token));
		char *val = calloc(str_size, sizeof(char));
		new->fname = LEX_FNAME;
		new->row = row;
		new->col = col;
		char b_i = buf[i];
		int vlen = 0;
		if (b_i == '"') { // handle string
			new->t = LEX_STRING;
			++i;
			++col;
			int j = 0;
			int escaped = 0;
			while (i < buf_counter && ((b_i = buf[i]) != '"' || b_i == '"' && escaped)) {
				val[j] = b_i;
				++j;
				++i;
				++col;
				if (j >= str_size) {
					str_size += LEX_SIZE;
					val = realloc(val, str_size);
				}
				int is_slash = b_i == '\\';
				escaped = is_slash * (is_slash ^ escaped);
			}
			val = realloc(val, j*sizeof(char));
			vlen = j;
		} else if (b_i == '\'') { // handle char
			new->t = LEX_CHAR;
			++i;
			++col;
			int j = 0;
			int escaped = 0;
			while (i < buf_counter && ((b_i = buf[i]) != '\'' || b_i == '\'' && escaped)) {
				val[j] = b_i;
				++j;
				++i;
				++col;
				if (j >= str_size) {
					str_size += LEX_SIZE;
					val = realloc(val, str_size);
				}
				int is_slash = b_i == '\\';
				escaped = is_slash * (is_slash ^ escaped);
			}
			val = realloc(val, j*sizeof(char));
			vlen = j;
		} else if (b_i >= 'A' && b_i <= 'Z' || b_i >= 'a' && b_i <= 'z') { // handle word
			new->t = LEX_WORD;
			val[0] = b_i;
			int j = 1;
			++i;
			++col;
			while (i < buf_counter && ((b_i = buf[i]) >= 'a' && b_i <= 'z' || b_i >= 'A' && b_i <= 'Z' || b_i == '-' || b_i == '_' || b_i >= '0' && b_i <= '9')) {
				val[j] = b_i;
				++j;
				++i;
				++col;
				if (i >= str_size) {
					str_size += LEX_SIZE;
					val = realloc(val, str_size);
				}
			}
			val = realloc(val, j*sizeof(char));
			vlen = j;
			--i;
			--col;
		} else if (b_i >= '0' && b_i <= '9' || // handle int
			b_i == '-' && i+1<buf_counter && buf[i+1] >= '0' && buf[i+1] <= '9') { // handle neg int
			new->t = LEX_INT;
			++i;
			++col;
			val[0] = b_i;
			int j = 1;
			while ((b_i = buf[i]) >= '0' && b_i <= '9') {
				val[j] = b_i;
				++i;
				++col;
				++j;
			}
			--i;
			--col;
			val = realloc(val, j*sizeof(char));
			vlen = j;
		} else { // handle symbol
			new->t = LEX_SYMBOL;
			val[0] = b_i;
			if (b_i == '\n') {
				col = 1;
				++row;
			} else {
				++col;
			}
			val = realloc(val, 1*sizeof(char));
			vlen = 1;
		}
		new->v = val;
		new->vlen = vlen;
		tokens[tok_count] = new;
		++tok_count;
	}
	*token_counter = tok_count;
	return tokens;
}

lex_token **lex_tokenize(FILE *stream, size_t *token_counter) {
	if (LEX_TOKEN_COUNT != sizeof(LEX_TOKEN_NAMES)/sizeof(*LEX_TOKEN_NAMES)) {
		return NULL;
	}
	size_t buf_counter = 0;
	char *buf = lex_bufferize(stream, &buf_counter);
	lex_token **tokens = lex_tokenize_buf(buf, buf_counter, token_counter);
	free(buf);
	return tokens;
}

#endif // LEX_IMPLEMENTATION
