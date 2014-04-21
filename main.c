#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "lex.h"
#include "parse.h"

void print_tokens(lexer *l, char *data) {
	lexer_set_data(l, data);
	token *t;
	while ((t = lexer_next_token(l))) {
		if (t->type == TT_ERROR) {
			printf("ERROR: %s\n", t->data);
			break;
		}

		if (t->type == TT_EOF) {
			puts("End of data");
			break;
		}
		printf("Token: '%s' (type %d)\n", t->data, t->type);
		token_free(t);
	}
}

void test_lexer() {
	lexer *l = lexer_init();

	while (1) {
		// Output our prompt and get input
		char *input = readline("> ");
		add_history(input);
		print_tokens(l, input);
		free(input);
	}
	lexer_free(l);
}

void print_ast(struct ast_node *node) {
	if (node == NULL) {
		puts("NULL");
		return;
	} else if (node->tag == AT_LIST) {
		puts("LIST:");
		print_ast(node->child);
	} else if (node->tag == AT_QUOTE) {
		puts("QUOTE:");
		print_ast(node->child);
	} else if (node->tag == AT_SYMBOL) {
		printf("SYMBOL: '%s'\n", node->t->data);
	}

	if (node->next != NULL)
		print_ast(node->next);
}

void test_parser() {
	parser *p = parser_init();

	while (1) {
		// Output our prompt and get input
		char *input = readline("> ");
		add_history(input);
		parser_parse(p, input);
		print_ast(parser_get_ast(p));
		free(input);
	}

	parser_free(p);
}

int main(int argc, char **argv) {
	puts("Lisperpreter - a simple Lisp interpreter");
	puts("Press Ctrl+c to exit\n");

	test_parser();
	return 0;
}
