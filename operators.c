#include "common.h"

int token_token_is_gramar(token * token) {
	token_type type = token->type;
	if ((type == COMMA) ||(type == END) ||(type == LBRACE) ||(type == RBRACE) ||(type == LPAREN) ||(type == RPAREN) ||(type == EQUAL) ||(type == NOT_EQUAL) || (type == SEMICOLON)) {
		return 1;
	} else {
		return 0;
	}
}

int parser_get_operator_presedence(token * operator_token) {
	switch (operator_token->type) {
		case DIVIDE: //BEDMAS operators
			return 6;
		case MULTIPLY:
			return 5;
		case ADD:
			return 4;
		case SUBTRACT: 
			return 3;
		case COMPARE:	//low prescedence
			return 2;
		case NOT_EQUAL:
			return 2;
		case GT:
			return 2;
		case LT:
			return 2;
		case GTE:
			return 2;
		case LTE:	//super low presedence
			return 2; 
		case AND:
			return 1;
		case OR:
			return 1;
		case EQUAL: //assignment is the lowest prescedence 
			return 0;
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

int is_valid_infix_operator(token * token) {
	token_type type = token->type; 
	if (type == SUBTRACT || type == LPAREN) {
		return 1;
	}
	return 0;
}



