#ifndef parse_h
#define parse_h

enum ast_tag {
	AT_ERROR,
	AT_LIST,
	AT_QUOTE,
	AT_SYMBOL,
};

struct ast_node {
	enum ast_tag tag;
	token *t;
	struct ast_node *next;
	struct ast_node *child;
};

typedef struct Parser parser;

enum parse_result {PR_OK, PR_ERROR};

/*
 * Allocate memory and init a parser.
 * Returns a pointer to the new parser.
 */
parser *parser_init();

/*
 * Parse the given data. All completely parsed code will be added to the AST.
 *
 * Returns PR_READY if the data was parsed completely.
 * Returns PR_INCOMPLETE if the code wasn't complete. Parsing can be continued
 *   with another call to parse. The parts of the code that were parsed were
 *   added to the AST.
 * Returns PR_ERROR if there were errors in the code.
 */
enum parse_result parser_parse(parser* p, char *data);

/*
 * Get the root node of the AST in the parser.
 */
struct ast_node *parser_get_ast(parser *p);

/*
 * Frees the memory associated with the parser struct.
 * Does not free the data assigned with parse.
 */
void parser_free(parser *p);
#endif
