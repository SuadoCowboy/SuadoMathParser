#include <stdio.h>

#include "SuadoMathParser.h"

// TODO: "expected x thing but received y"
// TODO: calc priority
// TODO: error messages

int main() {
	char input[256] = {0};
	for (int i = 0; i < 256; ++i) {
		input[i] = getchar();
		if (input[i] == '\n') {
			input[i] = '\0';
			break;
		}
	}

	smp_Token token;
	char* it = input;
	while (next_token(&token, &it)) {
		switch (token.type) {
		case SMP_TOKEN_TYPE_UNKNOWN:	printf("unknown  %d\n", token.value); break;
		case SMP_TOKEN_TYPE_NUMBER:		printf("number   %d\n", token.value); break;
		case SMP_TOKEN_TYPE_OPERATOR:	printf("operator %c\n", token.value); break;
		case SMP_TOKEN_TYPE_BRACKET:	printf("bracket  %c\n", token.value); break;
		}
	}

	return 0;
}