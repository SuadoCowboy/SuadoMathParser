#include "SuadoMathParser.h"

#include <math.h>

#define SMP_IS_EOF(x) ((x) == '\0' || (x) == '\n')
#define SMP_IS_OPERATOR(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '%' || (x) == '^')
#define SMP_ERROR_CHECK(error) if ((error) != SMP_ERROR_OKAY) { return error; }
#define SMP_ERROR_RETURN_IF_EOF_OR_ERROR_IF_NOT_OPERATOR_NOR_CLOSING_BRACKET(tokenType, tokenBracket, error) \
	if ((tokenType) == SMP_TOKEN_TYPE_EOF) { return error; } \
	else if ((tokenType) != SMP_TOKEN_TYPE_OPERATOR && ((tokenType) != SMP_TOKEN_TYPE_BRACKET || (tokenBracket) != ')')) { return SMP_ERROR_MISSING_OPERATOR; }

static inline int smp__is_digit(char c) {
	return c >= '0' && c <= '9';
}

/**
 * @note already skips spacing
 * @param negative 0 = positive; 1 = negative
 */
static inline smp_Error smp__get_number_negativity(char const **it, int *negative) {
	*negative = 0;
	while (!SMP_IS_EOF((*it)[0]) && !smp__is_digit((*it)[0])) {
		switch ((*it)[0]) {
		case '-':
			*negative = !(*negative);
			break;

		case '(':
		case ')':
		case '^':
		case '*':
		case '/':
		case '%':
		case '+':
		case ' ':
			break;

		default:
			return SMP_ERROR_INVALID_SYNTAX;
		}

		*it += sizeof(char);
	}

	return SMP_ERROR_OKAY;
}

static inline void smp__skip_spacing(char const **it) {
	while (!SMP_IS_EOF((*it)[0]) && (*it)[0] == ' ') {
		*it += sizeof(char);
	}
}


smp_Error smp_string_to_double(char const **it, double *result) {
	#define _SMP_STRING_TO_DOUBLE_NUMBER_NEGATIVE 1
	#define _SMP_STRING_TO_DOUBLE_RESULT_CHANGED 2
	#define _SMP_STRING_TO_DOUBLE_DOT_FOUND 4
	int flags = 0, decimal_position = 1;
	*result = 0.0;
	double digit;

	{ // Check if number is negative.
		int negative = 0;
		smp_Error e = smp__get_number_negativity(it, &negative);
		if (e != SMP_ERROR_OKAY) {
			return e;
		}

		if (negative) {
			flags |= _SMP_STRING_TO_DOUBLE_NUMBER_NEGATIVE;
		}
	}

	while (!SMP_IS_EOF((*it)[0])) {
		if ((*it)[0] == '.') {
			if ((flags & _SMP_STRING_TO_DOUBLE_DOT_FOUND) != 0 || (flags & _SMP_STRING_TO_DOUBLE_RESULT_CHANGED) == 0) {
				return SMP_ERROR_UNEXPECTED_DOT;
			}

			flags |= _SMP_STRING_TO_DOUBLE_DOT_FOUND;
			*it += sizeof(char);
			continue;

		} else if (!smp__is_digit((*it)[0])) {
			break;
		}

		flags |= _SMP_STRING_TO_DOUBLE_RESULT_CHANGED;
		digit = (*it)[0] - '0';

		if ((flags & _SMP_STRING_TO_DOUBLE_DOT_FOUND) != 0) {
			for (int i = 0; i < decimal_position; ++i) {
				digit = digit * 0.1;
			}
			++decimal_position;
		} else {
			*result *= 10;
		}

		*result += digit;
		*it += sizeof(char);
	}

	if ((flags & _SMP_STRING_TO_DOUBLE_NUMBER_NEGATIVE) != 0) {
		*result *= -1;
	}

	return SMP_ERROR_OKAY;
	#undef _SMP_STRING_TO_DOUBLE_NUMBER_NEGATIVE
	#undef _SMP_STRING_TO_DOUBLE_RESULT_CHANGED
}

smp_Error smp_next_token(smp_Context* ctx) {
	smp__skip_spacing(ctx->it);
	smp_Error e = SMP_ERROR_OKAY;

	if (smp__is_digit((*ctx->it)[0]) || (ctx->token.type == SMP_TOKEN_TYPE_OPERATOR && (*ctx->it)[0] == '-')) { // only allow passing '-' when the previous token was an operator. Example: "1+-1"
		ctx->token.type = SMP_TOKEN_TYPE_NUMBER;
		e = smp_string_to_double(ctx->it, &ctx->token.value);

	} else if (SMP_IS_OPERATOR((*ctx->it)[0])) {
		ctx->token.type = SMP_TOKEN_TYPE_OPERATOR;
		ctx->token.operator = (*ctx->it)[0];
		*ctx->it += sizeof(char);

	} else if ((*ctx->it)[0] == '(' || (*ctx->it)[0] == ')') {
		ctx->token.type = SMP_TOKEN_TYPE_BRACKET;
		ctx->token.bracket = (*ctx->it)[0];
		*ctx->it += sizeof(char);

	} else if (SMP_IS_EOF((*ctx->it)[0])) {
		ctx->token.type = SMP_TOKEN_TYPE_EOF;
		ctx->token.value = (*ctx->it)[0];
		return e;

	} else {
		ctx->token.type = SMP_TOKEN_TYPE_UNKNOWN;
		ctx->token.value = 0;
		*ctx->it += sizeof(char);
	}
	return e;
}

smp_Error smp_parse_primary(smp_Context *ctx, double *value) {
	smp_Error e = SMP_ERROR_OKAY;

	if (ctx->token.type == SMP_TOKEN_TYPE_NUMBER) {
		*value = ctx->token.value;

		e = smp_next_token(ctx);
		SMP_ERROR_CHECK(e)

		return e;
	} else if (ctx->token.type == SMP_TOKEN_TYPE_BRACKET) {
		if (ctx->token.bracket == '(') {
			e = smp_next_token(ctx);
			SMP_ERROR_CHECK(e)
	
			if (ctx->token.type == SMP_TOKEN_TYPE_EOF) {
				return SMP_ERROR_MISSING_RIGHT_EXPRESSION;
			}
	
			double e;
			e = smp_parse_expression(ctx, value);
			SMP_ERROR_CHECK(e)
	
			if (ctx->token.type == SMP_TOKEN_TYPE_BRACKET && ctx->token.bracket == ')') {
				e = smp_next_token(ctx);
				SMP_ERROR_CHECK(e)
			}
			return e;
		} else { // ')' -> mismatch
			e = SMP_ERROR_INVALID_SYNTAX;
		}
	}

	return e;
}

smp_Error smp_parse_factor(smp_Context *ctx, double *left) {
	smp_Error e = smp_parse_primary(ctx, left);
	SMP_ERROR_CHECK(e)
	SMP_ERROR_RETURN_IF_EOF_OR_ERROR_IF_NOT_OPERATOR_NOR_CLOSING_BRACKET(ctx->token.type, ctx->token.bracket, e)

	while (ctx->token.type == SMP_TOKEN_TYPE_OPERATOR && ctx->token.operator == '^') {
		e = smp_next_token(ctx);
		SMP_ERROR_CHECK(e)

		if (ctx->token.type == SMP_TOKEN_TYPE_EOF) {
			return SMP_ERROR_MISSING_RIGHT_EXPRESSION;
		}

		double right;
		e = smp_parse_primary(ctx, &right);
		SMP_ERROR_CHECK(e)

		*left = pow(*left, right);
	}

	return e;
}

smp_Error smp_parse_term(smp_Context *ctx, double *left) {
	smp_Error e = smp_parse_factor(ctx, left);
	SMP_ERROR_CHECK(e)
	SMP_ERROR_RETURN_IF_EOF_OR_ERROR_IF_NOT_OPERATOR_NOR_CLOSING_BRACKET(ctx->token.type, ctx->token.bracket, e)

	while (ctx->token.type == SMP_TOKEN_TYPE_OPERATOR && (ctx->token.operator == '*' || ctx->token.operator == '/' || ctx->token.operator == '%')) {
		char op = ctx->token.operator;
		e = smp_next_token(ctx);
		SMP_ERROR_CHECK(e)

		if (ctx->token.type == SMP_TOKEN_TYPE_EOF) {
			return SMP_ERROR_MISSING_RIGHT_EXPRESSION;
		}
		
		double right;
		e = smp_parse_factor(ctx, &right);
		SMP_ERROR_CHECK(e)
		
		if (op == '*') *left *= right;
		else if (op == '/') *left /= right;
		else *left = fmod(*left, right);
	}

	return e;
}

smp_Error smp_parse_expression(smp_Context *ctx, double *left) {
	smp_Error e = smp_parse_term(ctx, left);
	SMP_ERROR_CHECK(e)
	SMP_ERROR_RETURN_IF_EOF_OR_ERROR_IF_NOT_OPERATOR_NOR_CLOSING_BRACKET(ctx->token.type, ctx->token.bracket, e)

	while (ctx->token.type == SMP_TOKEN_TYPE_OPERATOR && (ctx->token.operator == '+' || ctx->token.operator == '-')) {
		char op = ctx->token.operator;
		
		e = smp_next_token(ctx);
		SMP_ERROR_CHECK(e)

		if (ctx->token.type == SMP_TOKEN_TYPE_EOF) {
			return SMP_ERROR_MISSING_RIGHT_EXPRESSION;
		}

		double right;
		e = smp_parse_term(ctx, &right);
		SMP_ERROR_CHECK(e)

		if (op == '+') *left += right;
		else *left -= right;
	}

	return e;
}

smp_Error smp_parse(smp_Context *ctx, double *result) {
	*result = 0.0;

	{
		smp_Error e = smp_next_token(ctx);
		SMP_ERROR_CHECK(e)
		if (ctx->token.type == SMP_TOKEN_TYPE_EOF)
			return SMP_ERROR_OKAY;
	}

	return smp_parse_expression(ctx, result);
}