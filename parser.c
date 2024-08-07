#include "common.h"
#include <stdio.h>
#include <string.h>


typedef struct ast_node {
	token * lexer_token; //this contains the type info
	struct ast_node * left_node;
	struct ast_node * right_node;
	void * value_pointer;
} ast_node;

typedef struct parser {
	tokenized_program * program;
	int token_read_index;
	ast_node ** expressions;
	int current_expression; 
	int number_of_expression;
} parser;

typedef struct parse_error_info {
	char * error_message;
} parse_error_info;

void parser_parse_tokens(parser * parser);
void parser_free_parser(parser * parser);
ast_node * parser_parse_token(parser * parser);
parser * parser_init(char * input, int expected_token_count); 
void parser_complete_parsing(parser * parser); //delte tokenized program since it is not needed after a tree is made.

parser * parser_init(char * input, int approximate_token_count) {
	tokenized_program * program = tokenizer_init(input, approximate_token_count);
	tokenizer_tokenize_program(program);
	tokenizer_check_program(program);

	//lexer part 
	parser * new_parser = malloc(sizeof(parser));
	new_parser->program = program;
	new_parser->token_read_index = 0;

	//tree part
	new_parser->current_expression = 0;
	new_parser->number_of_expression = program->token_count; //size of the parser
	new_parser->expressions = malloc(sizeof(ast_node *) * program->token_count); //This thing starts as the max size. Resize at the end.
	
	return new_parser;
}

void parser_complete_parsing(parser * parser) {
	//this is going to cause a massive segfault because instead of copying nodes from the lexer, I am copying addresses. 
	//tokenizer_delete_program(parser->program); 
	//parser->program = NULL;
}

void parser_free_parser(parser * parser) {
	//todo
}

//expected functions
token * create_branch_token(); //used parsing if statements 
ast_node * parse_let_token(parser * parser);
ast_node * parse_return_token(parser * parser);
ast_node * parser_parse_infix(parser * parser);  //this thing needs some kind of prescedence? 
ast_node * parser_parse_prefix(parser * parser); //this thing needs some kind of prescedence?
ast_node * parser_parse_if(parser * parser); //this thing needs some kind of prescedence?
void parser_print_node_and_children(ast_node * ast_node, int depth);
void print_depth(int depth);
//how to handle if? If node has 2 child expressions. left is the condition. Right is a BRANCH node (this is basically a dummy node which has a left and right expression. During eval, the if is replaced by either the left or right child of the branch node.

token * create_branch_token() {
	token * new_token = malloc(sizeof(token));
	new_token->type = BRANCH;
	new_token->token_string = strdup("branch");
	//these are a little unfortunate
	new_token->line = -1;
	new_token->position = -1;
	return new_token;
}

ast_node * parser_parse_let(parser * parser);
ast_node * parser_parse_return(parser * parser);
ast_node * new_generic_node();
ast_node * parser_parse_return_(parser * parser);
int token_token_is_gramar(token * token);

//could have more functions like this. Think "is valid infix, is valid postfix, etc."
int token_token_is_gramar(token * token) {
	token_type type = token->type;
	if ((type == COMMA) ||(type == END) ||(type == LBRACE) ||(type == RBRACE) ||(type == LPAREN) ||(type == RPAREN) ||(type == EQUAL) ||(type == NOT_EQUAL) || (type == SEMICOLON)) {
		return 1;
	} else {
		return 0;
	}
}

ast_node * new_generic_node() {
	ast_node * new_node = malloc(sizeof(ast_node));
	new_node -> left_node = NULL;
	new_node -> right_node = NULL;
	new_node -> value_pointer = NULL;
	new_node -> lexer_token = NULL;
	return new_node;
}

ast_node * parser_parse_let(parser * parser) {
	//get the next couple of tokens to make sure they fit into a let statement
	int parser_read_index = parser->token_read_index;
	token ** token_array_pointer = parser->program->tokens;	

	//check the requirements for the let expression
	token * let = token_array_pointer[parser_read_index];
	token * symbol = token_array_pointer[parser_read_index + 1];
	token * equals = token_array_pointer[parser_read_index + 2];
	token * expression = token_array_pointer[parser_read_index + 3];

	//The whole != EOF thing should probably be a isValidExpression function....
	if (symbol->type == IDENTIFIER && equals->type == EQUAL && !token_token_is_gramar(expression)) {
		ast_node * new_let_node = new_generic_node();
		new_let_node->lexer_token = let;
		new_let_node->left_node = new_generic_node();
		new_let_node->left_node->lexer_token = symbol;
		parser->token_read_index += 3; //next tokenizer pass should be starting at the expression
		new_let_node->right_node = parser_parse_token(parser); //for now I will not attach values to the nodes.
		return new_let_node;

	} else {
		printf("Invalid let statement detected at line %d, position %d.\n", let->line, let->position);
		printf("Expected Identifier = Symbol, Received %s %s %s...\n", symbol->token_string, equals->token_string, expression->token_string);
		exit(0);
	}
}

ast_node * parser_parse_return_(parser * parser) {
	//get the next couple of tokens to make sure they fit into a let statement
	int parser_read_index = parser->token_read_index;
	token ** token_array_pointer = parser->program->tokens;	

	//check the requirements for the let expression
	token * return_symbol = token_array_pointer[parser_read_index];
	token * return_value = token_array_pointer[parser_read_index + 1];

	//The whole != EOF thing should probably be a isValidExpression function....
	if (!token_token_is_gramar(return_value)) {
		ast_node * new_return_node = new_generic_node();
		new_return_node ->lexer_token = return_symbol;
		parser->token_read_index += 1; 
		new_return_node -> left_node = parser_parse_token(parser); 
		return new_return_node;

	} else {
		printf("Invalid return statement detected at line %d, position %d.\n", return_symbol->line, return_symbol->position);
		printf("Expected return expression, Received %s %s...\n", return_symbol->token_string, return_value->token_string);
		exit(0);
	}
}

ast_node * parser_parse_return(parser * parser) {
	return NULL;
}

ast_node * parser_parse_token(parser * parser) {
	//typedef enum {
//IDENTIFIER, FUNCTION, ADD, SUBTRACT,COMPARE, GT, LT, GTE, LTE, OR, AND, TRUE, FALSE, IF, ELSE, EQUAL,NOT_EQUAL,DIVIDE,MULTIPLY, BANG, ILLIGAL,SEMICOLON,LPAREN, RPAREN, LBRACE,RBRACE,END, COMMA, BRANCH 
//} token_type;

	token * current_token = parser->program->tokens[parser->token_read_index]; //make sure to update parser->token_read_index
	switch (current_token->type) {
		case LET:
			return parser_parse_let(parser);
		case RETURN:
			return parser_parse_return(parser);
		case SEMICOLON: 
			//somehow semicolon needs to just let you know that everything is done.
			parser->token_read_index++;
			return NULL;
		default:
			parser->token_read_index++; //these ++ expressions are citical, without them the tokenizer can get stuck 
			return NULL;

		//some more things could go here. Ie an algebraic case, a semicolon case? Idk.
	}
}

void print_depth(int depth) {
	while (depth > 0) {
		printf("\t");
		depth--;
	}
}

void parser_print_node_and_children(ast_node * ast_node, int depth) {
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
		default:
			return;	
	}
}


void parser_parse_program(parser * parser) {

	while (parser->program->tokens[parser->token_read_index]->type != END) { 
		parser->expressions[parser->current_expression] = parser_parse_token(parser);
		parser->current_expression++;
	}

	parser->expressions= realloc(parser->expressions, sizeof(ast_node*) * parser->current_expression);
}

//a couple of tests for parsing let and return would be good.
//figuring out what the seocond token in the let x = 10; statement is would also be good.
//I guess its probably just a stray null value...
//try your print function out 

int main() {
	char * input = "let x = 10;";
	parser * parser = parser_init(input, 10);
	parser_parse_program(parser);	
	//test_tokenizer();
}

