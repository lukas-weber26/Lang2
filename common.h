#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
INT, FLOAT,STRING, IDENTIFIER, LET,  RETURN, FUNCTION, ADD, SUBTRACT,COMPARE, GT, LT, GTE, LTE, OR, AND, TRUE, FALSE, IF, ELSE, EQUAL,NOT_EQUAL,DIVIDE,MULTIPLY, BANG, ILLIGAL,SEMICOLON,LPAREN, RPAREN, LBRACE,RBRACE,END, COMMA, BRANCH 
} token_type;

typedef struct token {
	char * token_string;
	token_type type;	
	//this is for debug info
	int line; 
	int position;
	//would be cool to add language functionality directly into the token
} token;

typedef struct tokenized_program {
	token ** tokens;
	int token_count;
	int max_size;
	char * input;
	int token_line_number;
	int token_position;
	int read_position ;
} tokenized_program;

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

void parser_free_parser(parser * parser);
parser * parser_init(char * input, int expected_token_count); 
void parser_print_program(parser * parser);
void parser_parse_program(parser * parser);

void parser_test_return();
void parser_test_let();
void parser_test_math();
void parser_test_math_advanced();

tokenized_program * tokenizer_init(char * input_program, int approximate_token_count);
void tokenizer_tokenize_program(tokenized_program * program);
void tokenizer_check_program(tokenized_program * program);
void tokenizer_print_tokens(tokenized_program * program);
void tokenizer_delete_program(tokenized_program * program);
void test_tokenizer();


