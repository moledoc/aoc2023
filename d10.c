#include <stdio.h>

typedef struct {
	char c; // tile char
	int pos; // cur idx
	int n; // north
	int s; // south
	int w; // west
	int e; // east
	int in; // in loop - default 1 to do flood fill on the outside
	int is_pipe;
	int visited;
} tile;

void print_tile(tile t) {
	printf("char: %c -- pos: %d -- n: %d -- s: %d, -- e: %d -- w: %d -- in: %d -- is_pipe: %d\n", t.c, t.pos, t.n, t.s, t.e, t.w, t.is_pipe);
}

void print_tiles(tile tiles[], int line_count, int line_len) {
	for (int row=0; row<line_count+2; ++row) {
		for (int col=0; col<line_len+2; ++col) {
			int pos = row*(line_len+2)+col;
			print_tile(tiles[pos]);
		}
	}
}

void print_grid(tile tiles[], int line_count, int line_len) {
	int in_loop = 0;
	for (int row=0; row<line_count+2; ++row) {
		for (int col=0; col<line_len+2; ++col) {
			int pos = row*(line_len+2)+col;
			if (tiles[pos].c == '.' && tiles[pos].in) {
				printf("*");
			} else if (tiles[pos].c == '.' && !tiles[pos].in) {
				printf("0");
			} else {
				printf("%c", tiles[pos].c);
			}
		}
		putchar('\n');
	}
}
int how_many_in_loop(tile tiles[], int line_count, int line_len) {
	int in_loop = 0;
	for (int row=0; row<line_count+2; ++row) {
		for (int col=0; col<line_len+2; ++col) {
			int pos = row*(line_len+2)+col;
			if (tiles[pos].c == '.' && tiles[pos].in) {
				++in_loop;
			}
		}
	}
	return in_loop;
}

int e1(tile tiles[], tile cur, int came_from, int depth) {
	int res;
	if (cur.c == 'S' && depth != 0) {
		return depth/2+depth%2;
	}
	if (cur.c == '.') {
		return 0;
	}
	tiles[cur.pos].is_pipe = 1;
	tiles[cur.pos].in = 1;
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


void unfill(tile tiles[], int line_count, int line_len, int row, int col, int r_dir, int c_dir) {
	if (row < 0 || row > line_count+1 || col < 0 || col > line_len+1) {
		return;
	}
	// printf("HERE: %d, %d\n", row, col);
	tile t = tiles[row*(line_len+2)+col];
	if (t.visited==1) {
		return;
	}
	tiles[row*(line_len+2)+col].visited=1;
	// print_tile(t);
	if (t.is_pipe) {
		return;
	}
	tiles[row*(line_len+2)+col].in = 0;
	unfill(tiles, line_count, line_len, row, col+c_dir, r_dir, c_dir);
	unfill(tiles, line_count, line_len, row+r_dir, col, r_dir,c_dir);
	unfill(tiles, line_count, line_len, row-r_dir, col, r_dir, c_dir);
	unfill(tiles, line_count, line_len, row, col-c_dir, r_dir,c_dir);
}

// TODO add buffer around pipe that is 'not in' by default
// then when doing flood fill from the outside, we can capture 'squeezed' dots + buffer dots are not counted 'in', since other dots are by default 'in' and then marked as 'not in'

void explode(tile exploded_tiles[], tile tiles[], int line_count, int line_len, int depth) {
	for (int i=0; i<2*line_count*(line_len+2+depth*2); ++i) {
		tile t = {'.', i, 0, 0, 0, 0, 0, 0};
		exploded_tiles[i] = t;
	}
	int row_offset = 0;
	for (int row=1; row<line_count+2; ++row) {
		int col_offset = 0;
		for (int col=1; col<line_len+2; ++col) {
			int pos = (row+row_offset)*(line_len+2+depth*2)+col+col_offset;
			tile t = tiles[row*(line_len+2)+col];
			t.pos = pos;
			exploded_tiles[pos] = t;
			++col_offset;
			if (t.c == 'S' && t.e || t.c == '-' || t.c == 'F' || t.c == 'L') {
				tile tt = {'-', pos+1, 0, 0, pos, pos+2, 0, 0, 1};
				exploded_tiles[pos+1] = tt;
				tt.is_pipe = 1;
			}
			if (t.c == 'S' && t.s || t.c == '|' || t.c == 'F' || t.c == '7') {
				tile tt = {'|', pos+line_len+2+depth*2, 0, 0, pos, pos+2*(line_len+2+depth*2), 0, 1};
				tt.is_pipe = 1;
				exploded_tiles[pos+line_len+2+depth*2] = tt;
			}
		}
		row_offset+=1;
	}
}

int e2(tile tiles[], int line_count, int line_len, tile cur) {
	int depth = e1(tiles, cur, cur.pos, 0);
	// printf("DEPTH=:%d, line_len+2:%d\n", depth, line_len+2);

	tile exploded_tiles[2*line_count*(line_len+2+depth*2)];
	explode(exploded_tiles, tiles, line_count, line_len, depth);
	// print_grid(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2);

	// print_grid(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2);
	unfill(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2, 0, 0, 1, 1);
	unfill(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2, 2*line_count-1, 0, -1, 1);
	unfill(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2, 0, line_len+2+depth*2-1, 1, -1);
	unfill(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2, 2*line_count-1, line_len+2+depth*2-1, -1,-1);

	// printf("-----------------------------------\n");
	// print_grid(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2);
	int in_loop = how_many_in_loop(exploded_tiles, 2*line_count-2, line_len+2+depth*2-2);

	return in_loop;
}


int main(void) {
	char *fname = "./inputs/e1.in";
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
		tile t1 = {'.', i, 0, 0, 0, 0, 1}; // first row;
		tile t2 = {'.', (line_count+1)*(line_len+2)+i, 0, 0, 0, 0, 1}; // last row
		tiles[i] = t1;
		tiles[(line_count+1)*(line_len+2)+i] = t2;
	}
	for (int i=0; i<line_count+2; ++i) {
		tile c1 = {'.', i*(line_len+2), 0, 0, 0, 0, 1}; // first col;
		tile c2 = {'.', i*(line_len+2)+line_len+1, 0, 0, 0, 0, 1}; // last col;
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
			tile t = {b_i, pos, 0, 0, 0, 0, 1};
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
	printf("e2: %d\n", e2(tiles, line_count, line_len, tiles[cur]));
}