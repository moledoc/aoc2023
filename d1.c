#include <stdio.h>
#include <stdlib.h>

void mymemset(char *dest, char c, size_t len) {
	for (int i=0; i<len; ++i) {
		dest[i] = c;
	}
}

int e1(char *buf, size_t buf_counter) {
	char w[3];
	mymemset(w, '\0', 3);
	int j = 0;
	int sum = 0;
	for (int i=0; i<buf_counter; ++i) {
		j = j > 0 ? 1 : 0;
		char c = buf[i];
		if (c >= '0' && c <= '9') {
			w[j] = c;
			++j;
		} else if (c == '\n' || c == EOF) {
			if ( w[1] == '\0') {
				w[1] = w[0];
			}
			j = 0;
			sum += atoi(w);
			w[1] = '\0';
			w[0] = '0';
		}
	}
	return sum;
}

int equal(const char *s1, const char *s2, int len) {
	char *s1c = (char *)s1;
	char *s2c = (char *)s2;
	int i = 0;
	while (*s1c != '\0' && *s2c != '\0' && i < len) {
		if (*s1c != *s2c) {
			return 0;
		}
		++s1c;
		++s2c;
	}
	return 1;
}

int e2(char *buf, size_t buf_counter) {
	char mapping[10][6] = {"zero\0", "one\0", "two\0", "three\0", "four\0", "five\0", "six\0", "seven\0", "eight\0", "nine\0",};
	int mapping_lens[10] = {4, 3, 3, 5, 4, 4, 3, 5, 5, 4};

	char w[3];
	mymemset(w, '\0', 3);
	int j = 0;
	int sum = 0;
	for (int i=0; i<buf_counter; ++i) {
		j = j > 0 ? 1 : 0;
		char c = buf[i];
		if (c >= '0' && c <= '9') {
			w[j] = c;
			++j;
		} else if (c == '\n' || c == EOF) {
			if ( w[1] == '\0') {
				w[1] = w[0];
			}
			j = 0;
			sum += atoi(w);
			w[1] = '\0';
			w[0] = '0';
		} else {
			for (int k=0; k<10; ++k) {
				if (equal(buf+i, mapping[k], mapping_lens[k])) {
					w[j] = '0' + k;
					++j;
				}
			}
		}
	}
	return sum;
}

int main(void) {
	char *fname = "./inputs/d1.in";
	FILE *fptr = fopen(fname, "r");
	fseek(fptr, 0L, SEEK_END);
	long buf_counter = ftell(fptr) + 1; // +1 to count EOF
	char buf[buf_counter];
	rewind(fptr);
	for (int i=0; i<buf_counter+1;++i) {
		buf[i] = fgetc(fptr);
	}

	printf("e1: %d\n", e1(buf, buf_counter));
	printf("e2: %d\n", e2(buf, buf_counter));

}