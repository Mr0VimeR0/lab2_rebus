#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char* slags[7];
	int slag_count;
	char* result;
	char letters[26];
	int letter_count;
	int numbers[26];
	int used[10];
} Rebus;

char* read_line(FILE* f) {
	int cap = 16;
	int length = 0;
	char* buf = malloc(cap);
	if (buf == NULL) return NULL;
	int c;
	while ((c = fgetc(f)) != EOF && c != '\n') {
		if (length + 1 >= cap) {
			cap *= 2;
			char* temp = realloc(buf, cap);
			if (temp == NULL) {
				free(buf);
				return NULL;
			}
			buf = temp;
		}
		buf[length] = (char)c;
		length++;
	}
	if (length == 0 && c == EOF) {
		free(buf);
		return NULL;
	}
	if (length > 0 && buf[length - 1] == '\r') length--;
	buf[length] = '\0';
	return buf;
}

int parser(char* line, Rebus* reb) {
	memset(reb, 0, sizeof(*reb));
	char* tokens[8];
	int n = 0;
	char* tok = strtok(line, " +=");
	while (tok != NULL) {
		tokens[n] = tok;
		n++;
		tok = strtok(NULL, " +=");
	}
	reb->result = tokens[n - 1];
	reb->slag_count = n - 1;
	for (int i = 0; i < reb->slag_count; i++) {
		reb->slags[i] = tokens[i];
	}
	int see[26] = { 0 };
	for (int i = 0; i < reb->slag_count; i++) {
		for (int j = 0; reb->slags[i][j] != '\0'; j++) {
			char c = reb->slags[i][j];
			if (!see[c - 'A']) {
				see[c - 'A'] = 1;
				reb->letters[reb->letter_count] = c;
				reb->letter_count++;
			}
		}
	}
	for (int i = 0; reb->result[i] != '\0'; i++) {
		char c = reb->result[i];
		if (!see[c - 'A']) {
			see[c - 'A'] = 1;
			reb->letters[reb->letter_count] = c;
			reb->letter_count++;
		}
	}
	for (int i = 0; i < 26; i++) {
		reb->numbers[i] = -1;
	}
	return 1;
}

long long transfer(char* word, int* numbers) {
	long long n = 0;
	for (int i = 0; word[i] != '\0'; i++) {
		n = n * 10 + numbers[word[i] - 'A'];
	}
	return n;
}

int check_solution(Rebus* reb) {
	for (int i = 0; i < reb->slag_count; i++) {
		if (reb->slags[i][1] != '\0' && reb->numbers[reb->slags[i][0] - 'A'] == 0) return 0;
	}
	if (reb->result[1] != '\0' && reb->numbers[reb->result[0] - 'A'] == 0) return 0;
	long long sum = 0;
	for (int i = 0; i < reb->slag_count; i++) {
		sum += transfer(reb->slags[i], reb->numbers);
	}
	return sum == transfer(reb->result, reb->numbers);
}

int solve(Rebus* reb, int index) {
	if (index == reb->letter_count) return check_solution(reb);
	int letter_index = reb->letters[index] - 'A';
	for (int i = 0; i <= 9; i++) {
		if (reb->used[i]) continue;
		reb->used[i] = 1;
		reb->numbers[letter_index] = i;
		if (solve(reb, index + 1)) return 1;
		reb->used[i] = 0;
		reb->numbers[letter_index] = -1;
	}
	return 0;
}

void print_solution(Rebus* reb) {
	for (int i = 0; i < reb->slag_count; i++) {
		if (i > 0) printf(" + ");
		printf("%lld", transfer(reb->slags[i], reb->numbers));
	}
	printf(" = %lld\n", transfer(reb->result, reb->numbers));
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Use fille to solve problems\n");
		return 0;
	}
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		printf("Error open file\n");
		return 0;
	}
	char* line;
	while ((line = read_line(f)) != NULL) {
		Rebus reb;
		if (parser(line, &reb)) {
			if (solve(&reb, 0)) print_solution(&reb);
			else printf("No solutions: %s\n", line);
		}
		else printf("Parse Error\n");
		free(line);
	}
	fclose(f);
	return 0;
}