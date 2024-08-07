output: _parser.o _lexer.o
	cc _parser.o _lexer.o -o output
	rm *.o output

_parser.o: parser.c
	cc -c -g parser.c -o parser.o

_lexer.o: lexer.c
	cc -c -g lexer.c -o lexer.o

clean: 
	rm *.o output
