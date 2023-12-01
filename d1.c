#define LEX_IMPLEMENTATION
#include "./lex.h"

int ex1(char *buf, size_t buf_counter) {

	char w[3];
	int j = 0;
	int sum = 0;
	for (int i=0; i<buf_counter; ++i) {
		char c = buf[i];
		if (c >= '0' && c <= '9') {
			int pos = j > 0 ? 1 : 0;
			w[pos] = c;
			++j;
			continue;
		} else if (c == '\n' || c == EOF || i == buf_counter - 1) {
			if ( j == 1) {
				w[1] = w[0];
			}
			w[2] = '\0';
			j = 0;
			sum += atoi(w);
			continue;
		}
	}
	return sum;
}

int main(void) {
	size_t buf_counter = 0;
	char *fname = "d1.in";
	FILE *fptr = fopen(fname, "r");
	LEX_FNAME = fname;
	char *buf = lex_bufferize(fptr, &buf_counter);
	fclose(fptr);

	printf("sum: %d\n", ex1(buf, buf_counter));

	free(buf);
}