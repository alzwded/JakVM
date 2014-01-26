generator: scanner.l parser.y common.c asmGenerator.c rtGenerator.c
	flex -f scanner.l
	bison -v -t --defines parser.y
	gcc -g -c -o parser.o -I. parser.tab.c
	gcc -g -c -o scanner.o -I. lex.yy.c
	gcc -g -c -o common.o -I. common.c
	gcc -g -c -o asmGenerator.o -I. asmGenerator.c
	gcc -g -c -o rtGenerator.o -I. rtGenerator.c
	gcc -g -o generator *.o

run-tests: generator
	./runtests2.sh

clean:
	rm -f parser.tab.* lex.yy.c parser.output *.o generator
