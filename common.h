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

tokenized_program * tokenizer_init(char * input_program, int approximate_token_count);
void tokenizer_tokenize_program(tokenized_program * program);
void tokenizer_check_program(tokenized_program * program);
void tokenizer_print_tokens(tokenized_program * program);
void tokenizer_delete_program(tokenized_program * program);
void test_tokenizer();


