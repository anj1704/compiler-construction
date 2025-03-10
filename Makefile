compiler:
	gcc src/lexer/lexer.c src/parser/parser.c src/driver.c -o compiler

run: compiler
	./compiler > nigga.txt

clean:
	rm -rf compiler compiler_debug

debug:
	gcc src/lexer/lexer.c src/parser/parser.c src/driver.c -o compiler_debug -DDEBUG
	./compiler_debug

all: compiler run
