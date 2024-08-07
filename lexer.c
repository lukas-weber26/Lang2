#include "common.h"

int tokenizer_check_next(tokenized_program * program, char char_comparison);
token * token_token_from_char(char input);
void tokenizer_new_token(char * token_string, token_type token_type,tokenized_program * program,int read_jump);
void tokenizer_final_token(tokenized_program * program);
void tokenizer_jump(tokenized_program * program, int line_change);
void tokenizer_delte_token(token * token);
void tokenizer_tokenize_program(tokenized_program * program);
void tokenizer_extract_symbol(tokenized_program* program);
void tokenizer_branch_new_token(tokenized_program * program, char * string_1, char* string_2, token_type type_1, token_type type_2, char compare);
int is_identifier(char * buff);
int is_number(char * buff);
int char_is_letter(char c);
int char_is_number(char c);

void tokenizer_check_program(tokenized_program * program) {
	for (int i = 0; i < program->max_size; i++) {
		if (program->tokens[i]->type == ILLIGAL) {
			token * ct;
			printf("Parsing error detected on line %d, position %d. Problem symbol: %s.\n", ct->line, ct->position, ct->token_string);
			exit(0);
		}
	}
}

tokenized_program * tokenizer_init(char * input_program, int approximate_token_count) {
	tokenized_program * new_program = malloc(sizeof(tokenized_program));
	assert(strlen(input_program));
	new_program->input = strdup(input_program);	
	new_program->token_count = 0;
	new_program->max_size = approximate_token_count;
	new_program->tokens = malloc(sizeof(token *) * approximate_token_count);
	new_program->token_line_number = 0;
	new_program->token_position = 0;
	new_program->read_position = 0;
	return new_program;
}

void tokenizer_new_token(char * token_string, token_type token_type,tokenized_program * program,int read_jump) {
			token * new_token;
			new_token = malloc(sizeof(token));
			new_token->token_string = strdup(token_string);
			new_token->line = program->token_line_number;
			new_token->position = program->token_position;
			new_token->type = token_type;
			program->tokens[program->token_count] = new_token;
			program->token_count ++;
			program->token_position ++;
			program->read_position += read_jump;// ++;
			if (program->token_count == program->max_size) {
				program->max_size *= 2;
				program->tokens = realloc(program->tokens, (sizeof(token *)) * (program->max_size));
			}
}

void tokenizer_final_token(tokenized_program * program) {
	program->max_size = program ->token_count;
	program->tokens = realloc(program->tokens, (sizeof(token *)) * (program->max_size));
}

void tokenizer_jump(tokenized_program * program, int line_change) {
	program->read_position ++;
	program->token_line_number += line_change;
}

int tokenizer_check_next(tokenized_program * program, char char_comparison) {
	int compare = (program->input[program->read_position+1] == char_comparison);
	if (compare) {
		program->read_position++;	
	}
	return compare;
}

void tokenizer_branch_new_token(tokenized_program * program, char * string_1, char* string_2, token_type type_1, token_type type_2, char compare) {
	if (tokenizer_check_next(program, compare)) {
		tokenizer_new_token(string_1, type_1, program, 1);
	} else {
		tokenizer_new_token(string_2, type_2, program, 1);
	}
}

int char_is_number(char c) {
	if (c < '0' || c > '9') {
		return 0;	
	}
	return 1;
}

int char_is_letter(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ) {
		return 1;	
	}
	return 0;
}

int is_number(char * buff) {
	int index = 0; 
	assert(strlen(buff));
	while (buff[index] != '\0') {
		if (!char_is_number(buff[index])) {
			return 0;	
		}
		index ++;
	}

	return 1;
}

int is_identifier(char * buff) {
	int index = 0; 
	assert(strlen(buff));

	if (char_is_letter(buff[index])) {
		index ++;
		while (buff[index] != '\0') {
			if ((!char_is_letter(buff[index])) && (!char_is_number(buff[index])) && (!(buff[index] == '_'))) {
				return 0;	
			}
			index++;
		}
		return 1;
	} 
	return 0;
}

void tokenizer_extract_symbol(tokenized_program* program) {
	char buff [64];
	int count = 0;
	char current = program->input[program->read_position+count];

	while ((char_is_letter(current) || char_is_number(current)) && count < 63) {
		buff[count] = current;

		count++;
		current = program->input[program->read_position+count];
	}

	buff[count] = '\0';

	//can now figure out which identifier is present
	if (strcmp(buff, "let") == 0) {
		tokenizer_new_token("let", LET, program, count);
		return;
	} else if (strcmp(buff, "return") == 0) {
		tokenizer_new_token("return", RETURN, program, count);
		return;
	} else if (strcmp(buff, "function") == 0) {
		tokenizer_new_token("function", FUNCTION, program, count);
		return;
	} else if (strcmp(buff, "true") == 0) {
		tokenizer_new_token("true", TRUE, program, count);
		return;
	} else if (strcmp(buff, "false") == 0) {
		tokenizer_new_token("false", FALSE, program, count);
		return;
	} else if (strcmp(buff, "if") == 0) {
		tokenizer_new_token("if", IF, program, count);
		return;
	} else if (strcmp(buff, "else") == 0) {
		tokenizer_new_token("else", ELSE, program, count);
		return;
	} 

	if (is_number(buff)) {
		tokenizer_new_token(buff, INT, program, count);
		return;
	} else if (is_identifier(buff)) {
		tokenizer_new_token(buff, IDENTIFIER, program, count);
		return;

	}
	
	tokenizer_new_token("illigal", ILLIGAL, program, count);
	return;
}

void tokenizer_tokenize_program(tokenized_program * program) {
	switch (program->input[program->read_position]) {
		case '+': 
			tokenizer_new_token("+", ADD, program, 1);
			break;
		case '-':
			tokenizer_new_token("-", SUBTRACT, program, 1);
			break;
		case '=': 
			tokenizer_branch_new_token(program, "==", "=", EQUAL, COMPARE, '=');
			break;
		case '>': //>=
			tokenizer_branch_new_token(program, ">=", ">", GTE, GT, '=');
			break;
		case '<': //<=
			tokenizer_branch_new_token(program, "<=", "<", LTE, LT, '=');
			break;
		case '!': //!=
			tokenizer_branch_new_token(program, "!=", "=", NOT_EQUAL, BANG, '=');
			break;
		case '&':
			tokenizer_new_token("&", AND, program, 1);
			break;
		case '|':
			tokenizer_new_token("|", OR, program, 1);
			break;
		case '*':
			tokenizer_new_token("*", MULTIPLY, program, 1);
			break;
		case '/':
			tokenizer_new_token("/", DIVIDE, program, 1);
			break;
		case ';':
			tokenizer_new_token(";", SEMICOLON, program, 1);
			break;
		case '(':
			tokenizer_new_token("(", LPAREN, program, 1);
			break;
		case ')':
			tokenizer_new_token(")", RPAREN, program, 1);
			break;
		case '{':
			tokenizer_new_token("{", LBRACE, program, 1);
			break;
		case '}':
			tokenizer_new_token("}", RBRACE, program, 1);
			break;
		case ',':
			tokenizer_new_token(",", COMMA, program, 1);
			break;
		case '\0':
			tokenizer_new_token("EOF", END, program, 1);
			tokenizer_final_token(program);	
			return; //important return. In this case, we are done parsing.
		case '\n':
			tokenizer_jump(program,1);
			program->token_position = 0;
			break;
		case ' ':
			tokenizer_jump(program,0);
			break;
		case '\r':
			tokenizer_jump(program,0);
			break;
		default:
			tokenizer_extract_symbol(program);	
			break;
	}

	tokenizer_tokenize_program(program);
}

void tokenizer_print_tokens(tokenized_program * program) {
	printf("Token count: %d. Token capacity: %d.\n", program->token_count, program->max_size);
	for (int i = 0; i < program->token_count; i++) {
		token * ct = program->tokens[i];
		printf(">> %s (Type: %d). Line: %d, Position: %d\n", ct->token_string, ct->type, ct->line, ct->position);
	}
}

void tokenizer_delte_token(token * token) {
	free(token->token_string);
	free(token);
}

void tokenizer_delete_program(tokenized_program * program) {
	for (int i = 0; i < program->token_count; i++) {
		tokenizer_delte_token(program->tokens[i]);
		program->tokens[i] = NULL;
	}	
	free(program->tokens);
	free(program->input);
	free(program);
	program = NULL;
}

//INT, FLOAT,STRING, IDENTIFIER, LET,  RETURN, FUNCTION, ADD, SUBTRACT,COMPARE, GT, LT, GTE, LTE, OR, AND, TRUE, FALSE, IF, ELSE, EQUAL,NOT_EQUAL,DIVIDE,MULTIPLY, BANG, ILLIGAL,SEMICOLON,LPAREN, RPAREN, LBRACE,RBRACE,END, COMMA	
//step 2: write a unit test
void test_tokenizer() {
	char * input = "a == 10;\n"
	"10;\n"
	"5 < 10; let return;\n"
	"(test this, oof);\n"
	"not != doing;\n"
	"{(;\n"
	"if (test,test2) else;\n";

	tokenized_program * program = tokenizer_init(input, 4);
	tokenizer_tokenize_program(program);

	#define test_length 36
	token test_tokens [test_length] = {
	{"a", IDENTIFIER, 0, 0},
	{"==", EQUAL, 0, 1},
	{"10", INT, 0, 2},
	{";", SEMICOLON, 0, 3},

	{"10", INT, 1, 0},
	{";", SEMICOLON, 1, 1},
	
	{"5", INT, 2, 0},
	{"<", LT, 2, 1},
	{"10", INT, 2, 2},
	{";", SEMICOLON, 2, 3},
	{"let", LET, 2, 4},
	{"return", RETURN, 2, 5},
	{";", SEMICOLON, 2, 6},

	{"(", LPAREN, 3, 0},
	{"test", IDENTIFIER, 3, 1},
	{"this", IDENTIFIER, 3, 2},
	{",", COMMA, 3, 3},
	{"oof", IDENTIFIER, 3, 4},
	{")", RPAREN, 3, 5},
	{";", SEMICOLON, 3, 6},

	{"not", IDENTIFIER, 4, 0},
	{"!=", NOT_EQUAL, 4, 1},
	{"doing", IDENTIFIER, 4, 2},
	{";", SEMICOLON, 4, 3},
	
	{"{", LBRACE, 5, 0},
	{"(", LPAREN, 5, 1},
	{";", SEMICOLON, 5, 2},

	{"if", IF, 6, 0},
	{"(", LPAREN, 6, 1},
	{"test", IDENTIFIER, 6, 2},
	{",", COMMA, 6, 3},
	{"test2", IDENTIFIER, 6, 4},
	{")", RPAREN, 6, 5},
	{"else", ELSE, 6, 6},
	{";", SEMICOLON, 6, 7},
	{"EOF", END, 7, 0},
	};
	
	assert(program->token_count == test_length);
	for (int i = 0; i < test_length; i++) {
		token * ct = program->tokens[i];
		printf(">> %s (Type: %d). Line: %d, Position: %d\n", ct->token_string, ct->type, ct->line, ct->position);
		assert((test_tokens[i].type == program->tokens[i]->type));
		assert((test_tokens[i].line== program->tokens[i]->line));
		assert((test_tokens[i].position== program->tokens[i]->position));
		assert(strcmp(test_tokens[i].token_string,program->tokens[i]->token_string) == 0);
	}

	tokenizer_delete_program(program);
}

