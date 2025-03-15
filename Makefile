CC          = gcc
DEBUGFLAGS  = -DDEBUG
SRCS        = src/lexer/lexer.c src/parser/parser.c src/driver.c
EXE         = stage1exe
DEBUG_EXE   = compiler_debug

.SILENT:
.PHONY: all clean run debug

all: $(EXE)

$(EXE): $(SRCS)
	$(CC) $(SRCS) -o $(EXE)

run: $(EXE)
	./$(EXE) $(filter-out $@,$(MAKECMDGOALS))

debug: $(SRCS)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(SRCS) -o $(DEBUG_EXE)
	./$(DEBUG_EXE) $(filter-out $@,$(MAKECMDGOALS))

clean:
	rm -f $(EXE) $(DEBUG_EXE)

%:
	@:
