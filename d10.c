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
	for (int row=0; row<line_count; ++row) {
		for (int col=0; col<line_len; ++col) {
			int pos = row*(line_len)+col;
			print_tile(tiles[pos]);
		}
	}
}

void print_grid(tile tiles[], int line_count, int line_len) {
	int in_loop = 0;
	for (int row=0; row<line_count; ++row) {
		for (int col=0; col<line_len; ++col) {
			int pos = row*line_len+col;
			if (tiles[pos].in) {
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
	for (int row=0; row<line_count; ++row) {
		for (int col=0; col<line_len; ++col) {
			int pos = row*line_len+col;
			if (tiles[pos].in) {
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
	tiles[cur.pos].in = 0;
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


void unfill(tile tiles[], int line_count, int line_len, int row, int col) {
	if (row < 0 || row > line_count || col < 0 || col > line_len) {
		return;
	}
	tile t = tiles[row*line_len+col];
	if (t.visited==1) {
		return;
	}
	tiles[row*line_len+col].visited=1;
	if (t.is_pipe) {
		tiles[row*line_len+col].in = 0;
		return;
	}
	tiles[row*line_len+col].in = 0;
	unfill(tiles, line_count, line_len, row, col+1);
	unfill(tiles, line_count, line_len, row+1, col);
	unfill(tiles, line_count, line_len, row-1, col);
	unfill(tiles, line_count, line_len, row, col-1);
}

// TODO add buffer around pipe that is 'not in' by default
// then when doing flood fill from the outside, we can capture 'squeezed' dots + buffer dots are not counted 'in', since other dots are by default 'in' and then marked as 'not in'

void explode(tile exploded_tiles[], int explode_size, tile tiles[], int line_count, int line_len) {
	for (int i=0; i<explode_size; ++i) {
		tile t = {'.', i, 0, 0, 0, 0, 0, 0};
		exploded_tiles[i] = t;
	}
	int row_offset = 0;
	for (int row=1; row<line_count; ++row) {
		int col_offset = 0;
		for (int col=1; col<line_len; ++col) {
			int pos = (row+row_offset)*(2*line_len)+col+col_offset;
			tile t = tiles[row*line_len+col];
			exploded_tiles[pos] = t;
			++col_offset;
			if (t.is_pipe && (t.c == 'S' && t.e || t.c == '-' || t.c == 'F' || t.c == 'L')) {
				tile tt = {'-', pos+1, 0, 0, pos, pos+2, 0, 0, 1};
				tt.is_pipe = 1;
				exploded_tiles[pos+1] = tt;
			}
			if (t.is_pipe && (t.c == 'S' && t.s || t.c == '|' || t.c == 'F' || t.c == '7')) {
				tile tt = {'|', pos+2*line_len, 0, 0, pos, pos+2*(2*line_len), 0, 1};
				tt.is_pipe = 1;
				exploded_tiles[pos+2*line_len] = tt;
			}
		}
		row_offset+=1;
	}
}

int e2(tile tiles[], int line_count, int line_len, tile cur) {
	e1(tiles, cur, cur.pos, 0);

	int exploded_count = 2*(line_count+2);
	int exploded_len = 2*(line_len+2);

	tile exploded_tiles[exploded_count*exploded_len];
	explode(exploded_tiles, exploded_count*exploded_len, tiles, line_count+2, line_len+2);

	print_grid(exploded_tiles, 2*(line_count+2), exploded_len);

	printf("HEERE:%d<-%d %d\n", exploded_count, line_count, exploded_len);
	unfill(exploded_tiles, 2*(line_count+2), exploded_len, 0, 0);
	printf("HEERE:%d<-%d %d\n", exploded_count, line_count, exploded_len);

	print_grid(exploded_tiles, 2*(line_count+2), exploded_len);
	int in_loop = how_many_in_loop(exploded_tiles, 2*(line_count+2), exploded_len);

	return in_loop;
}


int main(void) {
	char *fname = "./inputs/d10.in";
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
				break;
			}
			++i;
			tiles[pos] = t;
		}
	}

	// print tiles
	// print_tiles(tiles, line_count+2, line_len+2);

	// printf("start: ");
	// print_tile(tiles[cur]);
	
	printf("e1: %d\n", e1(tiles, tiles[cur], cur, 0));
	// printf("start -- in_row: %d, in_col: %d\n", in_row, in_col);
	printf("e2: %d\n", e2(tiles, line_count, line_len, tiles[cur]));
}