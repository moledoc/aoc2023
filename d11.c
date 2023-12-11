#include <stdio.h>

int ROWS;
int COLS;

typedef struct {
	int r;
	int c;
} coord;

void print_coord(coord crd) {
	printf("row: %d, col: %d\n", crd.r, crd.c);
}

void zero(int arr[], size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}

void print_grid(int grid[ROWS][COLS]) {
	for (int row=0; row<ROWS; ++row) {
		for (int col=0; col<COLS; ++col) {
			printf("%d ", grid[row][col]);
		}
		putchar('\n');
	}
}

	int vabs(int a, int b) {
		int diff = a-b;
		int sign = diff > 0 ? 1 : -1;
		return sign*diff;
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

	//printf("%s\n", buf);
	// calc grid size
	int line_len = 0;
	while(buf[line_len]!='\n') {
		++line_len;
	}
	int line_count = (size+1)/(line_len+1); // size+1 - account EOF; line_len+1 - account '\n'
	printf("line len: %d, line count: %d\n", line_len, line_count);

	int empty_rows[line_count];
	zero(empty_rows, line_count);
	int empty_cols[line_len];
	zero(empty_cols, line_len);


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

	//printf("empty row col: %d %d\n", empty_row_count, empty_col_count);
	for (int i=0; i<line_count; ++i) {
		//printf("%d ", empty_rows[i]);
	}
	//putchar('\n');
	for (int i=0; i<line_len; ++i) {
		//printf("%d ", empty_cols[i]);
	}
	//putchar('\n');

	ROWS=line_count+empty_row_count;
	COLS=line_len+empty_col_count;
	int grid[ROWS][COLS];
	for (int row=0; row<ROWS; ++row) {
		for (int col=0; col<COLS; ++col) {
			grid[row][col] = 0;
		}
	}
	
	for (int i=0; i<galaxies_count; ++i) {
		//printf("before: \n");
		//print_coord(galaxies[i]);
		int counter = 0;
		for (int j=0; j<galaxies[i].r; ++j) {
			if (empty_rows[j]) {
				++counter;
			}
		}
		galaxies[i].r+=counter;
		counter = 0;
		for (int j=0; j<galaxies[i].c; ++j) {
			if (empty_cols[j]) {
				++counter;
			}
		}
		galaxies[i].c+=counter;
		//printf("after: \n");
		//print_coord(galaxies[i]);
	}

	// print_grid(grid);

	int cnt = 0;
	int sum = 0;
	for (int i=0; i<galaxies_count; ++i) {
		int shortest=1<<16;
		int shortest_idx;
		for (int j=i+1; j<galaxies_count; ++j) {
			if (i==j) {
				continue;
			}
			++cnt;
			int dist_r = vabs(galaxies[i].r, galaxies[j].r);
			int dist_c = vabs(galaxies[i].c, galaxies[j].c);
			int dist = dist_r+dist_c;
			sum+=dist;
			// printf("(%d,%d)--(%d,%d) distance: %d %d %d %d\n", galaxies[i].r,galaxies[i].c, galaxies[j].r,galaxies[j].c, dist_r, dist_c, dist_r+dist_c, dist);
			if (dist < shortest) {
				shortest=dist;
				shortest_idx=j;
			}
		}
		//printf("shortest dist for %d is with %d: %d\n", i, shortest_idx, shortest);
	}

	printf("count: %d\n", cnt);
	printf("e1: %d\n", sum);
	// printf("e1: %d\n", e1());
	// printf("e2: %d\n", e2());
}