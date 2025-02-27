compiler:
	gcc src/lexer/lexer.c src/parser/parser.c src/driver.c -o compiler

run: compiler
	./compiler

clean:
	rm compiler compiler_debug

debug:
	gcc src/lexer/lexer.c src/parser/parser.c src/driver.c -o compiler_debug -DDEBUG
	./compiler_debug

all: compiler
