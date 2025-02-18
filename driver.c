#include "lexer.h"
#include "parser.h"
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

// Lexer global variables
SymTable *table;
keyword* keywords[keywordCount];
FILE *fp;
char *endPtr, *startPtr;
bool buffOneFlag, buffTwoFlag;
int lineCount;
bool isEOF;
bool loadBufferOne = true;
bool loadBufferTwo = true;
twinBuffer tBuff;

// Parser global variables
followDS *follow_occurrence[nonTerminalCount];
Grammar *G;
non_terminal_sets first_follow_sets[nonTerminalCount];
ParseTable *PT;
helperStack *Stack;
treeNode *root;
gitems **itemList;

int main() {
  G = (Grammar *)malloc(sizeof(Grammar));
  addGrammarRules();
  printf("Grammar rules added\n");
  compute_firsts();
  printf("Computed firsts\n");
  compute_follow();
  printf("Computed follow\n");
  create_parse_table();
  printf("Created parse table\n");
  initiate_parse_table();
  /*print_parse_table();*/

  char *sourceFile = "./Parser Test Cases/t4.txt";
  char *cleanFile = "./Lexer Test Cases/cleaned.txt";
  removeComments(sourceFile, cleanFile);

  FILE *fp = initialise(cleanFile, BUFFER_SIZE);
  if (!fp) {
    fprintf(stderr, "Failed to initialize lexer with file: %s\n", sourceFile);
    return 1;
  }
  printf("initialised file\n");

  createParseTree(fp);
  printParseTree(root, 0);
}
