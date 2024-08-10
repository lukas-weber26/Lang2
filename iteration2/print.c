#include "common.h"

void print_depth(int depth) {
	while (depth > 0) {
		printf("\t");
		depth--;
	}
}

void print_operator(ast_node * ast_node, int depth, char * operator_name) {
		token * current = ast_node->lexer_token;
		printf("%s: %s (%d,%d)\n", operator_name,current->token_string, current->line, current->position);
		depth ++;
		parser_print_node_and_children(ast_node->left_node, depth);
		parser_print_node_and_children(ast_node->right_node, depth);
}

void parser_print_node_and_children(ast_node * ast_node, int depth) {
	
	if (!ast_node) {
		return;
	}

	print_depth(depth);
	token * current = ast_node->lexer_token;

	switch (current->type) {
		case LET:
			printf("LET: %s (%d,%d)\n", current->token_string, current->line, current->position);
			depth ++;
			parser_print_node_and_children(ast_node->left_node, depth);
			parser_print_node_and_children(ast_node->right_node, depth);
			break;
		case RETURN:
			printf("RETURN: %s (%d,%d)\n", current->token_string, current->line, current->position);
			depth ++;
			parser_print_node_and_children(ast_node->left_node, depth);
			break;
		case IDENTIFIER:
			printf("IDENTIFIER: %s (%d,%d)\n", current->token_string, current->line, current->position);
			break;
		case INT:
			printf("INTEGER: %s (%d,%d)\n", current->token_string, current->line, current->position);
			break;
		case FALSE:
			printf("BOOL: %s (%d,%d)\n", current->token_string, current->line, current->position);
			break;
		case TRUE:
			printf("BOOL: %s (%d,%d)\n", current->token_string, current->line, current->position);
			break;
		case ADD:
			print_operator(ast_node, depth, "ADD");
			break;
		case SUBTRACT:
			print_operator(ast_node, depth, "SUBTRACT");
			break;
		case COMPARE:
			print_operator(ast_node, depth, "COMPARE");
			break;
		case GT:
			print_operator(ast_node, depth, "GT");
			break;
		case LT:
			print_operator(ast_node, depth, "LT");
			break;
		case GTE:
			print_operator(ast_node, depth, "GTE");
			break;
		case LTE:
			print_operator(ast_node, depth, "LTE");
			break;
		case OR:
			print_operator(ast_node, depth, "OR");
			break;
		case AND:
			print_operator(ast_node, depth, "AND");
			break;
		case EQUAL:
			print_operator(ast_node, depth, "EQUAL");
			break;
		case NOT_EQUAL:
			print_operator(ast_node, depth, "NOT EQUAL");
			break;
		case DIVIDE:
			print_operator(ast_node, depth, "DIVIDE");
			break;
		case MULTIPLY:
			print_operator(ast_node, depth, "MULTIPLY");
			break;
		default:
			return;	
	}
}

void parser_print_program(parser * parser) {
	printf("Printing AST. Expecting %d expressions.\n", parser->number_of_expression);
	int count = 0;
	while(parser->number_of_expression > count) {
		parser_print_node_and_children(parser->expressions[count], 0);
		count ++;
	}
}
