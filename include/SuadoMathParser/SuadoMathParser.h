#ifndef SUADOMATHPARSER_H_
#define SUADOMATHPARSER_H_

#include "DLL.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SMP_TOKEN_TYPE_UNKNOWN = 0,
	SMP_TOKEN_TYPE_NUMBER,
	SMP_TOKEN_TYPE_OPERATOR, ///< '+', '-', '*', '/', '%' or '^'.
	SMP_TOKEN_TYPE_BRACKET, ///< '(' or ')'.
	SMP_TOKEN_TYPE_EOF
} smp_TokenType;

typedef enum smp_error_e {
	SMP_ERROR_OKAY = 0, ///< No error ocurred.
	SMP_ERROR_INVALID_SYNTAX,
	SMP_ERROR_UNEXPECTED_DOT,
	SMP_ERROR_MISSING_OPERATOR,
	SMP_ERROR_MISSING_RIGHT_EXPRESSION,
} smp_Error;

typedef struct SMP_API smp_token_t {
	smp_TokenType type;
	union {
		double value;
		char operator; ///< SMP_TOKEN_TYPE_OPERATOR
		char bracket; ///< SMP_TOKEN_TYPE_BRACKET
	};
} smp_Token;

typedef struct SMP_API smp_context_t {
	smp_Token token;
	char const** it;
} smp_Context;

/**
 * @brief Gets the first valid number from string.
 */
smp_Error SMP_API smp_string_to_double(char const **it, double *result);
/**
 * @brief Parses a null-terminated string.
 */
smp_Error SMP_API smp_next_token(smp_Context *ctx);

smp_Error SMP_API smp_parse_primary(smp_Context *ctx, double *left);
smp_Error SMP_API smp_parse_factor(smp_Context *ctx, double *left);
smp_Error SMP_API smp_parse_term(smp_Context *ctx, double *left);
smp_Error SMP_API smp_parse_expression(smp_Context *ctx, double *result);

smp_Error SMP_API smp_parse(smp_Context *ctx, double *result);

#ifdef __cplusplus
}
#endif
#endif // SUADOMATHPARSER_H_