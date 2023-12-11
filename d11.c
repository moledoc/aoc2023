#include <stdio.h>

// int vs long vs long long vs unsigned long long cost me time again -.-

typedef struct {
	unsigned long long r;
	unsigned long long c;
} coord;

void zero(int arr[], size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}

unsigned long long dabs(long long a) {
	unsigned long long b;
	if (a > 0) {
		return a;
	} 
	return -a;
}

unsigned long long distance(coord a, coord b) {
	return dabs(a.r-b.r)+dabs(a.c-b.c);
}

unsigned long long ex(char *buf, size_t size, int magnitude) {
	int line_len = 0;
	while(buf[line_len]!='\n') {
		++line_len;
	}
	int line_count = (size+1)/(line_len+1); // size+1 - account EOF; line_len+1 - account '\n'

	int empty_rows[line_count];
	zero(empty_rows, line_count);
	int empty_cols[line_len];
	zero(empty_cols, line_len);

	// find empty rows count.
	// count galaxies
	int galaxies_count = 0;
	int empty_row_count = 0;
	for (int row=0; row<line_count; ++row) {
		int is_empty=1;
		for (int col=0; col<line_len; ++col) {
			if (buf[row*(line_len+1)+col] != '.') {
				is_empty=0;
				++galaxies_count;
			}
		}
		if (is_empty) {
			empty_rows[row] = 1;
			++empty_row_count;
		}
	}

	// find empty cols count
	// populate galaxies list
	coord galaxies[galaxies_count];
	int counter = 0;
	int empty_col_count = 0;
	for (int col=0; col<line_len; ++col) {
		int is_empty=1;
		for (int row=0; row<line_count; ++row) {
			if (buf[row*(line_len+1)+col] != '.') {
				is_empty=0;
				coord crd = {row, col};
				galaxies[counter] = crd;
				++counter;
			}
		}
		if (is_empty) {
			empty_cols[col] = 1;
			++empty_col_count;
		}
	}

	int mag = magnitude > 1 ? magnitude - 1 : 1;
	// fix galaxy coordinates after expansion
	for (int i=0; i<galaxies_count; ++i) {
		int c_r = 0;
		for (int j=0; j<galaxies[i].r; ++j) {
			if (empty_rows[j]) {
				c_r+=mag;
			}
		}
		galaxies[i].r+=c_r;
		int c_c = 0;
		for (int j=0; j<galaxies[i].c; ++j) {
			if (empty_cols[j]) {
				c_c+=mag;
			}
		}
		galaxies[i].c+=c_c;
	}

	// find distances and their sum
	unsigned long long sum = 0;
	for (int i=0; i<galaxies_count; ++i) {
		for (int j=i+1; j<galaxies_count; ++j) {
			sum+=distance(galaxies[i], galaxies[j]);
		}
	}
	return sum;
}

int main(void) {
	char *fname = "./inputs/d11.in";
	FILE *fptr = fopen(fname, "r");
	fseek(fptr, 0, SEEK_END);
	long size = ftell(fptr);
	rewind(fptr);
	char buf[size];
	size_t rd = fread(buf, sizeof(char), size, fptr);
	fclose(fptr);

	printf("e1: %d\n", ex(buf, size, 1));
	printf("e2: %llu\n", ex(buf, size, 1000000));
}