output: _parser.o _lexer.o _test_parser.o _print.o _operators.o
	cc -g _parser.o _lexer.o _test_parser.o _print.o _operators.o -o output
	rm *.o 
	./output

_parser.o: parser.c
	cc -c -g parser.c -o _parser.o

_lexer.o: lexer.c
	cc -c -g lexer.c -o _lexer.o

_test_parser.o: test_parser.c
	cc -c -g test_parser.c -o _test_parser.o

_print.o: print.c
	cc -c -g print.c -o _print.o

_operators.o: operators.c
	cc -c -g operators.c -o _operators.o

clean: 
	rm *.o output
