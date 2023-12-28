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
		if (arr[i] < 0) {
			break;
		}
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

int e1_0(char buf[], size_t size) {
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


void pat_zero(int patterns[100][20][20]) {
	for (int p=0; p<100; ++p) {
		for (int r=0; r<20; ++r) {
			for (int c=0; c<20; ++c) {
				patterns[p][r][c]=-1;
			}
		}
	}
}

void pat_print(int pat[20][20]) {
	for (int r=0; r<20; ++r) {
		for (int c=0; c<20; ++c) {
			if (pat[r][c] < 0) {
				break;
			}
			putchar('0'+pat[r][c]);
		}
		if (pat[r][0] < 0) {
			break;
		}
		putchar('\n');
	}
	if (pat[0][0] < 0) {
		return;
	}
	putchar('\n');
}

void pats_print(int pats[100][20][20]) {
	for (int p=0; p<100; ++p) {
		pat_print(pats[p]);
	}
}

// reverse binary to decimal
unsigned long decimal(int arr[], size_t size) {
	unsigned long dec = 0;
	for (int i=1; i<size; ++i) {
		if (arr[i] < 0) {
			break;
		}
		dec += arr[i] << i;
	}
	if (arr[0] < 0) {
		return dec;
	}
	dec += arr[0];
// 	printf("%d: ", dec);
// 	pdarr(arr, size);
	return dec;
}

int ref_point(int pat[20][20]) {

	// find all possible midpoints and bounds
	// check for perf reflection && count rows

	int mid_pts[5];
	zarr((char *)mid_pts, 5);		
	int bounds[5];
	zarr((char*)bounds, 5);
	int cnt = 0;

	int rows = 0;
	for (; rows<20; ++rows) {
		if (pat[rows][0] < 0) {
			break;
		}
	}

	int prev = 0;
	for (int r=0; r<20; ++r) {
		int cur = decimal(pat[r], 20);
		if (!cur) {
			break;
		}
		if (!prev) {
			prev = cur;
			continue;
		}
			
		if (cur == prev) {
			mid_pts[cnt] = r;
			bounds[cnt] = min(r, rows-r);
			++cnt;
		}
		prev = cur;
	}
	// pat_print(pat);

	for (int c=0; c<cnt; ++c) {
		int ref = 1;
		for (int b=1; b<bounds[c]; ++b) {
			if (decimal(pat[mid_pts[c]-1-b],20) != decimal(pat[mid_pts[c]+b],20)) {
	//	printf("%d %d %d %d\n", bounds[c], mid_pts[c], decimal(pat[mid_pts[c]-1-b],20), decimal(pat[mid_pts[c]+b],20));
				ref=0;
				break;
			}
		}
		if (ref && bounds[c]) { // if bounds is 0, then it's not a valid reflection
			return mid_pts[c];
		}
	}
	return 0;
}

int e1(int A[100][20][20], int AT[100][20][20]) {
	int rs = 0;
	int cs = 0;
	for (int p=0; p<100; ++p) {
		rs += ref_point(A[p]);
		cs += ref_point(AT[p]);
	}
	return 100*rs + cs;
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

	int A[100][20][20]; // 100 patterns, max cols=18; max rows=17;
	int AT[100][20][20];
	pat_zero(A);
	pat_zero(AT);
	int pat_cnt = 0;
	int row_cnt = 0;
	int col_cnt = 0;
	for (int i=0; i<size; ++i) {
		switch (buf[i]) {
			case '.': {
				A[pat_cnt][row_cnt][col_cnt]=0;
				AT[pat_cnt][col_cnt][row_cnt]=0;
				++col_cnt;
			} break;
			case '#': {
				A[pat_cnt][row_cnt][col_cnt]=1;
				AT[pat_cnt][col_cnt][row_cnt]=1;
				++col_cnt;
			} break;
			case '\n': {
				if (buf[i-1] == '\n') {
					++pat_cnt;
					row_cnt = 0;
					col_cnt = 0;
				} else {
					++row_cnt;
					col_cnt=0;
				}
			} break;	
		}
	}

	printf("e1: %d\n", e1(A, AT));

}