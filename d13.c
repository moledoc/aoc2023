#include <stdio.h>

void zarr(char arr[], size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}

void parr(char arr[], size_t size) {
	for (int i=0; i<size; ++i) {
		printf("%c", arr[i]);
	}
	putchar('\n');
}

void pdarr(int arr[], size_t size) {
	for (int i=0; i<size; ++i) {
		printf("%d ", arr[i]);
	}
	putchar('\n');
}

int min(int a, int b) {
	if (a<b) {
		return a;
	}
	return b;
}


int btod(char b[], size_t size) {
	int pow = 2;
	int d = 0;
	for (int i=size-1-1; i>=0; --i) { // -1 for len-1; -1 for handle bin repr index 0 separately
		d += (b[i] == '1') * pow;
		pow *= 2;
	}
	d += (b[size-1] == '1');
	return d;
}

int reflecto(int arr[], size_t len, int *offset) {
	int rpoint = 0;
	for (int i=*offset; i<len; ++i) {
		if (arr[i-1] == arr[i]) {
			rpoint = i;
			*offset = i+1;
			break;
		}
	}
	int bound = min(rpoint, len-rpoint);
	int ref = 1;
	for (int i=1; i<bound; ++i) {
		if (arr[rpoint-1-i] != arr[rpoint+i]) {
			ref = 0;
			return 0;
		}
	}
	if (rpoint == 0) {
		*offset = len;
	}
	return rpoint;
}

int e1(char buf[], size_t size) {
	int rs = 0;
	int cs = 0;
	int pat_start = 0;

	for (int offset=0; offset<size; ++offset) {
		int rows = 0;
		int cols = 0;
		// int is_new_pat = 1;
		for (int i=offset; i<size; ++i) {
			if (buf[i]=='\n' && cols == 0) {
				cols = (i-pat_start);
				continue;
			}
			if (buf[i] =='\n' && buf[i] == buf[i-1]) {
				rows = (i-pat_start)/(cols+1);
				pat_start = i+1;
				break;
			}
		}
		if (rows == 0) { // handle last pattern
			rows = (size-pat_start)/cols;
		}
		char A[rows][cols];
		char AT[cols][rows];
		for (int i=0; i<rows; ++i) {
			for (int j=0; j<cols; ++j) {
				char v = buf[offset+i*(cols+1)+j];
				A[i][j] = v;
				AT[j][i] = v;
			}
		}
	
		int row_vals[rows];
		zarr((char *)row_vals, cols);
		for (int i=0; i<rows; i++) {
			row_vals[i] = btod(A[i], cols);
		}
	
		int col_vals[cols];
		zarr((char *)col_vals, rows);
		int ridx = 0;
		for (int i=0; i<cols; ++i) {
			col_vals[i] = btod(AT[i], rows);
		}
	
		// parr(A[5], cols);
		// parr(AT[0], rows);
		// printf("%d %d\n", btod(A[0], cols), btod(AT[0], rows));
		// pdarr(row_vals, rows);
		// pdarr(col_vals, cols);

		int roffset = 0;
		while (roffset < rows) {
			rs+=reflecto(row_vals, rows, &roffset);
		}
		int coffset = 0;
		while (coffset < cols) {
			cs+=reflecto(col_vals, cols, &coffset);
		}
		offset+=rows*(cols+1);
	}
	return 100*rs+cs;
}

int main(void) {
	char *fname = "./inputs/d13.in";
	FILE *fptr = fopen(fname, "r");
	fseek(fptr, 0, SEEK_END);
	long size = ftell(fptr);
	rewind(fptr);

	char buf[size];
	fread(buf, sizeof(char), size, fptr);
	fclose(fptr);

	for (int i=0; i<size; ++i) {
		switch (buf[i]) {
		case '.':
			buf[i]='0'; break;
		case '#':
			buf[i]='1'; break;
		}
	}


	printf("e1: %d\n", e1(buf, size));

}