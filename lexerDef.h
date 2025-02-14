#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define tableSize 100000000
#define keywordCount 28
#define terminalCount 59
#define nonTerminalCount 54

#define BUFFER_SIZE (1<<20) // check size of buffer

typedef enum {
    // Program Structure
    program,
    mainFunction,
    otherFunctions,
    function,
    stmts,
    otherStmts,
    stmt,

    // Parameters and Declarations
    parameter_list,
    input_par,
    output_par,
    inputParameters,
    outputParameters,
    declarations,
    declaration,
    global_or_not,

    // Data Types and Type Definitions
    dataType,
    primitiveDatatype,
    constructedDatatype,
    typeDefinitions,
    typeDefinition,
    definetypestmt,
    actualOrRedefined,

    // Field Definitions
    fieldDefinitions,
    fieldDefinition,
    fieldtype,
    moreFields,

    // Statements
    assignmentStmt,
    funCallStmt,
    iterativeStmt,
    conditionalStmt,
    ioStmt,
    returnStmt,
    optionalReturn,
    elsePart,

    // Expressions
    arithmeticExpression,
    expPrime,
    term,
    termPrime,
    factor,
    booleanExpression,
    
    // Operators
    highPrecedenceOperators,
    lowPrecedenceOperators,
    logicalOp,
    relationalOp,

    // Identifiers and Variables
    var,
    idList,
    more_ids,
    singleOrRecId,
    option_single_constructed,

    // Miscellaneous
    A,
    remaining_list,
    oneExpansion,
    moreExpansions
} nonTerminals;

typedef enum {
    // Operators
    TK_PLUS,      // Arithmetic operators
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    
    TK_GT,        // Relational operators
    TK_LT,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    
    TK_AND,       // Logical operators
    TK_OR,
    TK_NOT,
    
    TK_ASSIGNOP,  // Assignment operator

    // Delimiters and Punctuation
    TK_DOT,
    TK_COMMA,
    TK_COLON,
    TK_SEM,       // Semicolon
    TK_OP,        // Opening parenthesis
    TK_CL,        // Closing parenthesis
    TK_SQL,       // Square bracket left
    TK_SQR,       // Square bracket right

    // Identifiers and Constants
    TK_ID,
    TK_FUNID,
    TK_FIELDID,
    TK_RUID,
    TK_NUM,
    TK_RNUM,

    // Keywords for Data Types
    TK_INT,
    TK_REAL,
    TK_LIST,
    TK_RECORD,
    TK_UNION,
    TK_TYPE,

    // Control Flow Keywords
    TK_IF,
    TK_THEN,
    TK_ELSE,
    TK_ENDIF,
    TK_WHILE,
    TK_ENDWHILE,

    // Function and Parameter Keywords
    TK_MAIN,
    TK_PARAMETERS,
    TK_PARAMETER,
    TK_GLOBAL,
    TK_RETURN,
    TK_CALL,
    TK_INPUT,
    TK_OUTPUT,

    // I/O Keywords
    TK_READ,
    TK_WRITE,

    // Structure Definition Keywords
    TK_DEFINETYPE,
    TK_WITH,
    TK_AS,
    TK_END,
    TK_ENDRECORD,
    TK_ENDUNION,

    // Special Tokens
    TK_COMMENT,
    EPS,            // Epsilon
    END_OF_INPUT
} terminals;

char* terminalStrings[] = {
    "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV",
    "TK_GT", "TK_LT", "TK_EQ", "TK_NE", "TK_LE", "TK_GE",
    "TK_AND", "TK_OR", "TK_NOT",
    "TK_ASSIGNOP",
    "TK_DOT", "TK_COMMA", "TK_COLON", "TK_SEM", "TK_OP", "TK_CL", "TK_SQL", "TK_SQR",
    "TK_ID", "TK_FUNID", "TK_FIELDID", "TK_RUID", "TK_NUM", "TK_RNUM",
    "TK_INT", "TK_REAL", "TK_LIST", "TK_RECORD", "TK_UNION", "TK_TYPE",
    "TK_IF", "TK_THEN", "TK_ELSE", "TK_ENDIF", "TK_WHILE", "TK_ENDWHILE",
    "TK_MAIN", "TK_PARAMETERS", "TK_PARAMETER", "TK_GLOBAL", "TK_RETURN", "TK_CALL", "TK_INPUT", "TK_OUTPUT",
    "TK_READ", "TK_WRITE",
    "TK_DEFINETYPE", "TK_WITH", "TK_AS", "TK_END", "TK_ENDRECORD", "TK_ENDUNION",
    "TK_COMMENT", "EPS", "END_OF_INPUT"
};

char* nonTerminalStrings[] = {
    "program", "mainFunction", "otherFunctions", "function", "stmts", "otherStmts", "stmt",
    "parameter_list", "input_par", "output_par", "inputParameters", "outputParameters",
    "declarations", "declaration", "global_or_not",
    "dataType", "primitiveDatatype", "constructedDatatype", "typeDefinitions", "typeDefinition",
    "definetypestmt", "actualOrRedefined",
    "fieldDefinitions", "fieldDefinition", "fieldtype", "moreFields",
    "assignmentStmt", "funCallStmt", "iterativeStmt", "conditionalStmt", "ioStmt",
    "returnStmt", "optionalReturn", "elsePart",
    "arithmeticExpression", "expPrime", "term", "termPrime", "factor", "booleanExpression",
    "highPrecedenceOperators", "lowPrecedenceOperators", "logicalOp", "relationalOp",
    "var", "idList", "more_ids", "singleOrRecId", "option_single_constructed",
    "A", "remaining_list", "oneExpansion", "moreExpansions"
};


//Item of a symbol table
typedef struct symTableItem {
  terminals token;
  char *lexeme;
  char *type;
  int intVal;
  float realVal;
  int lineCount;
  int eof;
  char **parameter;
  struct symTableItem *nextItem;
} SymTableItem;

typedef struct symTable {
SymTableItem *items[tableSize];
int sizeOfTable;
} SymTable;

// Twin Buffering
typedef struct {
char bufferOne[BUFFER_SIZE];
char bufferTwo[BUFFER_SIZE];
} twinBuffer;

typedef struct keyword{
char* key;
terminals token;
} keyword;

SymTable *table;

keyword* keywords[keywordCount];

typedef struct tokenInfo 
{
terminals token; 
char* lexeme;
int lineNo;
struct tokenInfo* next;
} TokenInfo;

FILE *fp;
char *endPtr, *startPtr;
bool buffOneFlag, buffTwoFlag;
int lineCount;
bool isEOF;
bool loadBufferOne = true;
bool loadBufferTwo = true;
twinBuffer tBuff;
