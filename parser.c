#include "common.h"

ast_node * parser_parse_token(parser * parser);
void parser_free_ast(ast_node * ast_node);
ast_node * parser_parse_let(parser * parser);
ast_node * parser_parse_return(parser * parser);
ast_node * new_generic_node();
ast_node * parser_parse_infix_expression(parser * parser, ast_node * left_node);

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

//expected functions
token * create_branch_token() {
	token * new_token = malloc(sizeof(token));
	new_token->type = BRANCH;
	new_token->token_string = strdup("branch");
	//these are a little unfortunate
	new_token->line = -1;
	new_token->position = -1;
	return new_token;
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

		parser->token_read_index += 3;
		new_let_node->right_node = parser_parse_token(parser); //for now I will not attach values to the nodes.

		return new_let_node;

	} else {
		printf("Invalid let statement detected at line %d, position %d.\n", let->line, let->position);
		printf("Expected Identifier = Symbol, Received %s %s %s...\n", symbol->token_string, equals->token_string, expression->token_string);
		exit(0);
	}
}

ast_node * parser_parse_return(parser * parser) {
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

		parser->token_read_index += 1; //decrease for acutal use (used to be 2)
		new_return_node->left_node= parser_parse_token(parser); //for now I will not attach values to the nodes.
		//
		//new_return_node->left_node= new_generic_node();
		//new_return_node->left_node->lexer_token = return_value;
		//new_return_node -> left_node = parser_parse_token(parser); 
		return new_return_node;

	} else {
		printf("Invalid return statement detected at line %d, position %d.\n", return_symbol->line, return_symbol->position);
		printf("Expected return expression, Received %s %s...\n", return_symbol->token_string, return_value->token_string);
		exit(0);
	}
}

ast_node * parser_parse_prefix_expression(parser * parser) {
	ast_node * new_node = new_generic_node();
	new_node ->lexer_token = parser->program->tokens[parser->token_read_index];
	parser->token_read_index += 1;
	
	token * infix_data = parser->program->tokens[parser->token_read_index];

	if (infix_data->type == INT || infix_data->type == IDENTIFIER) {
		ast_node * infix_data_node = new_generic_node();
		infix_data_node ->lexer_token = parser->program->tokens[parser->token_read_index];
		new_node->left_node = infix_data_node;
		parser->token_read_index ++;
	
		if (parser_get_operator_presedence(parser->program->tokens[parser->token_read_index]) != -1) {
			return parser_parse_infix_expression(parser, new_node);
		} else {
			return new_node;
		}

	} else if (infix_data->type == LPAREN) {
		parser->token_read_index ++;
		return parser_parse_token(parser); //this seems suss!

	} else {printf("Invalid prefix expression detected at line %d, position %d.\n", infix_data->line, infix_data->position);
		printf("Expected type INT or IDENTIFIER, received %s...\n", infix_data->token_string);
		exit(0);
	}

	new_node -> left_node = parser_parse_token(parser); 
	return new_node;
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
			parser->token_read_index++;
			return NULL;
		case LPAREN:
			parser->token_read_index++;
			return NULL;
		default:
			if (is_valid_infix_operator(current_token)) {
				return parser_parse_prefix_expression(parser);
			} else if (current_token->type == IDENTIFIER || current_token->type == INT) {
				if (parser_get_operator_presedence(parser->program->tokens[parser->token_read_index+1]) != -1) {
					ast_node * new_left_node = new_generic_node();
					new_left_node->lexer_token = current_token;
					parser->token_read_index ++;
					return parser_parse_infix_expression(parser,new_left_node); 
				} else { 
					ast_node * new_node = new_generic_node();
					new_node ->lexer_token = current_token;
					parser->token_read_index ++;
					return new_node;
				}
			} else {
				parser->token_read_index++; //these ++ expressions are citical, without them the tokenizer can get stuck 
				return NULL;
			}

		//some more things could go here. Ie an algebraic case, a semicolon case? Idk.
	}
}

void parser_parse_program(parser * parser) {

	while (parser->program->tokens[parser->token_read_index]->type != END) { 
		ast_node * new_node = parser_parse_token(parser);
		if (new_node != NULL) {
			parser->expressions[parser->current_expression] = new_node;
			parser->current_expression++;
		}
	}

	parser->expressions= realloc(parser->expressions, sizeof(ast_node*) * parser->current_expression);
	parser->number_of_expression = parser->current_expression;
}

void parser_free_ast(ast_node * ast_node) {
	if (ast_node->value_pointer != NULL) {
		free(ast_node->value_pointer); //danger danger... this may become insufficient
	}
	if (ast_node->left_node != NULL) {
		parser_free_ast(ast_node->left_node);
	}
	if (ast_node->right_node != NULL) {
		parser_free_ast(ast_node->right_node);
	}
	free(ast_node);
}

void parser_free_parser(parser * parser) {
	tokenizer_delete_program(parser->program);
	int count = 0;
	while(parser->number_of_expression > count) {
		parser_free_ast(parser->expressions[count]);
		count ++;
	}
	free(parser->expressions);
	free(parser);
}

//start off expecting only infix
//for now, this thing will call itself and nothing else 
ast_node * parser_parse_infix_expression(parser * parser, ast_node * left_node) {
	int parser_read_index = parser->token_read_index;
	token ** token_array_pointer = parser->program->tokens;	

	//check the requirements for the let expression
	token * operator_token = token_array_pointer[parser_read_index];
	token * expression_token = token_array_pointer[parser_read_index + 1];

	//check if the expression is valid?.

	//no matter what, you will need your value attached to your operator
	ast_node * operator_node = new_generic_node();
	
	operator_node->lexer_token = operator_token;
	operator_node->left_node = left_node;

	//find the next operator
	if (is_valid_infix_operator(expression_token)) {
		parser->token_read_index += 1;
		operator_node -> right_node = parser_parse_prefix_expression(parser);
		return operator_node;
	} else {
		int current_op_presedence= parser_get_operator_presedence(operator_token);
		int next_op_prescedence = parser_get_next_operator_prescedence(parser);

		if (next_op_prescedence == -1) { //expression done
			ast_node * expression_node = new_generic_node();
			expression_node->lexer_token = expression_token;
			operator_node->right_node = expression_node;
			parser->token_read_index += 2;
			return operator_node;
		} else if (next_op_prescedence < current_op_presedence) {//lower priority
			ast_node * expression_node = new_generic_node();
			expression_node->lexer_token = expression_token;
			operator_node->right_node = expression_node;
			parser->token_read_index += 2;
			return parser_parse_infix_expression(parser, operator_node);
		} else {//next is higher priority {
			ast_node * new_left_node = new_generic_node();
			new_left_node->lexer_token = token_array_pointer[parser_read_index + 1];
			parser->token_read_index += 2;
			operator_node->right_node = parser_parse_infix_expression(parser, new_left_node);
			return operator_node;
		}
	}

}

int main() {
	//goal: switch return and let to use real switch statement
	//parser_test_return();
	//parser_test_let();
	//parser_test_math();
	//parser_test_math_advanced();
	//parser_test_prefix();
	parser_test_paren();
}

