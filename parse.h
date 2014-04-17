#ifndef parse_h
#define parse_h

typedef enum Ast_tag {
	AT_SUM, 
	AT_SUBTRACT, 
	AT_MULTIPLY, 
	AT_DIVIDE,

	AT_FLOAT, 
	AT_INT, 
	AT_STRING
} ast_tag;

typedef struct Ast_node ast_node;

struct Ast_node {
	ast_tag tag;
	token *t;
	ast_node *next;
	ast_node *child;
};

typedef struct Parser parser;

typedef enum Parse_result {PR_READY, PR_INCOMPLETE, PR_ERROR} parse_result;

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
parser_result parser_parse(parser* p, char *data);

/*
 * Get the root node of the AST in the parser.
 */
ast_node *parser_get_ast(parser *p);

/*
 * Frees the memory associated with the parser struct.
 * Does not free the data assigned with parse.
 */
void parser_free(parser *p);
#endif
