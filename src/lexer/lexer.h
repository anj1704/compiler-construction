#ifndef LEXER
#define LEXER
#include "lexerDef.h"

FILE *initialise(char *inputFile, long long int buff_size);
void initializeSymbolTable();
int hash(char *lexeme);
void insert(char *lexeme, terminals token);
SymTableItem *lookup(char *lexeme);
void initializeKeywords();
SymTableItem error_helper(int error, char *lex, int line);
char *getLexeme();
FILE *getstream(FILE *fp);
void removeComments(char *testFile);
char getNextCharacter(FILE *fp);
terminals findKeyword(char *lexeme);
SymTableItem tokenize(char *lex, terminals g, int line);
SymTableItem getToken(FILE *fp);
TokenInfo *createTokenNode(terminals token, char *lexeme, int lineNo);
void appendTokenNode(TokenInfo **head, terminals token, char *lexeme,
                     int lineNo);
void printTokens(TokenInfo *head);
void freeTokenList(TokenInfo *head);
#endif
