#include <stdio.h>
#include <string.h>

#include "SuadoMathParser.h"

int main() {
	char _input[256];
	smp_Context ctx;
	double result;

	while (1) {
		int c, i = 0;
		while ((c = getchar()) != '\n' && c != EOF && i < 256) {
			_input[i] = (char)c;
			++i;
		}
		_input[i] = '\0';
	
		memset(&ctx.token, 0, sizeof(ctx.token));
		const char* input = _input;
		ctx.it = &input;
	
		smp_Error e = smp_parse(&ctx, &result);
		switch (e) {
		case SMP_ERROR_OKAY:
			printf("%lf\n", result);
			break;
		case SMP_ERROR_INVALID_SYNTAX:
			printf("Invalid syntax\n");
			break;
		case SMP_ERROR_UNEXPECTED_DOT:
			printf("Unexpected '.'\n");
			break;
		case SMP_ERROR_MISSING_OPERATOR:
			printf("Missing operator\n");
			break;
		case SMP_ERROR_MISSING_RIGHT_EXPRESSION:
			printf("Missing right-side expression\n");
			break;
		}
	}
	return 0;
}