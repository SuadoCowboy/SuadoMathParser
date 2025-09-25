#ifndef SUADOMATHPARSER_H_
#define SUADOMATHPARSER_H_

#include "DLL.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SMP_TOKEN_TYPE_UNKNOWN = 0,
	SMP_TOKEN_TYPE_NUMBER,
	SMP_TOKEN_TYPE_OPERATOR,
	SMP_TOKEN_TYPE_BRACKET
} smp_TokenType;

typedef struct SMP_API smp_token_t {
  int type;
  int value;
} smp_Token;

SMP_API int smp_is_digit(char c);
/**
 * @brief Gets the first valid number from string.
 * @return Resulted number.
 */
SMP_API int smp_get_number_from_string(char** it);
/**
 * @brief Parses a null-terminated string.
 * @return 1 if should continue calling next_token; 0 if should stop.
 */
SMP_API int next_token(smp_Token *token, char** it);

#ifdef __cplusplus
}
#endif
#endif // SUADOMATHPARSER_H_