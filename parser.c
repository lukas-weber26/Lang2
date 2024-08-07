#include "common.h"
#include <cstdio>
#include <string.h>

typedef struct ast_node {
	token lexer_token; //this contains the type info
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
	tokenizer_delete_program(parser->program);
	parser->program = NULL;
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
//how to handle if? If node has 2 child expressions. left is the condition. Right is a BRANCH node (this is basically a dummy node which has a left and right expression. During eval, the if is replaced by either the left or right child of the branch node.

token * create_branch_token() {
	token * new_token = malloc(sizeof(token));
	new_token->type = BRANCH;
	new_token->token_string = strdup("branch");
	//these are a little unfortunate
	new_token->line = -1;
	new_token->position = -1;
}

ast_node * parser_parse_token(parser * parser) {
	//something like read the first token. Switch based on its type.
	//depending on the type, send that shit to a handler function. see above 
	//the handler function keeps reading tokens until the expression is done. (ie ; or eof is detected)
	//note, the handler function can also call parse token to figure out how to handle its chilren
	//note that this thing has the advantage of being recursive.... hence the whole recursive descent parser.

	token * current_token = parser->program->tokens[parser->token_read_index]; //make sure to update parser->token_read_index
	switch (current_token->type) {
		//this is too much for this second! 

	}
	return NULL;
}

void parser_parse_program(parser * parser) {
	//This thing calls parser_parse_token and sticks the result into an array.
	//It then does it again until we are out of tokens.
	//Hence, this thing is based.

	while (parser->token_read_index < parser->program->token_count) { 
		parser->expressions[parser->current_expression] = parser_parse_token(parser);
		parser->current_expression++;
	}

	//no resizing is required during the while loop because the maximum number of items is allocated at the start
	//realloc at the end to avoid memory mess
	parser->expressions= realloc(parser->expressions, sizeof(ast_node*) * parser->current_expression);
		
}

int main() {
	test_tokenizer();
}
