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

void parser_test_math() {
	printf("Starting math test.\n");

	char * input = "x * 10 + 5;\n"
	"x + 10 * 5;\n"
	"a + b + c + d;\n"
	"a + b * c + d * e;\n";

	parser * parser = parser_init(input, 10);
	parser_parse_program(parser);	

	assert(parser->number_of_expression == 4);

	//unfortunately, this mess is best tested manually
	ast_node * first = parser->expressions[0];
	assert(first->lexer_token->type == ADD);
	assert(first->left_node->lexer_token->type == MULTIPLY);
	assert(first->left_node->left_node->lexer_token->type == IDENTIFIER);
	assert(strcmp(first->left_node->left_node->lexer_token->token_string, "x") == 0);
	assert(first->left_node->right_node->lexer_token->type == INT);
	assert(strcmp(first->left_node->right_node->lexer_token->token_string, "10") == 0);
	assert(first->right_node->lexer_token->type == INT);
	assert(strcmp(first->right_node->lexer_token->token_string, "5") ==0);
	
	ast_node * second = parser->expressions[1];
	assert(second->lexer_token->type == ADD);
	assert(second->left_node->lexer_token->type == IDENTIFIER);
	assert(second->right_node->lexer_token->type == MULTIPLY);
	assert(second->right_node->left_node->lexer_token->type == INT);
	assert(strcmp(second->right_node->left_node->lexer_token->token_string, "10") == 0);
	assert(second->right_node->right_node->lexer_token->type == INT);
	assert(strcmp(second->right_node->right_node->lexer_token->token_string, "5") == 0);
	
	ast_node * third = parser->expressions[2];
	assert(third->lexer_token->type == ADD);
	assert(third->left_node->lexer_token->type == IDENTIFIER);
	assert(third->right_node->lexer_token->type == ADD);
	assert(third->right_node->left_node->lexer_token->type == IDENTIFIER);
	assert(third->right_node->right_node->lexer_token->type == ADD);
	assert(third->right_node->right_node->left_node->lexer_token->type == IDENTIFIER);
	assert(third->right_node->right_node->right_node->lexer_token->type == IDENTIFIER);

	ast_node * fourth = parser->expressions[3];
	assert(fourth->lexer_token->type == ADD);
	assert(fourth->left_node->lexer_token->type == IDENTIFIER);
	assert(fourth->right_node->lexer_token->type == ADD);
	assert(fourth->right_node->left_node->lexer_token->type == MULTIPLY);
	assert(fourth->right_node->left_node->left_node->lexer_token->type == IDENTIFIER);
	assert(fourth->right_node->left_node->right_node->lexer_token->type == IDENTIFIER);
	assert(fourth->right_node->right_node->lexer_token->type == MULTIPLY);
	assert(fourth->right_node->right_node->left_node->lexer_token->type == IDENTIFIER);
	assert(fourth->right_node->right_node->right_node->lexer_token->type == IDENTIFIER);

	parser_print_program(parser);
	parser_free_parser(parser);

	printf("Math test passed.\n");
}
