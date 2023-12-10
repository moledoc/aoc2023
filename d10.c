#include <stdio.h>

typedef struct {
	char c; // tile char
	int pos; // cur idx
	int n; // north
	int s; // south
	int e; // east
	int w; // west
} tile;

void print_tile(tile t) {
	printf("char: %c -- pos: %d -- n: %d -- s: %d, -- e: %d -- w: %d\n", t.c, t.pos, t.n, t.s, t.e, t.w);
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

int e2(tile tiles[], tile cur, int came_from, int depth) {
	// printf("following tile:");
	// print_tile(cur);
	int res;
	if (cur.c == 'S' && depth != 0) {
		return depth/2+depth%2;
	}
	if (cur.c == '.') {
		return 0;
	}
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
	// fill tiles
	for (int i=0; i<line_len+2; ++i) {
		tile t1 = {'.', i}; // first row;
		tile t2 = {'.', (line_count+1)*(line_len+2)+i}; // last row
		tiles[i] = t1;
		tiles[(line_count+1)*(line_len+2)+i] = t2;
	}
	for (int i=0; i<line_count+2; ++i) {
		tile c1 = {'.', i*(line_len+2)}; // first col;
		tile c2 = {'.', i*(line_len+2)+line_len+1}; // last col;
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
			tile t = {b_i, pos};
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
				}
				if ((j=i+line_len+1) < size && 
					((b_j=buf[j]) == '|' || b_j == 'L' || b_j == 'J')) {
					t.s = t.pos+(line_len+2);
				}
				if ((j=i+1) < size && 
					((b_j=buf[j]) == '-' || b_j == '7' || b_j == 'J')) {
					t.e = t.pos+1;
				}
				if ((j=i-1) >= 0 && 
					((b_j=buf[j]) == '-' || b_j == 'L' || b_j == 'F')) {
					t.w = t.pos-1;
				}
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
//	printf("e2: %d\n", e2(tiles, tiles[cur], cur, 0));	
}