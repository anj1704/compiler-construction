#include "lexer/lexer.h"
#include "parser/parser.h"
#include <time.h>

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
    "TK_COMMENT", "EPS", "END_OF_INPUT", "TK_ERROR"
};

char* nonTerminalStrings[] = {
    "program", "mainFunction", "otherFunctions", "function", "stmts", "otherStmts", "stmt",
    "parameterList", "inputPar", "outputPar", "inputParameters", "outputParameters",
    "declarations", "declaration", "globalOrNot",
    "dataType", "primitiveDatatype", "constructedDatatype", "typeDefinitions", "typeDefinition",
    "definetypestmt", "actualOrRedefined",
    "fieldDefinitions", "fieldDefinition", "fieldtype", "moreFields",
    "assignmentStmt", "funCallStmt", "iterativeStmt", "conditionalStmt", "ioStmt",
    "returnStmt", "optionalReturn", "elsePart",
    "arithmeticExpression", "expPrime", "term", "termPrime", "factor", "booleanExpression",
    "highPrecedenceOperators", "lowPrecedenceOperators", "logicalOp", "relationalOp",
    "var", "idList", "moreIds", "singleOrRecId", "optionSingleConstructed",
    "A", "remainingList", "oneExpansion", "moreExpansions"
};

// Lexer global variables
SymTable *table;
keyword* keywords[keywordCount];
FILE *fp;
char *endPtr, *startPtr;
bool buffOneFlag, buffTwoFlag;
int lineCount = 1;
bool isEOF;
bool loadBufferOne = true;
bool loadBufferTwo = true;
twinBuffer tBuff;

// Parser global variables
followDS *followOccurrence[nonTerminalCount];
Grammar *G;
nonTerminalSets firstFollowSets[nonTerminalCount];
ParseTable *PT;
gitems **itemList;
Stack* mainStack;
TreeNode* parseTreeRoot;

void Usage() {
  fprintf(stderr, "Usage: ./compiler <sourceFile> <outputFile>\n");
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    Usage();
    return 1;
  }

  clock_t start, end;
  G = (Grammar *)malloc(sizeof(Grammar));
  mainStack = (Stack*)malloc(sizeof(Stack));
  parseTreeRoot = (TreeNode*)malloc(sizeof(TreeNode));
  addGrammarRules();
  printf("Grammar rules added\n");
  computeFirsts();
  printf("Computed first sets\n");
  computeFollow();
  printf("Computed follow\n");
  createParseTable();
  initiateParseTable();
  printf("Created and filled parse table\n");

  char *sourceFile = argv[1];
  char *outputFile = argv[2];

  FILE *fp = initialise(sourceFile, BUFFER_SIZE);
  if (!fp)
  {
    fprintf(stderr, "Failed to initialize lexer with file: %s\n", sourceFile);
    return 1;
  }
  FILE *output = fopen(outputFile, "w");
  if (!output)
  {
    fprintf(stderr, "Failed to open output file: %s\n", outputFile);
    return 1;
  }
  printf("File, buffers and symbol table initialized\n");
  printf("\n***Lexical and syntax analysis modules implemented, all testcases functional***\n");
  int option;

  lineCount = 1;
  start = clock();
  createParseTree(fp);
  end = clock();

  while (1) {
    printf("====================================================================================\n");
    printf("Choose an option:\n");
    printf("0. Exit\n");
    printf("1. Print comment free code on console\n");
    printf("2. Print tokens on console\n");
    printf("3. Perform parsing and print parse tree to file.\n");
    printf("4. Print the time taken to perform parsing\n");
    scanf("%d", &option);
    printf("====================================================================================\n");

    if (option == 0)
      break;

    switch (option) {
    case 1: {
      removeComments(sourceFile);
      break;
    }
    case 2: {
      fclose(fp);
      fp = initialise(sourceFile, BUFFER_SIZE);
      SymTableItem currToken;
      lineCount = 1;
      printf("*");
      for (int i = 0; i < 77; i++) {
        printf("-");
      }
      printf("*\n");
      printf("|%-25s|%-25s|%-25s|\n", "Line Number", "Token", "Lexeme");
      printf("*");
      for (int i = 0; i < 77; i++) {
        printf("-");
      }
      printf("*\n");
      while (1) {
        currToken = getToken(fp);
        currToken.lineCount = lineCount;
        if (currToken.eof) {
          break;
        }
        printf("|%-25d|%-25s|%-25s|\n", currToken.lineCount,
               terminalStrings[currToken.token], currToken.lexeme);
        for (int i = 0; i < 79; i++) {
          printf("-");
        }
        printf("\n");
      }

      break;
    }
    case 3: {
      printf("Parsing.....\n");
      printParseTree(output);
      printf("Please check %s for the Parse Table\n", outputFile);
      break;
    }
    case 4: {
      double totalCPUTime = (double)(end - start);
      double totalCPUTimeInSeconds = totalCPUTime / CLOCKS_PER_SEC;
      printf("Total CPU time taken: %f\n", totalCPUTime);
      printf("Total CPU time in seconds: %f\n", totalCPUTimeInSeconds);
      break;
    }
    default: {
      printf("Please enter a value in [0, 4]\n");
      break;
    }
    }
  }

  fclose(fp);
  fclose(output);
  return 0;
}
