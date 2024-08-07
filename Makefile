output: parser.o lexer.o
	cc parser.o lexer.o -o output

parser.o: parser.c
	cc -c -g parser.c -o parser.o

lexer.o: lexer.c
	cc -c -g lexer.c -o lexer.o

clean: 
	rm *.o output
