output: _parser.o _lexer.o _test_parser.o
	cc _parser.o _lexer.o _test_parser.o -o output
	rm *.o 

_parser.o: parser.c
	cc -c -g parser.c -o _parser.o

_lexer.o: lexer.c
	cc -c -g lexer.c -o _lexer.o

_test_parser.o: test_parser.c
	cc -c -g test_parser.c -o _test_parser.o

clean: 
	rm *.o output
