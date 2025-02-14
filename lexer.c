#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include <string.h>

void removeComments(char *testcaseFile, char *cleanFile) {
  FILE *testcaseFp = fopen(testcaseFile, "r");
  FILE *cleanFp = fopen(cleanFile, "w");

  if (!testcaseFp) {
    printf("Opening testcase file failed\n");
    return;
  }

  if (!cleanFp) {
    printf("Opening clean file failed\n");
    return;
  }

  char *line = malloc(200 * sizeof(char));
  bool isComment = false;

  while (fgets(line, 200, testcaseFp)) {
    if (!line) {
      break;
    }

    isComment = false;
    for (int i = 0; line[i] != '\n'; ++i) {
      if (line[i] == ' ') {
        continue;
      } else if (line[i] == '%') {
        isComment = true;
        break;
      } else {
        isComment = false;
        break;
      }
    }

    if (!isComment) {
      fputs(line, cleanFp);
    }
  }

  fclose(testcaseFp);
  fclose(cleanFp);
}

// initialising lexer
FILE *initialise(char *inputFile, long long int buff_size) {
  memset(tBuff.bufferOne, 0, sizeof(tBuff.bufferOne));
  memset(tBuff.bufferTwo, 0, sizeof(tBuff.bufferTwo));
  FILE *fileptr = fopen(inputFile, "r");
  if (fileptr == NULL) {
    printf("ERROR! File not opened.\n");
  }
  int size = fread(tBuff.bufferOne, sizeof(char), BUFFER_SIZE, fileptr);
  if (size < BUFFER_SIZE) {
    tBuff.bufferOne[size] = EOF;
  }

  loadBufferOne = false;
  loadBufferTwo = true;
  // lineCount = 0;
  isEOF = false;
  startPtr = tBuff.bufferOne;
  endPtr = tBuff.bufferOne;
  initializeSymbolTable();
  return fileptr;
}
void initializeKeywords() {
  keyword key[] = {
      {"_main", TK_MAIN},
      {"as", TK_AS},
      {"call", TK_CALL},
      {"definetype", TK_DEFINETYPE},
      {"else", TK_ELSE},
      {"end", TK_END},
      {"endif", TK_ENDIF},
      {"endrecord", TK_ENDRECORD},
      {"endunion", TK_ENDUNION},
      {"endwhile", TK_ENDWHILE},
      {"global", TK_GLOBAL},
      {"if", TK_IF},
      {"input", TK_INPUT},
      {"int", TK_INT},
      {"list", TK_LIST},
      {"output", TK_OUTPUT},
      {"parameters", TK_PARAMETERS},
      {"parameter", TK_PARAMETER},
      {"read", TK_READ},
      {"real", TK_REAL},
      {"record", TK_RECORD},
      {"return", TK_RETURN},
      {"then", TK_THEN},
      {"type", TK_TYPE},
      {"union", TK_UNION},
      {"while", TK_WHILE},
      {"with", TK_WITH},
      {"write", TK_WRITE},
  };

  for (int i = 0; i < keywordCount; i++) {
    keywords[i] = (keyword *)malloc(sizeof(keyword));

    if (keywords[i] == NULL) {
      fprintf(stderr, "Memory allocation failed for keywords %d\n", i);
      exit(EXIT_FAILURE);
    }

    keywords[i]->key = strdup(key[i].key);
    keywords[i]->token = key[i].token;
  }
}

// Function to initialize SymbolTable
void initializeSymbolTable() {
  table = (SymTable *)malloc(sizeof(SymTable));
  if (table == NULL) {
    fprintf(stderr, "Memory allocation failed for SymbolTable.\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < tableSize; i++) {
    table->items[i] = NULL;
  }
  table->sizeOfTable = 0;
  initializeKeywords();
  // Insert keywords into the symbol table
  for (int i = 0; i < keywordCount; i++) {
    insert(keywords[i]->key, keywords[i]->token);
  }
  return;
}

// Function to calculate hash
int hash(char *lexeme) {
  unsigned long hash = 5381;
  int c;
  while ((c = *lexeme++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash % tableSize;
}

// Function to insert into SymbolTable
void insert(char *lexeme, terminals token) {
  int index = hash(lexeme);

  // Check if the lexeme already exists
  if (!lookup(lexeme)) {
    // Lexeme doesn't exist, create a new SymbolItem
    SymTableItem *newItem = (SymTableItem *)malloc(sizeof(SymTableItem));
    if (newItem == NULL) {
      fprintf(stderr, "Memory allocation failed for SymbolItem.\n");
      exit(EXIT_FAILURE);
    }
    newItem->lexeme = strdup(lexeme); // Make a copy of lexeme
    if (newItem->lexeme == NULL) {
      fprintf(stderr, "Memory allocation failed for lexeme.\n");
      exit(EXIT_FAILURE);
    }
    newItem->token = token;
    newItem->nextItem = NULL; // Initialize next pointer

    // Insert newItem into the symbol table
    if (table->items[index] == NULL) {
      // No collision, insert newItem directly
      table->items[index] = newItem;
    } else {
      // Collision occurred, append newItem to the existing linked list
      SymTableItem *current = table->items[index];
      while (current->nextItem != NULL) {
        current = current->nextItem;
      }
      current->nextItem = newItem;
    }
    table->sizeOfTable++;
  } else {
    // Lexeme already exists, handle as needed
    printf("Lexeme %s already exists in the symbol table.\n", lexeme);
  }
}

// Function to lookup a lexeme in SymbolTable
bool lookup(char *lexeme) {
  int index = hash(lexeme);

  // Traverse the linked list at index to find the lexeme
  SymTableItem *current = table->items[index];
  while (current != NULL) {
    if (strcmp(current->lexeme, lexeme) == 0) {
      // Lexeme found in the symbol table
      return true;
    }
    current = current->nextItem;
  }
  // Lexeme not found in the symbol table
  return false;
}

// error handling
SymTableItem error_helper(int error, char *lex, int line) {
  SymTableItem nextSymbolItem;
  nextSymbolItem.token = 0;
  nextSymbolItem.lexeme = NULL;
  nextSymbolItem.intVal = 0;
  nextSymbolItem.realVal = 0.00;
  nextSymbolItem.lineCount = line;
  if (error == -2)
    printf("Identified lexical error at line %d. Type: Unknown character %s\n",
           line, lex);
  else if (error == -3)
    printf("Identified lexical error at line %d. Type: Length of variable "
           "identifier more than 20 in %s\n",
           line, lex);
  else if (error == -4)
    printf("Identified lexical error at line %d. Type: Length of function "
           "identifier more than 30 in %s\n",
           line, lex);
  else
    printf("Identified lexical error at line %d. Type: Wrong sequence of "
           "characters in %s\n",
           line, getLexeme());
  startPtr = endPtr;
  return nextSymbolItem;
}

char *getLexeme() {
  int length_lexeme;
  char *lexeme;

  if ((startPtr >= tBuff.bufferOne &&
       startPtr < tBuff.bufferOne + BUFFER_SIZE) &&
      (endPtr >= tBuff.bufferOne &&
       endPtr <= tBuff.bufferOne + BUFFER_SIZE)) {

    length_lexeme = endPtr - startPtr;
  }

  else if ((startPtr >= tBuff.bufferTwo &&
            startPtr < tBuff.bufferTwo + BUFFER_SIZE) &&
           (endPtr >= tBuff.bufferTwo &&
            endPtr <= tBuff.bufferTwo + BUFFER_SIZE)) {

    length_lexeme = endPtr - startPtr;
  } else {
    length_lexeme = tBuff.bufferOne + BUFFER_SIZE - startPtr +
                    endPtr - tBuff.bufferTwo;
  }

  lexeme = (char *)malloc((length_lexeme + 1) * sizeof(char));

  if (!lexeme) {
    fprintf(stderr, "Mem allocation failed");
    exit(EXIT_FAILURE);
  }

  if ((startPtr >= tBuff.bufferOne &&
       startPtr < tBuff.bufferOne + BUFFER_SIZE) &&
      (endPtr >= tBuff.bufferOne &&
       endPtr <= tBuff.bufferOne + BUFFER_SIZE)) {

    strncpy(lexeme, startPtr, length_lexeme);
  }

  else if ((startPtr >= tBuff.bufferTwo &&
            startPtr < tBuff.bufferTwo + BUFFER_SIZE) &&
           (endPtr >= tBuff.bufferTwo &&
            endPtr <= tBuff.bufferTwo + BUFFER_SIZE)) {
    strncpy(lexeme, startPtr, length_lexeme);

  } else {

    int firstPartLength = tBuff.bufferOne + BUFFER_SIZE - startPtr;
    strncpy(lexeme, startPtr, firstPartLength);
    strncpy(lexeme + firstPartLength, tBuff.bufferTwo,
            length_lexeme - firstPartLength);
  }

  lexeme[length_lexeme] = '\0';

  return lexeme;
}

FILE *getstream(FILE *fp) {
  // If the buffer is completed, load character stream to next buffer from file
  if (endPtr == tBuff.bufferOne + BUFFER_SIZE - 1) {
    if (loadBufferTwo) {
      size_t size =
          fread(tBuff.bufferTwo, sizeof(char), BUFFER_SIZE, fp);
      if (size < BUFFER_SIZE) {
        tBuff.bufferTwo[size] = EOF;
      }
    }
    loadBufferOne = true;
    endPtr = tBuff.bufferTwo;
    loadBufferTwo = false;
  } else if (endPtr == tBuff.bufferTwo + BUFFER_SIZE - 1) {
    if (loadBufferOne) {
      size_t size =
          fread(tBuff.bufferOne, sizeof(char), BUFFER_SIZE, fp);
      if (size < BUFFER_SIZE) {
        tBuff.bufferOne[size] = EOF;
      }
    }
    loadBufferTwo = true;
    endPtr = tBuff.bufferOne;
    loadBufferOne = false;
  } else {
    endPtr++;
  }
  return fp;
}

// DFA Handle....
char getNextCharacter(FILE *fp) {
  char ch = *endPtr;
  if (ch == EOF) {
    isEOF = true;
  }
  getstream(fp);

  return ch;
}

// FINDING KEYWORDS
terminals findKeyword(char *lexeme) {
  for (int i = 0; i < keywordCount; i++) {
    if (strcmp(lexeme, keywords[i]->key) == 0) {
      return keywords[i]->token;
    }
  }
  return -1;
}

// tokenize
SymTableItem tokenize(char *lex, terminals g, int line) {
  SymTableItem nextSymbolItem;
  nextSymbolItem.lexeme = lex;
  nextSymbolItem.lineCount = line;
  nextSymbolItem.token = g;
  nextSymbolItem.intVal = 0;
  nextSymbolItem.realVal = 0.0;
  switch (g) {
  case TK_NUM:
    nextSymbolItem.intVal = atoi(lex);
    break;
  case TK_RNUM:
    nextSymbolItem.realVal = atof(lex);
    break;
  case TK_FIELDID:
    nextSymbolItem.token = findKeyword(nextSymbolItem.lexeme);
    if (nextSymbolItem.token == -1) {
      nextSymbolItem.token = TK_FIELDID;
    }
  case TK_ID:
    if (strlen(nextSymbolItem.lexeme) > 20) {
      return error_helper(-3, lex, line);
    }
    break;
  case TK_FUNID:
    if (strlen(nextSymbolItem.lexeme) > 30) {
      return error_helper(-4, lex, line);
    }
    break;
  }
  startPtr = endPtr;
  return nextSymbolItem;
}
SymTableItem getToken(FILE *fp) {
  startPtr = endPtr;
  char ch = getNextCharacter(fp);
  int dfastate = 0;
  SymTableItem newSymbolItem;

  while (dfastate >= 0) {

    if (isEOF) {
      newSymbolItem.eof = true;
      newSymbolItem.lexeme = NULL;
      newSymbolItem.lineCount = lineCount;
      return newSymbolItem;
    }

    switch (dfastate) {
    case 0:
      switch (ch) {
      case '\t':
        startPtr = endPtr;
        ch = getNextCharacter(fp);
        break;
      case ' ':
        startPtr = endPtr;
        ch = getNextCharacter(fp);
        break;
      case '\n':
        startPtr = endPtr;
        lineCount++;
        ch = getNextCharacter(fp);
        break;
      case '+':
        return tokenize("+", TK_PLUS, lineCount);
        dfastate = 30;
        break;
      case '/':
        dfastate = 33;
        break;
      case '-':
        dfastate = 31;
        break;
      case '*':
        dfastate = 32;
        break;
      case ')':
        return tokenize(")", TK_CL, lineCount);
        dfastate = 29;
        break;
      case ';':
        return tokenize(";", TK_SEM, lineCount);
        // dfastate = 25;
        break;
      case ':':
        return tokenize(":", TK_COLON, lineCount);
        // dfastate = 26;
        break;
      case '.':
        dfastate = 27;
        break;
      case '(':
        return tokenize("(", TK_CL, lineCount);
        // dfastate = 28;
        break;
      case ',':
        return tokenize(",", TK_COMMA, lineCount);
        dfastate = 24;
        break;
      case '~':
        dfastate = 40;
        break;
      case '@':
        dfastate = 37;
        break;
      case '[':
        return tokenize("[", TK_SQL, lineCount);
        // dfastate = 41;
        break;
      case ']':
        return tokenize("]", TK_SQR, lineCount);
        // dfastate = 42;
        break;
      case '%':
        do {
          ch = getNextCharacter(fp);
        } while (ch != '\n' && ch != EOF);

        // while((ch==getNextCharacter(fp))!='\n' && ch!=EOF){

        // }
        endPtr--;
        return tokenize("%", TK_COMMENT, lineCount);
        // dfastate = 56;
        break;
      case '_':
        dfastate = 7;
        break;
      case '#':
        dfastate = 11;
        break;
      case '&':
        dfastate = 34;
        break;
      case '=':
        dfastate = 43;
        break;
      case '<':
        dfastate = 47;
        break;
      case '>':
        dfastate = 53;
        break;
      case '!':
        dfastate = 45;
        break;
      default:
        if (ch >= 'b' && ch <= 'd') {
          dfastate = 1;
        }

        else if ((ch == 'a' || ch >= 'e')) {
          dfastate = 5;
        }

        else if (ch >= '0' && ch <= '9') {
          dfastate = 14;
        } else {
          dfastate = -2;
        }
      }
      break;

    case 1:
      ch = getNextCharacter(fp);

      if (ch >= '2' && ch <= '7') {
        dfastate = 2;
      } else if (ch >= 'a' && ch <= 'z') {
        dfastate = 5;
      } else {
        dfastate = -5;
      }
      break;

    case 2:

      ch = getNextCharacter(fp);

      if (ch >= 'b' && ch <= 'd') {
        dfastate = 2;
      } else if (ch >= '2' && ch <= '7') {
        dfastate = 3;
      } else {
        dfastate = 4;
      }
      break;

    case 3:
      ch = getNextCharacter(fp);

      if (ch >= '2' && ch <= '7') {
        dfastate = 3;
      } else {
        dfastate = 4;
      }
      break;

    case 4:
      endPtr--;
      return tokenize(getLexeme(), TK_ID, lineCount);
      break;

    case 5:
      ch = getNextCharacter(fp);
      if (ch >= 'a' && ch <= 'z') {
        dfastate = 5;
      } else {
        dfastate = 6;
      }
      break;

    case 6:
      endPtr--;
      if (strcmp(getLexeme(), "|") == 0) {
        dfastate = -2;
      } else {
        return tokenize(getLexeme(), TK_FIELDID, lineCount);
      }
      break;
    case 7:
      ch = getNextCharacter(fp);

      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        dfastate = 8;
      } else {
        dfastate = -5;
      }

      break;

    case 8:
      ch = getNextCharacter(fp);

      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        dfastate = 8;
      } else if (ch >= '0' && ch <= '9') {
        dfastate = 9;
      } else {
        dfastate = 10;
      }
      break;

    case 9:
      ch = getNextCharacter(fp);

      if (ch >= '0' && ch <= '9') {
        dfastate = 9;
      } else {
        dfastate = 10;
      }
      break;

    case 10:
      endPtr--;
      return tokenize(getLexeme(), TK_FUNID, lineCount);
      break;

    case 11:

      ch = getNextCharacter(fp);

      if (ch >= 'a' && ch <= 'z') {
        dfastate = 12;
      } else {
        dfastate = -5;
      }
      break;

    case 12:

      ch = getNextCharacter(fp);

      if (ch >= 'a' && ch <= 'z') {
        dfastate = 12;
      } else {
        dfastate = 13;
      }
      break;

    case 13:
      endPtr--;
      return tokenize(getLexeme(), TK_RUID, lineCount);
      break;

    case 14:

      ch = getNextCharacter(fp);

      if (ch == '.') {
        dfastate = 15;
      } else if (ch >= '0' && ch <= '9') {
        dfastate = 14;
      } else {
        dfastate = 22;
      }

      break;

    case 15:

      ch = getNextCharacter(fp);

      if (ch >= '0' && ch <= '9') {
        dfastate = 16;
      } else {
        dfastate = -5;
      }
      break;

    case 16:
      ch = getNextCharacter(fp);

      if (ch >= '0' && ch <= '9') {
        dfastate = 17;
      } else {
        endPtr--;
        dfastate = -5;
      }
      break;

    case 17:
      ch = getNextCharacter(fp);

      if (ch == 'E') {
        dfastate = 18;
      } else {
        dfastate = 23;
      }
      break;

    case 18:
      ch = getNextCharacter(fp);

      if (ch == '+' || ch == '-') {
        dfastate = 19;
      } else if (ch >= '0' && ch <= '9') {
        dfastate = 20;
      } else {
        dfastate = -5;
      }
      break;

    case 19:
      ch = getNextCharacter(fp);

      if (ch >= '0' && ch <= '9') {
        dfastate = 20;
      } else {
        dfastate = -5;
      }
      break;

    case 20:
      ch = getNextCharacter(fp);

      if (ch >= '0' && ch <= '9') {
        dfastate = 21;
      } else {

        dfastate = -5;
      }
      break;
    case 21:
      endPtr++;
      dfastate = 23;
      break;
    case 22:
      endPtr--;
      return tokenize(getLexeme(), TK_NUM, lineCount);
      break;

    case 23:
      endPtr--;
      return tokenize(getLexeme(), TK_RNUM, lineCount);
      break;

    case 24:
      return tokenize(getLexeme(), TK_COMMA, lineCount);
      break;

    case 25:
      return tokenize(getLexeme(), TK_SEM, lineCount);
      break;

    case 26:
      return tokenize(getLexeme(), TK_COLON, lineCount);
      break;

    case 27:
      return tokenize(getLexeme(), TK_DOT, lineCount);
      break;

    case 28:
      return tokenize(getLexeme(), TK_OP, lineCount);
      break;

    case 29:
      return tokenize(getLexeme(), TK_CL, lineCount);
      break;

    case 30:
      return tokenize(getLexeme(), TK_PLUS, lineCount);
      break;

    case 31:
      return tokenize(getLexeme(), TK_MINUS, lineCount);
      break;

    case 32:
      return tokenize(getLexeme(), TK_MUL, lineCount);
      break;

    case 33:
      return tokenize(getLexeme(), TK_DIV, lineCount);
      break;

    case 34:
      ch = getNextCharacter(fp);

      if (ch == '&') {
        dfastate = 35;
      } else {
        dfastate = -5;
      }
      break;

    case 35:
      ch = getNextCharacter(fp);

      if (ch == '&') {
        dfastate = 36;
      } else {
        endPtr--;
        dfastate = -5;
      }
      break;

    case 36:
      return tokenize(getLexeme(), TK_AND, lineCount);
      break;

    case 37:
      ch = getNextCharacter(fp);

      if (ch == '@') {
        dfastate = 38;
      } else {
        dfastate = -5;
      }
      break;

    case 38:
      ch = getNextCharacter(fp);

      if (ch == '@') {
        dfastate = 39;
      } else {
        dfastate = -5;
      }
      break;

    case 39:
      return tokenize(getLexeme(), TK_OR, lineCount);
      break;

    case 40:
      return tokenize(getLexeme(), TK_NOT, lineCount);
      break;

    case 41:
      return tokenize(getLexeme(), TK_SQL, lineCount);
      break;

    case 42:
      return tokenize(getLexeme(), TK_SQR, lineCount);
      break;

    case 43:
      ch = getNextCharacter(fp);

      if (ch == '=') {
        dfastate = 44;
      } else {
        endPtr--;
        dfastate = -5;
      }
      break;

    case 44:
      return tokenize(getLexeme(), TK_EQ, lineCount);
      break;

    case 45:
      ch = getNextCharacter(fp);

      if (ch == '=') {
        dfastate = 46;
      } else {
        dfastate = -5;
      }
      break;

    case 46:
      return tokenize(getLexeme(), TK_NE, lineCount);
      break;

    case 47:

      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 50;
      } else if (ch == '=') {
        return tokenize("<=", TK_LE, lineCount);
        // dfastate = 49;
      } else {
        endPtr--;
        return tokenize("<", TK_LT, lineCount);
        // dfastate = 48;
      }
      break;

    case 48:
      return tokenize(getLexeme(), TK_LT, lineCount);
      break;

    case 49:
      return tokenize(getLexeme(), TK_LE, lineCount);
      break;

    case 50:
      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 51;
      } else {
        dfastate = -5;
      }
      break;

    case 51:
      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 52;
      } else {
        dfastate = -5;
      }
      break;

    case 52:
      return tokenize(getLexeme(), TK_ASSIGNOP, lineCount);
      break;

    case 53:
      ch = getNextCharacter(fp);

      if (ch == '=') {
        dfastate = 55;
      } else {
        dfastate = 54;
      }
      break;

    case 54:
      return tokenize(getLexeme(), TK_GT, lineCount);
      break;

    case 55:
      return tokenize(getLexeme(), TK_GE, lineCount);
      break;

    case 56:
      ch = getNextCharacter(fp);

      if (ch == '\n') {
        dfastate = 57;
      } else {
        dfastate = 56;
      }
      break;

    case 57:
      return tokenize(getLexeme(), TK_COMMENT, lineCount);
      break;
    }
    if (dfastate == -2)

    {
      return error_helper(-2, getLexeme(), lineCount);
    }

    if (dfastate == -5) {
      return error_helper(-5, getLexeme(), lineCount);
    }
  }
  newSymbolItem.lexeme = NULL;
  newSymbolItem.lineCount = -1;
  newSymbolItem.token = -1;
  newSymbolItem.intVal = 0;
  newSymbolItem.realVal = 0.0;
  return newSymbolItem;
}

TokenInfo *createTokenNode(terminals token, char *lexeme, int lineNo) {
  TokenInfo *node = (TokenInfo *)malloc(sizeof(TokenInfo));
  if (!node) {
    printf("Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  node->token = token;
  node->lexeme = strdup(lexeme);
  node->lineNo = lineNo;
  node->next = NULL;
  return node;
}

void appendTokenNode(TokenInfo **head, terminals token, char *lexeme,
                     int lineNo) {
  TokenInfo *newNode = createTokenNode(token, lexeme, lineNo);
  if (*head == NULL) {
    *head = newNode;
  } else {
    TokenInfo *temp = *head;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = newNode;
  }
}

void printTokens(TokenInfo *head) {
  TokenInfo *current = head;
  while (current != NULL) {
    printf("LineNo: %d , Token: %s , Lexeme: %s\n", current->lineNo,
           terminalStrings[current->token], current->lexeme);
    current = current->next;
  }
}

void freeTokenList(TokenInfo *head) {
  TokenInfo *temp;
  while (head != NULL) {
    temp = head;
    head = head->next;
    free(temp->lexeme);
    free(temp);
  }
}
