#include "common.h"

ast_node * parser_parse_token(parser * parser);
void parser_free_ast(ast_node * ast_node);
void parser_print_node_and_children(ast_node * ast_node, int depth);
void print_depth(int depth);
ast_node * parser_parse_let(parser * parser);
ast_node * parser_parse_return(parser * parser);
ast_node * new_generic_node();
int token_token_is_gramar(token * token);
int parser_get_operator_presedence(token * operator_token);
int parser_get_next_operator_prescedence(parser * parser);

//expected expressions
token * create_branch_token(); //used parsing if statements 
ast_node * parse_let_token(parser * parser);
ast_node * parse_return_token(parser * parser);
ast_node * parser_parse_prefix(parser * parser); //this thing needs some kind of prescedence?
ast_node * parser_parse_if(parser * parser); //this thing needs some kind of prescedence?
void parser_parse_tokens(parser * parser);

ast_node * parser_parse_infix_expression(parser * parser, ast_node * left_node);
//how to handle if? If node has 2 child expressions. left is the condition. Right is a BRANCH node (this is basically a dummy node which has a left and right expression. During eval, the if is replaced by either the left or right child of the branch node.

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
		parser->token_read_index += 4; //next tokenizer pass should be starting at the expression

		//what I should do 
		//new_let_node->right_node = parser_parse_token(parser); //for now I will not attach values to the nodes.

		//what I will do for now. //IF YOU CHANGE THIS DECREASE THE INCREMENT above
		new_let_node->right_node = new_generic_node();
		new_let_node->right_node->lexer_token = expression;

		//experimenting with an int node reql quick.
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
		parser->token_read_index += 2; //decrease for acutal use
		new_return_node->left_node= new_generic_node();
		new_return_node->left_node->lexer_token = return_value;
		//new_return_node -> left_node = parser_parse_token(parser); 
		return new_return_node;

	} else {
		printf("Invalid return statement detected at line %d, position %d.\n", return_symbol->line, return_symbol->position);
		printf("Expected return expression, Received %s %s...\n", return_symbol->token_string, return_value->token_string);
		exit(0);
	}
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
		default:
			if (current_token->type == IDENTIFIER || current_token->type == INT) {
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

void print_depth(int depth) {
	while (depth > 0) {
		printf("\t");
		depth--;
	}
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
		case ADD:
			//check if this is an expression
			printf("ADD: %s (%d,%d)\n", current->token_string, current->line, current->position);
			depth ++;
			parser_print_node_and_children(ast_node->left_node, depth);
			parser_print_node_and_children(ast_node->right_node, depth);
			break;
		case MULTIPLY:
			printf("MULIPLY: %s (%d,%d)\n", current->token_string, current->line, current->position);
			depth ++;
			parser_print_node_and_children(ast_node->left_node, depth);
			parser_print_node_and_children(ast_node->right_node, depth);
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

int parser_get_operator_presedence(token * operator_token) {
switch (operator_token->type) {
		case DIVIDE:
			return 6;
		case MULTIPLY:
			return 5;
		case ADD:
			return 4;
		case SUBTRACT: 
			return 3;
		default:
			return -1;
	}
}

int parser_get_next_operator_prescedence(parser * parser) {
	int parser_read_index = parser->token_read_index;
	token ** token_array_pointer = parser->program->tokens;	
	token * current_token = token_array_pointer[parser_read_index+1];
	token * next_token = token_array_pointer[parser_read_index+1];
	token * potential_operator_token = token_array_pointer[parser_read_index+2];

	assert(current_token ->type != SEMICOLON);	
	
	if (next_token->type == SEMICOLON) {
		return -1;
	} 

	return parser_get_operator_presedence(potential_operator_token);
	
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
	int current_op_presedence= parser_get_operator_presedence(operator_token);
	int next_op_prescedence = parser_get_next_operator_prescedence(parser);

	//making anything non-algebraic return without recursive call might actually be really good. This could just trigger the 
	//more general parser to let it figure out what to do with the next thing 
	//note, this is not 100% thought out
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

int main() {
	parser_test_return();
	parser_test_let();
	parser_test_math();

}

