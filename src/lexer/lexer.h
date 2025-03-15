#ifndef LEXER
#define LEXER
#include "lexerDef.h"

/* 
 * Function: initialise
 * --------------------
 * Initializes the lexer by allocating memory for buffers, opening the input file, 
 * reading the first chunk of data into the buffer, and setting up the symbol table.
 *
 * Input: 
 * - inputFile: The path to the source code file to be analyzed.
 * - buffSize: The size of the buffer for reading input.
 *
 * Output:
 * - Returns a FILE pointer to the opened file.
 */
FILE *initialise(char *inputFile, long long int buffSize);

/* 
 * Function: initializeSymbolTable
 * --------------------------------
 * Initializes the symbol table by allocating memory and inserting predefined keywords.
 *
 * Input: None
 *
 * Output: None
 */
void initializeSymbolTable();

/* 
 * Function: hash
 * --------------
 * Computes a hash value for the given lexeme using the DJB2 hashing algorithm.
 *
 * Input:
 * - lexeme: The string to be hashed.
 *
 * Output:
 * - Returns an integer representing the hash index in the symbol table.
 */
int hash(char *lexeme);

/* 
 * Function: insert
 * ----------------
 * Inserts a lexeme-token pair into the symbol table.
 *
 * Input:
 * - lexeme: The lexeme (string) to be inserted.
 * - token: The corresponding terminal token type.
 *
 * Output: None
 */
void insert(char *lexeme, terminals token);

/* 
 * Function: lookup
 * ----------------
 * Searches for a given lexeme in the symbol table.
 *
 * Input:
 * - lexeme: The string to search for.
 *
 * Output:
 * - Returns a pointer to the found symbol table entry or NULL if not found.
 */
SymTableItem *lookup(char *lexeme);

/* 
 * Function: initializeKeywords
 * ----------------------------
 * Create a list of keyword tokens to insert into the symbol table.
 *
 * Input: None
 *
 * Output: None
 */
void initializeKeywords();

/* 
 * Function: errorHelper
 * ---------------------
 * Handles lexical errors by creating an error token with a corresponding message.
 *
 * Input:
 * - error: Error code indicating the type of error.
 * - lex: The problematic lexeme.
 * - line: The line number where the error occurred.
 *
 * Output:
 * - Returns a SymTableItem structure representing the error token.
 */
SymTableItem errorHelper(int error, char *lex, int line);

/* 
 * Function: getLexeme
 * -------------------
 * Extracts the current lexeme from the input buffer.
 *
 * Input: None
 *
 * Output:
 * - Returns a dynamically allocated string containing the lexeme.
 */
char *getLexeme();

/* 
 * Function: getstream
 * -------------------
 * Loads a new segment of the source file into the buffer when needed.
 *
 * Input:
 * - fp: A FILE pointer to the source file.
 *
 * Output:
 * - Returns the updated FILE pointer.
 */
FILE *getstream(FILE *fp);

/* 
 * Function: removeComments
 * ------------------------
 * Removes comments from the source code file and prints the cleaned content.
 *
 * Input:
 * - testFile: The path to the source file.
 *
 * Output: None
 */
void removeComments(char *testFile);

/* 
 * Function: getNextCharacter
 * --------------------------
 * Fetches the next character from the input buffer, handling buffer transitions.
 *
 * Input:
 * - fp: A FILE pointer to the source file.
 *
 * Output:
 * - Returns the next character in the input stream.
 */
char getNextCharacter(FILE *fp);

/* 
 * Function: findKeyword
 * ---------------------
 * Checks if a lexeme matches a keyword.
 *
 * Input:
 * - lexeme: The string to be checked.
 *
 * Output:
 * - Returns the corresponding terminal token if the lexeme is a keyword; otherwise, -1.
 */
terminals findKeyword(char *lexeme);

/* 
 * Function: tokenize
 * ------------------
 * Converts a lexeme into a token and inserts it into the symbol table.
 *
 * Input:
 * - lex: The lexeme to tokenize.
 * - g: The terminal token type.
 * - line: The line number where the lexeme was found.
 *
 * Output:
 * - Returns a SymTableItem representing the token.
 */
SymTableItem tokenize(char *lex, terminals g, int line);

/* 
 * Function: getToken
 * ------------------
 * Retrieves the next token from the input stream using a DFA-based approach.
 *
 * Input:
 * - fp: A FILE pointer to the source file.
 *
 * Output:
 * - Returns a SymTableItem representing the next token.
 */
SymTableItem getToken(FILE *fp);

/* 
 * Function: cleanTable
 * --------------------
 * Frees memory allocated for the symbol table and resets it.
 *
 * Input: None
 *
 * Output: None
 */
void cleanTable();

/* 
 * Function: cleanKeywords
 * --------------------
 * Frees memory allocated for the keywords
 *
 * Input: None
 *
 * Output: None
 */
void cleanKeywords();

/* 
 * Function: retract
 * --------------------
 * Moves endPtr back by 1. Wraps around the buffers if underflow is detected.
 *
 * Input: None
 *
 * Output: None
 */
void retract();
#endif
