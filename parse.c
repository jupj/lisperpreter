#include <stdlib.h>
#include <stdio.h>

#include "parse.h"

struct Parser {
	struct ast_node *root;
	lexer *l;
	token *next;
	int errors;
};

struct ast_node *parse_next(parser *p);

token *get_next(parser *p) {
	token *n = p->next;
	p->next = lexer_next_token(p->l);
	return n;
}

int parser_is_next(parser *p, token_tag tag) {
	return (tag == p->next->type);
}

token *match(parser *p, token_tag tag) {
	if (parser_is_next(p, tag)) {
		return get_next(p);
	} else {
		return NULL;
	}
}

int match_and_free(parser *p, token_tag tag) {
	token *t = match(p, tag);
	if (t) {
		token_free(t);
		return 1;
	} else {
		return 0;
	}
}

void skip_all(parser *p, token_tag tag) {
	while (parser_is_next(p, tag)) {
		match_and_free(p, tag);
	}
}

/*
 * Helper function - creates an error node.
 */
struct ast_node *parser_error(parser *p, char *msg) {
	p->errors = 1;
	fprintf(stderr, "ERROR: %s\n", msg);

	struct ast_node *err = malloc(sizeof(struct ast_node));
	err->tag = AT_ERROR;
	err->t = get_next(p);
	err->next = NULL;
	err->child = NULL;
	return err;
}


/*
 * Parse a symbol
 */
struct ast_node *symbol(parser* p) {
	token *t = match(p, TT_SYMBOL);
	if (t) {
		struct ast_node *s = malloc(sizeof(struct ast_node));
		s->tag = AT_SYMBOL;
		s->t = t;
		s->next = NULL;
		s->child = NULL;
		return s;
	} else {
		return parser_error(p, "Expected symbol");
	}
}

/*
 * Parse a quote
 */
struct ast_node *quote(parser* p) {
	token *t = match(p, TT_QUOTE);
	if (t) {
		struct ast_node *q = malloc(sizeof(struct ast_node));
		q->tag = AT_QUOTE;
		q->t = t;
		q->next = NULL;
		q->child = parse_next(p);
		return q;
	} else {
		return parser_error(p, "Expected quote");
	}
}

/*
 * Parse an S-expression
 */
struct ast_node *s_expr(parser* p) {
	token *t = match(p, TT_OPEN_PAREN);
	if (t) {
		struct ast_node *list = malloc(sizeof(struct ast_node));
		list->tag = AT_LIST;
		list->t = t;
		list->next = NULL;
		list->child = NULL;

		// Check empty list:
		if (match_and_free(p, TT_CLOSE_PAREN))
			return list;

		// Not an empty list, i.e. next token is the first item in list:
		list->child = parse_next(p);
		struct ast_node *current = list->child;

		while (!(parser_is_next(p, TT_CLOSE_PAREN) ||
		         parser_is_next(p, TT_EOF) ||
		         parser_is_next(p, TT_ERROR))) {
			current->next = parse_next(p);
			current = current->next;
		}

		if (!match_and_free(p, TT_CLOSE_PAREN))
			return parser_error(p, "Expected end of s-expression");

		return list;
	} else {
		return parser_error(p, "Expected s-expression");
	}
}

/*
 * Parse next token.
 */
struct ast_node *parse_next(parser *p) {
	if (parser_is_next(p, TT_OPEN_PAREN))
		return s_expr(p);
	if (parser_is_next(p, TT_QUOTE))
		return quote(p);
	if (parser_is_next(p, TT_SYMBOL))
		return symbol(p);

	return parser_error(p, "Expected list, quote or symbol");
}

/*
 * Allocate memory and init a parser.
 * Returns a pointer to the new parser.
 */
parser *parser_init() {
	parser *p = (parser *) malloc(sizeof(parser));
	p->l = lexer_init();
	p->root = NULL;
	p->errors = 0;
	return p;
}

/*
 * Parse the given data. All completely parsed code will be added to the AST.
 *
 * Returns PR_OK if the data was parsed completely.
 * Returns PR_ERROR if there were errors in the code.
 */
enum parse_result parser_parse(parser* p, char *data) {
	lexer_set_data(p->l, data);
	p->next = lexer_next_token(p->l);
	p->root = s_expr(p);
	if (p->errors)
		return PR_ERROR;
	else
		return PR_OK;
}

/*
 * Get the root node of the AST in the parser.
 */
struct ast_node *parser_get_ast(parser *p) {
	return p->root;
}

/*
 * Frees the memory associated with the AST node and its subtree.
 * Also frees the tokens.
 */
void ast_node_free(struct ast_node *n) {
	// Free sibling nodes
	if (n->next != NULL)
		ast_node_free(n->next);

	// Free child nodes
	if (n->child != NULL)
		ast_node_free(n->child);

	// Free the node itself
	token_free(n->t);
	free(n);
}

/*
 * Frees the memory associated with the parser struct.
 * Does not free the data assigned with parse.
 */
void parser_free(parser *p) {
	// free the AST:
	ast_node_free(p->root);
	lexer_free(p->l);
	free(p);
}
