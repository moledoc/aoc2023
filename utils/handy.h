#pragma once

#ifndef TOOLBOX_H_
#define TOOLBOX_H_

#include <stdio.h>

typedef struct {
	char **elems;
	size_t count;
} filter;

typedef struct {
	char **files;
	size_t *fp_lens; // file path lengths
	size_t fsize; // files size
	size_t cur_files_count; // current file count
	//
	char **dirs;
	size_t *dp_lens; // directory path lengths
	size_t dsize; // dirs size
	size_t cur_dirs_count; // current directory count
} ftree;

char *shift(int *argc, char ***argv);
void print_str_list(char **lst, size_t lst_size);
void free_str_list(char **lst, size_t lst_size);
size_t mystrlen(const char *s1);
int mystrcomp(const char *s1, const char *s2);
void mymemset(char *buf, int v, size_t size);
void mymemcpy(char *dest, char *src, size_t size);
int contains(char **ss, size_t sslen, char *s);
size_t split(char *s, char sep, char ***elems);
char *ctob(const char c1); // character to binary representation
char btoc(const char *b1); // binary representation to character
void mk_filter(char *filter_str, char sep, filter *flt_o);
void mk_ftree(ftree *ft, size_t arr_size);
void ftree_free(ftree *ft);
void ftree_print(ftree *ft);
void ftree_walk(char *path, size_t path_size, ftree *ft, filter *flt, int depth); // filter: what paths are ignored; depth < 0 means all

#endif // TOOLBOX_H_

#ifdef TOOLBOX_IMPLEMENTATION

#include <stdlib.h>
#include <dirent.h>

char *shift(int *argc, char ***argv) {
	if (*argc <= 0) {
		return NULL;
	}
	char *result = **argv;
	*argc -= 1;
	*argv += 1;
	return result;
}

void print_str_list(char **lst, size_t lst_size) {
	for (int i=0; i<lst_size; ++i) {
		printf("%s\n", lst[i]);
	}
}

void free_str_list(char **lst, size_t lst_size) {
	for (int i=0; i<lst_size; ++i) {
		if (!lst[i]) {
			continue;
		}
		free(lst[i]);
		lst[i] = NULL;
	}
	if (lst) {
		free(lst);
		lst = NULL;
	}
}

size_t mystrlen(const char *s1) {
	char *s1c = (char*)s1;
	size_t size = 0;
	while (*(s1c++) != '\0') {
		++size;
	}
	return size;
}

int mystrcomp(const char *s1, const char *s2) {
	char *s1c = (char *)s1;
	char *s2c = (char *)s2;
	while (*s1c != '\0' && *s2c != '\0') {
		if (*s1c != *s2c) {
			break;
		}
		++s1c;
		++s2c;
	}
	return *s1c == *s2c;
}

void mymemset(char *buf, int v, size_t size) {
	for (size_t i=0; i<size; ++i) {
		buf[i] = v;
	}
}

void mymemcpy(char *dest, char *src, size_t size) {
	for (int i=0; i<size; ++i) {
		dest[i] = src[i];
	}
}

int contains(char **ss, size_t sslen, char *s) {
	for (int i=0; i<sslen; ++i) {
		if (!mystrcomp(ss[i], s)) {
			return 0;
		}
	}
	return 1;
}

size_t split(char *s, char sep, char ***elems) {
	size_t slen = mystrlen(s);
	char *ss = calloc(slen+2, sizeof(char));
	mymemcpy(ss, s, slen);
	ss[slen] = sep;
	ss[slen+1] = '\0';
	size_t elems_count = 0;
	char **es = calloc(elems_count, sizeof(char *));
	int offset = 0;
	for (int i=0; i<slen+1; ++i) {
		if (ss[i] == sep) {
			int len = i-offset;
			es[elems_count] = calloc(len + 1, sizeof(char));
			mymemcpy(es[elems_count], ss + offset, len);
			es[elems_count][len] = '\0';
			offset = i+1;
			++elems_count;
		}
	}
	free(ss);
	*elems = es;
	return elems_count;
}

char *ctob(const char c1) { // character to binary representation
	char c1c = (char)c1;
	size_t blen = 0;
	while ( c1c >>= 1 > 0) {
		++blen;
	}
	blen += 1; // +1 for index 0;
	c1c = (char)c1; 
	char *b = calloc(blen, sizeof(char));
	for (int i=0; i<blen;++i) {
		b[blen-i-1] = c1c & 1 ? '1' : '0';
		c1c >>= 1;
	}
	return b;	
}

char btoc(const char *b1) { // binary representation to character
	char *b1c = (char *)b1;
	int blen = mystrlen(b1c);
	int pow = 2;
	char c = 0;
	for (int i=blen-1-1; i>=0; --i) { // -1 for len-1; -1 for handle bin repr index 0 separately
		c += (b1c[i] == '1') * pow;
		pow *= 2;
	}
	c += (b1c[blen-1] == '1');
	return c;
}

void mk_filter(char *filter_str, char sep, filter *flt_o) {
	char **filter_elems;
	size_t filter_elems_count = split(filter_str, sep, &filter_elems);
	filter flt = {filter_elems, filter_elems_count};
	*flt_o = flt;	
}

void mk_ftree(ftree *ft, size_t arr_size) {
	ft->files = malloc(arr_size*sizeof(char *));
	ft->dirs = malloc(arr_size*sizeof(char *));
	ft->fp_lens = malloc(arr_size*sizeof(size_t));
	ft->dp_lens = malloc(arr_size*sizeof(size_t));
	ft->fsize = arr_size;
	ft->cur_files_count = 0;
	ft->dsize = arr_size;
	ft->cur_dirs_count = 0;
}

void ftree_free(ftree *ft) {
	free_str_list(ft->dirs, ft->cur_dirs_count);
	free_str_list(ft->files, ft->cur_files_count);
	free(ft->fp_lens); 
	ft->fp_lens = NULL;
	free(ft->dp_lens);
	ft->dp_lens = NULL;
}

void ftree_print(ftree *ft) {
	print_str_list(ft->dirs, ft->cur_dirs_count);
	print_str_list(ft->files, ft->cur_files_count);
}

void ftree_walk(char *path, size_t path_size, ftree *ft, filter *flt, int depth) { // filter: what paths are ignored; depth < 0 means all
	if (depth == 0) {
		return;
	}
	DIR *dp;
	struct dirent *ep;
	dp = opendir(path);
	if (!dp) {
		return;
	}
	while (ep = readdir(dp)) {
		if (mystrcomp(".", ep->d_name) || mystrcomp("..", ep->d_name) || 
			contains(flt->elems, flt->count, ep->d_name)) {
			continue;
		}
		size_t ep_name_len = mystrlen(ep->d_name);
		size_t new_path_size = path_size+ep_name_len+1;
		char *new_path = calloc(new_path_size+1, sizeof(char));
		new_path[new_path_size] = '\0';

		mymemcpy(new_path, path, path_size);
		new_path[path_size] = '/';
		mymemcpy(new_path + (path_size+1), ep->d_name, ep_name_len);

		switch (ep->d_type) {
		case DT_DIR:
			ft->dirs[ft->cur_dirs_count] = new_path;
			ft->dp_lens[ft->cur_dirs_count] = new_path_size;
			++(ft->cur_dirs_count);
			if (ft->cur_dirs_count >= ft->dsize) {
				ft->dsize += 32;
				ft->dirs = realloc(ft->dirs, ft->dsize*sizeof(char *));
				ft->dp_lens = realloc(ft->dp_lens, ft->dsize*sizeof(size_t));
			}
			ftree_walk(new_path, new_path_size, ft, flt, depth-1);
			break;
		case DT_REG:
			ft->files[ft->cur_files_count] = new_path;
			ft->fp_lens[ft->cur_files_count] = new_path_size;
			++(ft->cur_files_count);
			if (ft->cur_files_count >= ft->fsize) {
				ft->fsize += 32;
				ft->files = realloc(ft->files, ft->fsize*sizeof(char *));
				ft->fp_lens = realloc(ft->fp_lens, ft->fsize*sizeof(size_t));
			}
			break;
		}
	}
	(void) closedir(dp);
}

#endif // TOOLBOX_IMPLEMENTATION