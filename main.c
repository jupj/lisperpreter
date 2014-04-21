#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "lex.h"

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

int main(int argc, char **argv) {
	puts("Lisperpreter - a simple Lisp interpreter");
	puts("Press Ctrl+c to exit\n");

	lexer *l = lexer_init();

	while (1) {
		// Output our prompt and get input
		char *input = readline("> ");
		add_history(input);
		print_tokens(l, input);
		free(input);
	}
	lexer_free(l);
	return 0;
}
