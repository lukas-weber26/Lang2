#include "common.h"

//a couple of tests for parsing let and return would be good.
void parser_test_return() {
	printf("Starting return test.\n");

	char * input = "return 10;\n"
	"return x;\n"
	"return y;\n";

	parser * parser = parser_init(input, 10);
	parser_parse_program(parser);	

	token_type types [] = {INT, IDENTIFIER, IDENTIFIER};

	//run through everything, assert things
	assert(parser->number_of_expression == 3);
	int count = 0;

	while(parser->number_of_expression > count) {
		ast_node * current = parser->expressions[count];
		assert(current->lexer_token->type == RETURN);
		assert(current->right_node == NULL);	
		assert(current->left_node->lexer_token->type == types[count]);
		assert(current->left_node->left_node == NULL);
		assert(current->left_node->right_node == NULL);
		count ++;
	}

	parser_print_program(parser);
	parser_free_parser(parser);

	printf("Return test passed.\n");
}

void parser_test_let() {
	printf("Starting let test.\n");

	char * input = "let x = 10;\n"
	"let boing = doing;\n"
	"let bingus = 4;\n";

	parser * parser = parser_init(input, 10);
	parser_parse_program(parser);	

	token_type types[] = {INT, IDENTIFIER, INT};

	//run through everything, assert things
	assert(parser->number_of_expression == 3);
	int count = 0;

	while(parser->number_of_expression > count) {
		ast_node * current = parser->expressions[count];
		assert(current->lexer_token->type == LET);
		
		assert(current->left_node->lexer_token->type == IDENTIFIER);
		assert(current->left_node->left_node == NULL);
		assert(current->left_node->right_node == NULL);

		assert(current->right_node->lexer_token->type == types[count]);
		assert(current->right_node->left_node == NULL);
		assert(current->right_node->right_node == NULL);
		count ++;
	}

	parser_print_program(parser);
	parser_free_parser(parser);

	printf("Let test passed.\n");
}
