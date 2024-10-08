#include "common.h"

int is_valid_infix_op(token * token);
int is_valid_val(token * token);
int is_valid_prefix_op(token * token);
int is_valid_paren(token * token);
int is_valid_val(token * token);
int parser_next_op_is_higher_order(parser * parser, int current_operator_index);
int parser_get_op_order(token * token);

void parser_free_ast(ast_node * ast_node);
ast_node * new_generic_node();
ast_node * parser_parse_token(parser * parser, ast_node * left_node);

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

void parser_parse_program(parser * parser) {

	while (parser->program->tokens[parser->token_read_index]->type != END) { 
		//step 1: check if the node should be a return or something like that 

		ast_node * new_node = parser_parse_token(parser,NULL);

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

int is_valid_infix_op(token * token) {
	token_type type = token->type;
	if (type== ADD || type == MULTIPLY || type == SUBTRACT || type == DIVIDE || type == COMPARE || type == NOT_EQUAL|| type == GT || type == LT || type == LTE || type == GTE || type == AND || type == OR || type == EQUAL || type == COMMA) {
		return 1;
	} return 0;
}

int is_valid_val(token * token) {
	token_type type = token->type;
	if (type == INT || type == IDENTIFIER || type == TRUE || type == FALSE) {
		return 1;
	}
	return 0;
}

//wish this thing had a next
int is_valid_prefix_op(token * token) {
	token_type type = token->type;
	if (type == SUBTRACT || type == RETURN || type == LET || type == IF || type == FUNCTION) { 
		return 1;
	} 
	return 0;
}

int is_valid_paren(token * token) {
	token_type type = token->type;
	if (type == LPAREN) {
		return 1;
	}
	return 0;
}

int parser_get_op_order(token * token) {
	switch (token->type) {
		case LPAREN:
			return 10000;
		case RPAREN:
			return -10000;
		case DIVIDE:
			return 7;
		case MULTIPLY:
			return 7;
		case ADD: 
			return 5;
		case SUBTRACT:
			return 5;
		case COMPARE:	//low prescedence
			return 4;
		case NOT_EQUAL:
			return 4;
		case GT:
			return 4;
		case LT:
			return 4;
		case GTE:
			return 4;
		case LTE:	//super low presedence
			return 4; 
		case AND:
			return 3;
		case OR:
			return 3;
		return COMMA;
			return 2;
		case EQUAL: //assignment is the lowest prescedence 
			return 1;
		case RETURN: 
			return 0;
		case IF: //Im not sure that these really meaningfull because they are always infix followed by brackets
		case FUNCTION:
			return 0;
		default: 
			return -1;
	}
}

int parser_next_op_is_higher_order(parser * parser, int current_operator_index) {
	token ** tokens = parser->program->tokens;
	if (is_valid_infix_op(tokens[current_operator_index + 1]) || is_valid_paren(tokens[current_operator_index + 1])) {
		return 1;
	}

	if (parser_get_op_order(tokens[current_operator_index]) < parser_get_op_order(tokens[current_operator_index + 2])) {
		return 1;
	}

	return 0;
}

int parser_get_order_after_paren(parser * parser);

int parser_get_order_after_paren(parser * parser) {
	int read_index = parser->token_read_index + 1;
	token ** tokens = parser->program->tokens;
	int depth = 1;	
	while (depth > 0) {
		if (tokens[read_index]->type == LPAREN) {
			depth ++;
		}
		if (tokens[read_index]->type == RPAREN) {
			depth --; 
		}
		read_index ++;
	}	
	return parser_get_op_order(tokens[read_index]);
}

ast_node * parser_parse_token(parser * parser, ast_node * left_node) {
	token ** tokens = parser->program->tokens;
	int read_index = parser->token_read_index;

	if (left_node == NULL)	{
		//four possible cases: //val,op (infix) //op (prefix) //( paren //val (standalone value)
		if (is_valid_val(tokens[read_index]) && is_valid_infix_op(tokens[read_index+1])) {
			ast_node * new_left_node = new_generic_node();
			new_left_node ->lexer_token = tokens[read_index+1];
			new_left_node -> left_node = new_generic_node();
			new_left_node -> left_node->lexer_token = tokens[read_index];
			parser->token_read_index += 2;			

			if (!parser_next_op_is_higher_order(parser, read_index + 1)) {
				new_left_node -> right_node = new_generic_node();
				new_left_node -> right_node -> lexer_token = tokens[read_index + 2];
				parser->token_read_index += 1;	
			} else {
				if (tokens[parser->token_read_index]->type == LPAREN) {
					if (parser_get_order_after_paren(parser) > parser_get_op_order(tokens[read_index+1])) {
						//post parentheses is higher order than current?
						//then treat this as a starting parentheses expression?
						//ie no increment
					} else {
						parser->token_read_index ++;
					}
				}	
					
				new_left_node -> right_node = parser_parse_token(parser, NULL);	
			}
			
			return parser_parse_token(parser, new_left_node);

		} else if (is_valid_prefix_op(tokens[read_index]) || (is_valid_val(tokens[read_index]) && (tokens[read_index+1]->type == LPAREN))) {
			//this thing is suspiciously simple 
			ast_node * new_left_node = new_generic_node();
			new_left_node ->lexer_token = tokens[read_index];
			parser->token_read_index += 1;			

			if (!parser_next_op_is_higher_order(parser, read_index)) {
				new_left_node -> right_node = new_generic_node();
				new_left_node -> right_node -> lexer_token = tokens[read_index + 1];
				parser->token_read_index += 1;	
			} else {
				new_left_node -> right_node = parser_parse_token(parser, NULL);	
			}
			
			return parser_parse_token(parser, new_left_node);

		} else if (is_valid_paren(tokens[read_index])) {

			//this case is only relevant if the first item in the expression is a (
			//tee hee this thing was in fact too simple. I need to do the same thing as for infix.
			//Check if the next token is a higher index
			parser->token_read_index += 1;	
			ast_node * parentheses_expression_node = parser_parse_token(parser, NULL);		
			return parser_parse_token(parser, parentheses_expression_node);
		
		} else if (is_valid_val(tokens[read_index])) {
			ast_node * new_left_node = new_generic_node();
			new_left_node ->lexer_token = tokens[read_index];
			parser->token_read_index += 1;
			return new_left_node;
		} else if (tokens[read_index]->type == SEMICOLON) {
			parser->token_read_index += 1;
			return NULL;	
		} else {
			token * problem_token = tokens[read_index];
			printf("PARSER ERROR (type 1): received invalid token order at line %d, position %d.\n", problem_token->line, problem_token->position);
			printf("Received token: %s\n", problem_token->token_string);
			exit(0);
		}
	
	} else {
		if (is_valid_infix_op(tokens[read_index])) { //this is the infix case
			ast_node * new_operator_node = new_generic_node();
			new_operator_node -> lexer_token = tokens[read_index];
			new_operator_node -> left_node = left_node;
			parser->token_read_index +=1;
				
			if (!parser_next_op_is_higher_order(parser, read_index)) {
				new_operator_node -> right_node = new_generic_node();
				new_operator_node -> right_node -> lexer_token = tokens[read_index + 1];
				parser->token_read_index += 1;	
			} else {
				new_operator_node -> right_node = parser_parse_token(parser, NULL);	
			}
			
			return parser_parse_token(parser, new_operator_node);
			
		} else if (tokens[read_index]->type == SEMICOLON) {
			//this is suss as well. Unclear if this will really stop anything 
			//parser->token_read_index += 1;
			return left_node;
		} else if (tokens[read_index]->type == RPAREN) {
			//idea behind this case: If a previous expression was high order due to parentheses, a right parentheses might be the next operator.
			//if this is the case, just skip the parentheses and expect valid infix
			//this is kind of suspicious 
			parser->token_read_index += 1;
			return left_node; //parser_parse_token(parser, left_node);
		} else {
			token * problem_token = tokens[read_index];
			printf("PARSER ERROR (type 2): received invalid token order at line %d, position %d.\n", problem_token->line, problem_token->position);
			printf("Received token: %s\n", problem_token->token_string);
			exit(0);
		}
	}
}


int main() {
	//all of these should pass!	
	//parser_test_math(); 
	//parser_test_prefix(); 
	//parser_test_math_advanced(); 
	//parser_test_paren(); 
	//parser_test_bool();
	//parser_test_let_return();
	//function calls!
	parser_test_function_calls();
}

