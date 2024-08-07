output: _parser.o _lexer.o
	cc _parser.o _lexer.o -o output
	rm *.o 

_parser.o: parser.c
	cc -c -g parser.c -o _parser.o

_lexer.o: lexer.c
	cc -c -g lexer.c -o _lexer.o

clean: 
	rm *.o output
