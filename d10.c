#include <stdio.h>

typedef struct {
	char c; // tile char
	int pos; // cur idx
	int n; // north
	int s; // south
	int e; // east
	int w; // west
	int in; // in loop
	int is_pipe;
	int in_r;
	int in_c;
} tile;

void print_tile(tile t) {
	printf("char: %c -- pos: %d -- n: %d -- s: %d, -- e: %d -- w: %d -- in: %d -- is_pipe: %d -- in_r: %d -- in_c: %d\n", t.c, t.pos, t.n, t.s, t.e, t.w, t.in, t.is_pipe, t.in_r, t.in_c);
}

void print_tiles(tile tiles[], int line_count, int line_len) {
	for (int row=0; row<line_count+2; ++row) {
		for (int col=0; col<line_len+2; ++col) {
			int pos = row*(line_len+2)+col;
			print_tile(tiles[pos]);
		}
	}
}

int e1(tile tiles[], tile cur, int came_from, int depth) {
	// printf("following tile:");
	// print_tile(cur);
	int res;
	if (cur.c == 'S' && depth != 0) {
		return depth/2+depth%2;
	}
	if (cur.c == '.') {
		return 0;
	}
	tiles[cur.pos].is_pipe = 1;
	if (cur.n != came_from && (res=e1(tiles, tiles[cur.n], cur.pos, depth+1))) {
		return res;
	}
	if (cur.s != came_from && (res=e1(tiles, tiles[cur.s], cur.pos, depth+1))) {
		return res;
	}
	if (cur.e != came_from && (res=e1(tiles, tiles[cur.e], cur.pos, depth+1))) {
		return res;
	}
	if (cur.w != came_from && (res=e1(tiles, tiles[cur.w], cur.pos, depth+1))) {
		return res;
	}
}

int find_in(tile tiles[], int line_len, tile cur, int came_from, int in_row, int in_col, int depth) {
	// printf("following tile:");
	// print_tile(cur);
	int res;
	if (cur.c == 'S' && depth != 0) {
		return depth/2+depth%2;
	}
	if (cur.c == '.') {
		return 0;
	}
	// mark diagonals from corners 'in'
	if (cur.n && cur.e) {
		--in_row;
		++in_col;
	} else if (cur.n && cur.w) {
		--in_row;
		--in_col;
	} else if (cur.s && cur.e) {
		++in_row;
		++in_col;
	} else if (cur.s && cur.w) {
		++in_row;
		--in_col;
	} else if (cur.e && cur.w) { // mark correct side from horizontal pipe 'in'
		int sgn = in_row*(line_len+2)>cur.pos ? 1 : -1;
		(tiles)[cur.pos+sgn*(line_len+2)].in = 1;
		in_col = in_col + (cur.pos - came_from);
	} else if (cur.n && cur.s) { // mark correct side from vertical pipe 'in'
		int sgn = (in_col)>cur.pos%(line_len+2) ? 1 : -1;
		(tiles)[cur.pos+sgn].in = 1;
		in_row=in_row + (cur.pos/line_len - in_row);
	}

	// traverse pipes
	if (cur.n != came_from && (res=find_in(tiles, line_len, (tiles)[cur.n], cur.pos, in_row, in_col, depth+1))) {
		return res;
	}
	if (cur.s != came_from && (res=find_in(tiles, line_len, (tiles)[cur.s], cur.pos, in_row, in_col, depth+1))) {
		return res;
	}
	if (cur.e != came_from && (res=find_in(tiles, line_len, (tiles)[cur.e], cur.pos, in_row, in_col, depth+1))) {
		return res;
	}
	if (cur.w != came_from && (res=find_in(tiles, line_len, (tiles)[cur.w], cur.pos, in_row, in_col, depth+1))) {
		return res;
	}
}

int clean_pipes(tile tiles[], int line_len, tile cur, int came_from, int depth){
	int res;
	if (cur.c == 'S' && depth != 0) {
		return depth/2+depth%2;
	}
	if (cur.c == '.') {
		return 0;
	}
	// if pipe is marked 'in', unmark it
	if ((tiles)[cur.pos].in) {
		(tiles)[cur.pos].in = 0;
	}

	// traverse pipes
	if (cur.n != came_from && (res=clean_pipes(tiles, line_len, (tiles)[cur.n], cur.pos, depth+1))) {
		return res;
	}
	if (cur.s != came_from && (res=clean_pipes(tiles, line_len, (tiles)[cur.s], cur.pos, depth+1))) {
		return res;
	}
	if (cur.e != came_from && (res=clean_pipes(tiles, line_len, (tiles)[cur.e], cur.pos, depth+1))) {
		return res;
	}
	if (cur.w != came_from && (res=clean_pipes(tiles, line_len, (tiles)[cur.w], cur.pos, depth+1))) {
		return res;
	}
}

int e2_0(tile tiles[], int line_count, int line_len, tile cur, int came_from, int in_row, int in_col, int depth) {
	find_in(tiles, line_len, cur, came_from, in_row, in_col, depth);
	clean_pipes(tiles, line_len, cur, came_from, depth);
	print_tiles(tiles, line_count, line_len);
	int in = 0;
	for (int row=1; row<line_count+1; ++row) {
		for (int col=1; col<line_len+1; ++col) {
			in += tiles[row*(line_len+2)+col].in;
		}
	}
	return in;
}

int e2(tile tiles[], int line_count, int line_len, tile cur, int came_from, int in_row, int in_col, int depth) {
	e1(tiles, cur, cur.pos, 0);
	for (int row=1; row<line_count+1; ++row) {
		int open_row = 0;
		for (int col=1; col<line_len+1; ++col) {
			if (open_row%2) {
				tiles[row*(line_len+2)+col].in_r = 1;
			}
			tile t = tiles[row*(line_len+2)+col];
			if (t.c == '|') {
				open_row = open_row + (open_row%2 ? 1 : -1);
			} else if (t.c == 'L' ||  (t.c == 'S' && t.s && t.e)) {
				++open_row;
			} else if (t.c == 'F' ||  (t.c == 'S' && t.s && t.e)) {
				++open_row;
			} else if (t.c == '7' ||  (t.c == 'S' && t.s && t.w)) {
				--open_row;
			} else if (t.c == 'J' || (t.c == 'S' && t.n && t.w)) {
				--open_row;
			}
		}
	}
	for (int col=1; col<line_len+1; ++col) {
		int open_col = 0;
		for (int row=1; row<line_count+1; ++row) {
			if (open_col%2) {
				tiles[row*(line_len+2)+col].in_c = 1;
			}
			tile t = tiles[row*(line_len+2)+col];
			if (t.c == '-') {
				open_col = open_col + (open_col%2 ? 1: -1);
			} else if (t.c == 'L' ||  (t.c == 'S' && t.s && t.e)) {
				--open_col;
			} else if (t.c == 'F' ||  (t.c == 'S' && t.s && t.e)) {
				++open_col;
			} else if (t.c == '7' ||  (t.c == 'S' && t.s && t.w)) {
				++open_col;
			} else if (t.c == 'J' || (t.c == 'S' && t.n && t.w)) {
				--open_col;
			}
		}
	}
	int in_count=0;
	for (int row=1; row<line_count+1; ++row) {
		for (int col=1; col<line_len+1; ++col) {
			tile t = tiles[row*(line_len+2)+col];
			if (t.in_r && t.in_c && t.c == '.') {
				// print_tile(t);
				++in_count;
			}
		}
	}
	// printf("\n");
	// print_tiles(tiles, line_count, line_len);
	return in_count;
}


int main(void) {
	char *fname = "./inputs/e2.in";
	FILE *fptr = fopen(fname, "r");
	fseek(fptr, 0, SEEK_END);
	long size = ftell(fptr);
	rewind(fptr);
	char buf[size];
	size_t rd = fread(buf, sizeof(char), size, fptr);
	fclose(fptr);

	// calc grid size
	int line_len = 0;
	while(buf[line_len]!='\n') {
		++line_len;
	}
	int line_count = (size+1)/(line_len+1); // size+1 - account EOF; line_len+1 - account '\n'
	// printf("line len: %d, line count: %d\n", line_len, line_count);

	// make one dim grid with 'ground' buffer
	tile tiles[(line_len+2)*(line_count+2)]; // +2 for either dimension to make a 'ground' area around to avoid bounds checking

	int cur; // populate with starting position index
	int in_row;
	int in_col;
	// fill tiles
	for (int i=0; i<line_len+2; ++i) {
		tile t1 = {'.', i, 0, 0, 0, 0, 0}; // first row;
		tile t2 = {'.', (line_count+1)*(line_len+2)+i, 0, 0, 0, 0, 0}; // last row
		tiles[i] = t1;
		tiles[(line_count+1)*(line_len+2)+i] = t2;
	}
	for (int i=0; i<line_count+2; ++i) {
		tile c1 = {'.', i*(line_len+2), 0, 0, 0, 0, 0}; // first col;
		tile c2 = {'.', i*(line_len+2)+line_len+1, 0, 0, 0, 0, 0}; // last col;
		tiles[i*(line_len+2)] = c1;
		tiles[i*(line_len+2)+line_len+1] = c2;
	}

	int i=0;
	for (int row=1; row<line_count+1; ++row) { // offset to account for 'ground' buffer
		for (int col=1; col<line_len+1; ++col) { // offset to account for 'ground' buffer
			char b_i = buf[i];
			if (b_i == '\n') {
				++i;
				b_i = buf[i];
			}

			int pos = row*(line_len+2)+col;
			tile t = {b_i, pos, 0, 0, 0, 0, 0};
			switch (b_i) {
			// mark the idx where the pipes directs to
			case '|':
				t.n = t.pos-(line_len+2);
				t.s = t.pos+(line_len+2);
				break;
			case '-':
				t.e=t.pos-1;
				t.w=t.pos+1;
				break;
			case 'L':
				t.n = t.pos-(line_len+2);
				t.e = t.pos+1;
				break;
			case 'J':
				t.n = t.pos-(line_len+2);
				t.w = t.pos-1;
				break;
			case '7':
				t.s = t.pos+(line_len+2);
				t.w = t.pos-1;
				break;
			case 'F':
				t.s = t.pos+(line_len+2);
				t.e = t.pos+1;
				break;
			case '.':
				break;
			case 'S':
				cur = pos;
				int j;
				char b_j;
				// TODO: note to explain how direction is set
				if ((j=i-line_len-1) >= 0 && 
					((b_j=buf[j]) == '|' || b_j == 'F' || b_j == '7')) {
					t.n = t.pos-(line_len+2);
					in_row = t.n;
				}
				if ((j=i+line_len+1) < size && 
					((b_j=buf[j]) == '|' || b_j == 'L' || b_j == 'J')) {
					t.s = t.pos+(line_len+2);
					in_row = t.s;
				}
				if ((j=i+1) < size && 
					((b_j=buf[j]) == '-' || b_j == '7' || b_j == 'J')) {
					t.e = t.pos+1;
					in_col = t.e;
				}
				if ((j=i-1) >= 0 && 
					((b_j=buf[j]) == '-' || b_j == 'L' || b_j == 'F')) {
					t.w = t.pos-1;
					in_col = t.w;
				}
				// in_row=row;
				// in_col=col;
				break;
			}
			++i;
			tiles[pos] = t;
		}
	}

	// print tiles
	// print_tiles(tiles, line_count, line_len);

	// printf("start: ");
	// print_tile(tiles[cur]);
	
	printf("e1: %d\n", e1(tiles, tiles[cur], cur, 0));
	// printf("start -- in_row: %d, in_col: %d\n", in_row, in_col);
	printf("e2: %d\n", e2(tiles, line_count, line_len, tiles[cur], cur, in_row, in_col, 0));
}