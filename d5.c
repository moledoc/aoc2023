#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

int MAP_SIZE=100;

void zero(int *arr, size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
}

void parr(int *arr, size_t size) {
	for (int i=0; i<size; ++i) {
		printf("%d ", arr[i]);
	}
	putchar('\n');
}

void pmap(int map[][3]) {
	for (int i=0; i<MAP_SIZE; ++i) {
		printf("%d: ", i);
		for (int j=0; j<3; ++j) {
			printf("%d ", map[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}

int skip_line(lex_token **tokens, int offset) {
	lex_token *tok;
	while (*(tok = tokens[offset])->v != '\n' && *tok->v != EOF) {
		++offset;
	}
	++offset; // go to next line;
	return offset;
}

int get_mapping(lex_token **tokens, int offset, int map[MAP_SIZE][3]) {
	for (int i=0; i<MAP_SIZE; ++i) {
		zero(map[i], 3);
	}
	lex_token *tok;
	offset = skip_line(tokens, offset);
	int j = 0;
	while (*tokens[offset]->v != '\n' && *tokens[offset]->v != EOF){	
		int k = 0;
		while (*(tok = tokens[offset])->v != '\n' && *tok->v != EOF) {
			if (tok->t == LEX_INT) {
				map[j][k] = atoi(tok->v);
				++k;
			}
			++offset;
		}
		++j;
		if (*tok->v != EOF) {
			++offset; // move over from the newline token
		}
	}
	if (*tokens[offset]->v != EOF) {
		++offset; // skip empty newline
	}
	return offset;
}

int e1(int mappings[7][MAP_SIZE][3], int seeds[], size_t seed_count) {
	int lowest = ((int)1 << 30);
	void follow(int mappings[7][MAP_SIZE][3], int m_id, int MAP_SIZE, int val, int *lowest) {
		if (m_id == 7) {
			if (*lowest > val && val > 0) { // MAYBE: BUG: check for overflow for 'val'
				*lowest = val;
			}
			return;
		}
		int in_range = 0;
		for (int i=0; i<MAP_SIZE; ++i) {
			if (mappings[m_id][i][0] == 0 && mappings[m_id][i][1] == 0 && mappings[m_id][i][2] == 0) {
				break;
			}
			if (mappings[m_id][i][1] <= val && val <= mappings[m_id][i][1]+mappings[m_id][i][2]) {
				in_range = 1;
				int nval = mappings[m_id][i][0] + (val - mappings[m_id][i][1]);
				follow(mappings, m_id+1, MAP_SIZE, nval, lowest);
			}
		}
		if (!in_range) {
			follow(mappings, m_id+1, MAP_SIZE, val, lowest);
		}
	}

	for (int i=0; i<seed_count; ++i) {
		follow(mappings, 0, MAP_SIZE, seeds[i], &lowest);
	}
	return lowest;
}

int e2(int mappings[7][MAP_SIZE][3], int seeds[], size_t seed_count) {
	int min(int a, int b) {
		if (a < b) {
			return a;
		}
		return b;
	}
	int max(int a, int b) {
		if (a > b) {
			return a;
		}
		return b;
	}
	int lowest = ((int)1 << 30);
	void follow(int mappings[7][MAP_SIZE][3], int m_id, int MAP_SIZE, int val_range[2], int *lowest) {
		if (m_id == 7) {
			// beginning of range is the smallest
			if (*lowest > val_range[0] && val_range[0] > 0) { // MAYBE: BUG: check for overflow for 'val'
				*lowest = val_range[0];
			}
			return;
		}
		int in_range = 0;
		for (int i=0; i<MAP_SIZE; ++i) {
			if (mappings[m_id][i][0] == 0 && mappings[m_id][i][1] == 0 && mappings[m_id][i][2] == 0) {
				break;
			}
			// find range union
			int infimum = max(val_range[0], mappings[m_id][i][1]);
			int supremum = min(val_range[0] + val_range[1], mappings[m_id][i][1]+mappings[m_id][i][2]);
			// range is invalid if union upper bound is smaller than lower bound
			if (infimum <= supremum) {
				in_range=1;
				int nval_range[2];
				nval_range[0]=mappings[m_id][i][0]+infimum-mappings[m_id][i][1];
				nval_range[1]=supremum - infimum;
				follow(mappings, m_id+1, MAP_SIZE, nval_range, lowest);
			}
		}
		if (!in_range) {
			follow(mappings, m_id+1, MAP_SIZE, val_range, lowest);
		}
	}

	for (int i=0; i<seed_count; i+=2) {
		int vr[2];
		vr[0] = seeds[i];
		vr[1] = seeds[i+1];
		follow(mappings, 0, MAP_SIZE, vr, &lowest);
	}
	return lowest;

}

int main(void) {
	char *fname = "./inputs/d5.in";
	LEX_FNAME = fname;
	size_t token_count = 0;
	FILE *fptr = fopen(fname, "r");
	lex_token **tokens = lex_tokenize(fptr, &token_count);
	fclose(fptr);

	lex_token *tok;
	int seed_count = 0;
	int offset = 0;
	while (*(tok = tokens[offset])->v != '\n') {
		if (tok->t == LEX_INT) {
			++seed_count;
		}
		++offset;
	}


	offset = 0; // reset offset
	int seeds[seed_count];
	int j = 0;
	zero(seeds, seed_count);
	while (*(tok = tokens[offset])->v != '\n') {
		if (tok->t == LEX_INT) {
			seeds[j] = atoi(tok->v);
			++j;
		}
		++offset;
	}
	offset+=2; // skip extra new line
	// parr(seeds, seed_count);

	// legend:
	// mappings[0] = seed_to_soil_map;
	// mappings[1] = soil_to_fert_map;
	// mappings[2] = fert_to_water_map;
	// mappings[3] = water_to_light_map;
	// mappings[4] = light_to_temp_map;
	// mappings[5] = temp_to_hum_map;
	int mappings[7][MAP_SIZE][3];

	offset = get_mapping(tokens, offset, mappings[0]);
	// pmap(mappings[0]);

	offset = get_mapping(tokens, offset, mappings[1]);
	// pmap(mappings[1]);

	offset = get_mapping(tokens, offset, mappings[2]);
	// pmap(mappings[2]);

	offset = get_mapping(tokens, offset, mappings[3]);
	// pmap(mappings[3]);

	offset = get_mapping(tokens, offset, mappings[4]);
	// pmap(mappings[3]);

	offset = get_mapping(tokens, offset, mappings[5]);
	// pmap(mappings[4]);

	offset = get_mapping(tokens, offset, mappings[6]);
	// pmap(mappings[5]);


	printf("e1: %d\n", e1(mappings, seeds, seed_count));
	printf("e2: %d\n", e2(mappings, seeds, seed_count));

	// lex_print(tokens, token_count);
	lex_free(tokens, token_count);
}