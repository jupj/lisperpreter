#ifndef lex_h
#define lex_h

/*
 * enum of lexer token types
 */
typedef enum Token_tag {
	TT_ERROR,
	TT_EOF,

	TT_OPEN_PAREN, 
	TT_CLOSE_PAREN, 

	TT_SUM, 
	TT_SUBTRACT, 
	TT_MULTIPLY, 
	TT_DIVIDE, 

	TT_EQUALS, 
	TT_NOT_EQUALS, 
	TT_LESS, 
	TT_LESS_EQUALS, 
	TT_GREATER,
	TT_GREATER_EQUALS,

	TT_WHITESPACE,

	TT_FLOAT, 
	TT_INT, 
	TT_STRING, 

	TT_QUOTE, 
	TT_SYMBOL} token_tag;

/*
 * struct for the tokens produced by the lexer
 */
typedef struct Token {
	token_tag type;
	char *data;
} token;

/*
 * Frees a lexer token.
 */
void token_free(token *t);

/*
 * struct for the lexer. Internals are hidden in lex.c
 */
typedef struct Lexer lexer;

/*
 * Allocate memory and init a lexer.
 * Returns a pointer to the new lexer
 */
lexer *lexer_init();

/*
 * Set the data for the lexer to lex.
 * Resets the next pointer to the beginning of the data.
 *
 * Caller is responsible for freeing data after another call to
 * lexer_set_data or lexer_free.
 */
void lexer_set_data(lexer *l, char *data);

/*
 * Lex the next token from the data.
 * Returns the lexed token.
 */
token *lexer_next_token(lexer *l);


/*
 * Frees the memory associated with the lexer struct.
 * Does not free the data assigned with lexer_set_data.
 */
void lexer_free(lexer *l);
#endif
