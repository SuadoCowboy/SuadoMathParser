#include "SuadoMathParser.h"

int smp_is_digit(char c) {
	return c >= '0' && c <= '9';
}

/**
 * @brief Gets the first valid number from string.
 * @return Resulted number.
 */
int smp_get_number_from_string(char** it) {
	int flags = 0; ///< 1 = negative; 2 = updated it at least once
	int result = 0;

	while ((*it)[0] != '\0' && (*it)[0] != '\n') {
		if ((*it)[0] == ' ') {
			(*it) += sizeof(char);
			flags |= 2;
			continue;
		}

		if ((*it)[0] == '-') {
			// Set negative to true or false.
			if ((flags & 1) == 0)
				flags |= 1;
			else
				flags &= ~1;
			continue;
		}

		if (!smp_is_digit((*it)[0])) {
			break;
		}

		result = result * 10 + (*it)[0] - '0';
		(*it) += sizeof(char);
		flags |= 2;
	}

	if ((flags & 1) != 0)
		result *= -1;

	if ((flags & 2) != 0)
		(*it) -= sizeof(char);

	return result;
}

/**
 * @brief Parses a null-terminated string.
 * @return 1 if should continue calling next_token; 0 if should stop.
 */
int next_token(smp_Token *token, char** it) {
	while ((*it)[0] == ' ')
		++(*it);
	
	if (smp_is_digit((*it)[0])) {
		token->type = SMP_TOKEN_TYPE_NUMBER;
		token->value = smp_get_number_from_string(it);

	} else if ((*it)[0] == '+' || (*it)[0] == '-' || (*it)[0] == '*' || (*it)[0] == '/') {
		token->type = SMP_TOKEN_TYPE_OPERATOR;
		token->value = (*it)[0];

	} else if ((*it)[0] == '(' || (*it)[0] == ')') {
		token->type = SMP_TOKEN_TYPE_BRACKET;
		token->value = (*it)[0];

	} else if ((*it)[0] == '\0' || (*it)[0] == '\n') {
		return 0;

	} else {
		token->type = SMP_TOKEN_TYPE_UNKNOWN;
		token->value = -1;
	}

	*it += sizeof(char);
	return 1;
}