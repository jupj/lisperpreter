#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "eval.h"
#include "parse.h"

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

void eval(struct ast_node *node) {
	if ((node->tag == AT_LIST) && node->child && (node->child->tag == AT_SYMBOL) && (strcmp(node->child->t->data, "+") == 0)) {
		// Add:
		struct ast_node *term = node->child->next;
		int val = 0;
		while (term) {
			val += atoi(term->t->data);
			term = term->next;
		}
		printf("%d\n", val);
	} else {
		print_ast(node);
	}
}

