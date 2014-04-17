#include "lex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDEBUG
#ifdef NDEBUG
	#define debug(M, ...)
#else
	#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

/*
 * Characters of lexer
 */
#define WHITESPACE " \t\r\n"
#define QUOTE "'"
#define NUMBERS "0123456789"
#define SYMBOLCHARS "abcdefghijklmnopqrstuvwqyzABCDEFGHIJKLMNOPQRSTUVWQYZ0123456789-?"

/*
 * A struct for the lexer. The members are only used inside this module.
 */
struct Lexer {
	char *data;
	char *dataend;
	char *start;
	char *next;
};

/*
 * Allocate memory and init a lexer.
 * Returns a pointer to the new lexer
 */
lexer *lexer_init() {
	lexer *l = (lexer *) malloc(sizeof(lexer));
	lexer_set_data(l, NULL);
	return l;
}

/*
 * Frees the memory associated with the lexer struct.
 * Does not free the data assigned with lexer_set_data.
 */
void lexer_free(lexer *l) {
	free(l);
}

/*
 * Set the data for the lexer to lex.
 * Resets the next pointer to the beginning of the data.
 *
 * Caller is responsible for freeing data after another call to
 * lexer_set_data or lexer_free.
 */
void lexer_set_data(lexer *l, char *data) {
	l->data = data;
	l->start = data;
	l->next = data;
	if (data) {
		l->dataend = data + strlen(data);
		debug("New data: '%s' (%u)", data, (unsigned)strlen(data));
	}
}

/*
 * Helper function - emits an end-of-file token.
 */
token *eof(lexer *l) {
	debug("EOF");
	return NULL;
}

/*
 * Helper function - emits an error token.
 */
token *error(lexer *l, char *message) {
	printf("ERROR: '%s'\n", message);
	return NULL;
}

/*
 * Helper function - emits a token of the chars between l->start and l->next.
 * Updates l->start to point at l->next.
 */
token *emit(lexer *l, token_tag type) {
	if ((l->start < l->next) && *l->start) {
		token *t = malloc(sizeof(token));
		t->type = type;

		// Copy data string
		int tokenLen = l->next - l->start;
		t->data = (char *) malloc((tokenLen + 1)*sizeof(char));
		strncpy(t->data, l->start, tokenLen);
		t->data[tokenLen] = '\0';

		// Move start pointer forward:
		l->start = l->next;

		return t;
	}

	return error(l, "Invalid lexer state");
}

/*
 * Helper function - moves l->next forward one char if the char at
 * current l->next is in the supplied string.
 * Returns a pointer to the accepted char if successful, otherwise NULL.
 */
char *accept(lexer *l, char *validchars) {
	if ((l->next < l->dataend) && (strchr(validchars, *l->next))) {
		return l->next++;
	}
	return NULL;
}

/*
 * Helper function - moves l->next forward until the char at current l->next
 * is not anymore in the supplied string.
 * Returns a pointer to the accepted char if successful, otherwise NULL.
 */
char *acceptRun(lexer *l, char *validchars) {
	char *result = NULL;
	while (accept(l, validchars)) {
		result = l->next - 1;
	}
	return result;
}

/*
 * Helper function - moves l->next and l->start forward if the char at current l->next
 * is in the supplied string.
 * Returns a pointer to the new l-> start if successful, otherwise NULL.
 */
char *skip(lexer *l, char *skippedchars) {
	char *result = NULL;

	while (accept(l, skippedchars)) {
		l->start++;
		result = l->start;
	}
	return result;
}

/*
 * Lex the next token from the data.
 * Returns the lexed token.
 * Caller must free the token with token_free.
 */
token *lexer_next_token(lexer *l) {
	// End of data:
	/*if (('\0' == (*l->next)) || (l->next >= l->dataend)) {*/
	if (l->next >= l->dataend) {
		return eof(l);

	// Whitespace:
	} else if (acceptRun(l, WHITESPACE)) {
		return emit(l, TT_WHITESPACE);

	// Parens:
	} else if (accept(l, "(")) {
		return emit(l, TT_OPEN_PAREN);
	} else if (accept(l, ")")) {
		return emit(l, TT_CLOSE_PAREN);

	// One-character operators:
	} else if (accept(l, "+")) {
		return emit(l, TT_SUM);
	} else if (accept(l, "-")) {
		return emit(l, TT_SUBTRACT);
	} else if (accept(l, "*")) {
		return emit(l, TT_MULTIPLY);
	} else if (accept(l, "=")) {
		return emit(l, TT_EQUALS);

	// One- or two character operators:
	} else if (accept(l, "/")) {
		if (accept(l, "=")) {
			return emit(l, TT_NOT_EQUALS);
		} else {
			return emit(l, TT_DIVIDE);
		}
	} else if (accept(l, "<")) {
		if (accept(l, "=")) {
			return emit(l, TT_LESS_EQUALS);
		} else {
			return emit(l, TT_LESS);
		}
	} else if (accept(l, ">")) {
		if (accept(l, "=")) {
			return emit(l, TT_GREATER_EQUALS);
		} else {
			return emit(l, TT_GREATER);
		}

	// Numbers:
	} else if (acceptRun(l, NUMBERS)) {
		if (accept(l, ".")) {
			if (acceptRun(l, NUMBERS)) {
				return emit(l, TT_FLOAT);
			} else {
				return error(l, "Invalid number format");
			}
		} else {
			return emit(l, TT_INT);
		}

	// Quotes
	} else if (skip(l, QUOTE)) {
		if (acceptRun(l, SYMBOLCHARS)) {
			return emit(l, TT_QUOTE);
		} else {
			return error(l, "Invalid quote char");
		}

	// Symbols:
	} else if (acceptRun(l, SYMBOLCHARS)) {
		return emit(l, TT_SYMBOL);
	}

	// If we've come this far, there is an unexpected character:
	return error(l, "Invalid character");
}

/*
 * Frees a lexer token.
 */
void token_free(token *t) {
	free(t->data);
	free(t);
}
