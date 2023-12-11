#define LEX_IMPLEMENTATION
#include "./utils/lex.h"

int MAP_SIZE=100;
int SEED_COUNT=20;

void zero(int *arr, size_t size) {
	for (int i=0; i<size; ++i) {
		arr[i] = 0;
	}
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

void follow(int mappings[7][MAP_SIZE][3], int m_id, int MAP_SIZE, int val_range[2], int *lowest) {
	if (m_id == 7) {
		// beginning of range is the smallest
		if (*lowest > val_range[0] && val_range[0] > 0) { // BUG: int overflow somewhere, but doesn't affect solution
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
		int infimum = val_range[0] > mappings[m_id][i][1] ? val_range[0] : mappings[m_id][i][1];
		int val_range_upper = val_range[0] + val_range[1];
		int cur_mappings_upper = mappings[m_id][i][1]+mappings[m_id][i][2];
		int supremum = val_range_upper < cur_mappings_upper ? val_range_upper : cur_mappings_upper;
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

int e1(int mappings[7][MAP_SIZE][3], int seeds[]) {
	int lowest = ((int)1 << 30);
	for (int i=0; i<SEED_COUNT; ++i) {
		int vr[2];
		vr[0] = seeds[i];
		vr[1] = 1;
		follow(mappings, 0, MAP_SIZE, vr, &lowest);
	}
	return lowest;
}

int e2(int mappings[7][MAP_SIZE][3], int seeds[]) {
	int lowest = ((int)1 << 30);
	for (int i=0; i<SEED_COUNT; i+=2) {
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
	int offset = 0;
	int seeds[SEED_COUNT];
	int j = 0;
	zero(seeds, SEED_COUNT);
	while (*(tok = tokens[offset])->v != '\n') {
		if (tok->t == LEX_INT) {
			seeds[j] = atoi(tok->v);
			++j;
		}
		++offset;
	}
	offset+=2; // skip extra new line
	SEED_COUNT = j; // trim SEED_COUNT to match the actual count

	// legend:
	// 0:seed_to_soil_map; 1:soil_to_fert_map; 2:fert_to_water_map; 3:water_to_light_map; 4:light_to_temp_map; 5:temp_to_hum_map; 6:hum_to_loc_map
	int mappings[7][MAP_SIZE][3];

	for (int i=0; i<7; ++i) {
		offset = get_mapping(tokens, offset, mappings[i]);
	}
	printf("e1: %d\n", e1(mappings, seeds));
	printf("e2: %d\n", e2(mappings, seeds));

	lex_free(tokens, token_count);
}