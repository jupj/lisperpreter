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
#define SYMBOLCHARS "abcdefghijklmnopqrstuvwqyzABCDEFGHIJKLMNOPQRSTUVWQYZ0123456789!$,_-./:;?+<=>#%&*@[]{|}`^~"


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
	token *t = malloc(sizeof(token));
	t->type = TT_EOF;

	// Empty data string
	t->data = (char *) malloc(sizeof(char));
	t->data[0] = '\0';
	return t;
}

/*
 * Helper function - emits an error token.
 */
token *error(lexer *l, char *message) {
	token *t = malloc(sizeof(token));
	t->type = TT_ERROR;

	// Copy data string
	t->data = (char *) malloc((strlen(message) + 1)*sizeof(char));
	if (t->data) {
       strcpy(t->data, message);
	} else {
		printf("ERROR: could not allocate memory for token data\n");
		return NULL;
	}
	return t;
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
 * Helper function - checks the next char without movin l->next forward.
 * Returns 1 if next char is within validchars, 0 otherwise.
 */
int is_next(lexer *l, char *validchars) {
	return ((l->next < l->dataend) && (strchr(validchars, *l->next)));
}

/*
 * Helper function - moves l->next forward one char if the char at
 * current l->next is in the supplied string.
 * Returns a pointer to the accepted char if successful, otherwise NULL.
 */
char *accept(lexer *l, char *validchars) {
	if (is_next(l, validchars)) {
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
 * Helper function - check that the next character is a valid "break" after a symbol.
 */
int is_break_next(lexer *l) {
	return (is_next(l, WHITESPACE) || is_next(l, ")") || (l->next >= l->dataend));
}

/*
 * Helper function - lex a whole string and emit the string.
 * Returns the lexed string token.
 */
token *emit_string(lexer *l) {
	if (!accept(l, "\""))
		return error(l, "Expected \" character");

	int escaped = 0;
	while ((l->next < l->dataend) && (!is_next(l, "\""))) {
		if ((!escaped) && is_next(l, "\\")) {
			l->next++;
			escaped = 1;
		} else {
			escaped = 0;
		}
		l->next++;
	}

	if (!accept(l, "\""))
		return error(l, "Expected \" character");

	if (!is_break_next(l))
		return error(l, "Expected whitespace or end of list");

	return emit(l, TT_SYMBOL);
}

/*
 * Lex the next token from the data.
 * Returns the lexed token.
 * Caller must free the token with token_free.
 */
token *lexer_next_token(lexer *l) {
	
	// Skip Whitespace:
	skip(l, WHITESPACE);

	// End of data:
	/*if (('\0' == (*l->next)) || (l->next >= l->dataend)) {*/
	if (l->next >= l->dataend) {
		return eof(l);

	// Parens:
	} else if (accept(l, "(")) {
		return emit(l, TT_OPEN_PAREN);
	} else if (accept(l, ")")) {
		return emit(l, TT_CLOSE_PAREN);
	
	// Quote:
	} else if (accept(l, "'")) {
		if (is_next(l, WHITESPACE))
			return error(l, "Expected an expression directly after quote");
		else
			return emit(l, TT_QUOTE);
	
	// Strings
	} else if (is_next(l, "\"")) {
		return emit_string(l);
	
	// Symbol:
	} else if (acceptRun(l, SYMBOLCHARS)) {
		if (!is_break_next(l))
			return error(l, "Expected whitespace or end of list");
		else
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
