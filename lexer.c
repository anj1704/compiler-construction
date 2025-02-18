#include "lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// SHOULD WE RETRACT WHILE THROWING ERROR

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
#ifdef DEBUG
    printf("Lexeme %s already exists in the symbol table.\n", lexeme);
#endif
  }
}

// Function to lookup a lexeme in SymbolTable
SymTableItem *lookup(char *lexeme) {
  int index = hash(lexeme);

  // Traverse the linked list at index to find the lexeme
  SymTableItem *current = table->items[index];
  while (current != NULL) {
    if (strcmp(current->lexeme, lexeme) == 0) {
      // Lexeme found in the symbol table
      return current;
    }
    current = current->nextItem;
  }
  // Lexeme not found in the symbol table
  return NULL;
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
      (endPtr >= tBuff.bufferOne && endPtr <= tBuff.bufferOne + BUFFER_SIZE)) {

    length_lexeme = endPtr - startPtr;
  }

  else if ((startPtr >= tBuff.bufferTwo &&
            startPtr < tBuff.bufferTwo + BUFFER_SIZE) &&
           (endPtr >= tBuff.bufferTwo &&
            endPtr <= tBuff.bufferTwo + BUFFER_SIZE)) {

    length_lexeme = endPtr - startPtr;
  } else {
    length_lexeme =
        tBuff.bufferOne + BUFFER_SIZE - startPtr + endPtr - tBuff.bufferTwo;
  }

  lexeme = (char *)malloc((length_lexeme + 1) * sizeof(char));

  if (!lexeme) {
    fprintf(stderr, "Mem allocation failed");
    exit(EXIT_FAILURE);
  }

  if ((startPtr >= tBuff.bufferOne &&
       startPtr < tBuff.bufferOne + BUFFER_SIZE) &&
      (endPtr >= tBuff.bufferOne && endPtr <= tBuff.bufferOne + BUFFER_SIZE)) {

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
      size_t size = fread(tBuff.bufferTwo, sizeof(char), BUFFER_SIZE, fp);
      if (size < BUFFER_SIZE) {
        tBuff.bufferTwo[size] = EOF;
      }
    }
    loadBufferOne = true;
    endPtr = tBuff.bufferTwo;
    loadBufferTwo = false;
  } else if (endPtr == tBuff.bufferTwo + BUFFER_SIZE - 1) {
    if (loadBufferOne) {
      size_t size = fread(tBuff.bufferOne, sizeof(char), BUFFER_SIZE, fp);
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
  int dfastate = 1;
  SymTableItem newSymbolItem;
  char *lexeme;

  while (dfastate >= 1) {
#ifdef DEBUG
    printf("DEBUG: dfastate = %d, char = [%c] (ASCII: %d)\n", dfastate, ch,
           (int)ch);
#endif
    if (isEOF) {
      newSymbolItem.eof = true;
      newSymbolItem.lexeme = NULL;
      newSymbolItem.lineCount = lineCount;
      return newSymbolItem;
    }

    switch (dfastate) {
    case 1:
      switch (ch) {
      // delim
      case '\t':
        // startPtr = endPtr;
        // ch = getNextCharacter(fp);
        dfastate = 41;
        break;
      // delim
      case ' ':
        // startPtr = endPtr;
        // ch = getNextCharacter(fp);
        dfastate = 41;
        break;
      // newline
      case '\n':
        // startPtr = endPtr;
        // lineCount++;
        // ch = getNextCharacter(fp);
        dfastate = 43;
        break;
      // operators
      case '+':
        dfastate = 15;
        break;
      case '/':
        dfastate = 18;
        break;
      case '-':
        dfastate = 16;
        break;
      case '*':
        dfastate = 17;
        break;
      case ')':
        dfastate = 26;
        break;
      case ';':
        dfastate = 22;
        break;
      case ':':
        dfastate = 23;
        break;
      case '.':
        dfastate = 24;
        break;
      case '(':
        dfastate = 25;
        break;
      case ',':
        dfastate = 21;
        break;
      case '~':
        dfastate = 27;
        break;
      case '@':
        dfastate = 38;
        break;
      case '[':
        dfastate = 20;
        break;
      case ']':
        dfastate = 19;
        break;
      case '%':
        // startPtr = endPtr;
        dfastate = 63;
        break;
      case '_':
        dfastate = 45;
        break;
      case '#':
        dfastate = 49;
        break;
      case '&':
        dfastate = 35;
        break;
      case '=':
        dfastate = 28;
        break;
      case '<':
        dfastate = 2;
        break;
      case '>':
        dfastate = 32;
        break;
      case '!':
        dfastate = 30;
        break;
      default:
        if (ch >= 'b' && ch <= 'd') {
          dfastate = 11;
        }

        else if ((ch == 'a' || (ch >= 'e' && ch <= 'z'))) {
          dfastate = 9;
        }

        else if (ch >= '0' && ch <= '9') {
          dfastate = 52;
        } else {
          dfastate = -2;
          lexeme = getLexeme();
        }
      }
      break;

    case 2:
      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 3;
      } else if (ch == '=') {
        dfastate = 7;
      } else {
        dfastate = 8;
      }
      break;

    case 3:
      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 4;
      } else {
        dfastate = 6;
      }
      break;

    case 4:
      ch = getNextCharacter(fp);

      if (ch == '-') {
        dfastate = 5;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;

    case 5:
      return tokenize(getLexeme(), TK_ASSIGNOP, lineCount);
      break;

    case 6:
      // Double retraction
      endPtr--;
      endPtr--;
      return tokenize(getLexeme(), TK_LT, lineCount);
      break;

    case 7:
      return tokenize(getLexeme(), TK_LE, lineCount);
      break;

    case 8:
      // Single Retraction
      endPtr--;
      return tokenize(getLexeme(), TK_LT, lineCount);
      break;

    case 9:
      ch = getNextCharacter(fp);
      if (ch >= 'a' && ch <= 'z') {
        dfastate = 9;
      } else {
        dfastate = 10;
      }
      break;

    // CHECK THIS
    case 10:
      endPtr--;
      SymTableItem *t = lookup(getLexeme());
      if (t) {
        return tokenize(getLexeme(), t->token, lineCount);
      } else {
        return tokenize(getLexeme(), TK_FIELDID, lineCount);
      }
      break;
    case 11:
      ch = getNextCharacter(fp);
      if (ch >= 'a' && ch <= 'z') {
        dfastate = 9;
      } else if (ch >= '2' && ch <= '7') {
        dfastate = 12;
      } else {
        dfastate = 10;
      }
      break;

    case 12:
      ch = getNextCharacter(fp);
      if (ch >= 'b' && ch <= 'd') {
        dfastate = 12;
      } else if (ch >= '2' && ch <= '7') {
        dfastate = 13;
      } else {
        dfastate = 14;
      }
      break;

    case 13:
      ch = getNextCharacter(fp);
      if (ch >= '2' && ch <= '7') {
        dfastate = 13;
      } else {
        dfastate = 14;
      }
      break;

    // CHECK THIS
    case 14:
      endPtr--;
      lexeme = getLexeme();
#ifdef DEBUG
      printf("Lexeme extracted: %s\n", lexeme); // Debug print
      printf("Tokenized as TK_ID\n");
#endif
      insert(lexeme, TK_ID);
      return tokenize(lexeme, TK_ID, lineCount);
      break;

    case 15:
      return tokenize(getLexeme(), TK_PLUS, lineCount);
      break;

    case 16:
      return tokenize(getLexeme(), TK_MINUS, lineCount);
      break;

    case 17:
      return tokenize(getLexeme(), TK_MUL, lineCount);
      break;

    case 18:
      return tokenize(getLexeme(), TK_DIV, lineCount);
      break;

    case 19:
      return tokenize(getLexeme(), TK_SQR, lineCount);
      break;

    case 20:
      return tokenize(getLexeme(), TK_SQL, lineCount);
      break;

    case 21:
      return tokenize(getLexeme(), TK_COMMA, lineCount);
      break;

    case 22:
      return tokenize(getLexeme(), TK_SEM, lineCount);
      break;

    case 23:
      return tokenize(getLexeme(), TK_COLON, lineCount);
      break;

    case 24:
      return tokenize(getLexeme(), TK_DOT, lineCount);
      break;

    case 25:
      return tokenize(getLexeme(), TK_OP, lineCount);
      break;

    case 26:
      return tokenize(getLexeme(), TK_CL, lineCount);
      break;

    case 27:
      return tokenize(getLexeme(), TK_NOT, lineCount);
      break;

    case 28:
      ch = getNextCharacter(fp);
      if (ch == '=') {
        dfastate = 29;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;     // CHECK
      }
      break;

    case 29:
      return tokenize(getLexeme(), TK_EQ, lineCount);
      break;

    case 30:
      ch = getNextCharacter(fp);
      if (ch == '=') {
        dfastate = 31;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;     // CHECK
      }
      break;

    case 31:
      return tokenize(getLexeme(), TK_NE, lineCount);
      break;

    case 32:
      ch = getNextCharacter(fp);
      if (ch == '=') {
        dfastate = 33;
      } else {
        dfastate = 34;
      }
      break;

    case 33:
      return tokenize(getLexeme(), TK_GE, lineCount);
      break;

    case 34:
      endPtr--;
      return tokenize(getLexeme(), TK_GT, lineCount);
      break;

    case 35:
      ch = getNextCharacter(fp);
      if (ch == '&') {
        dfastate = 36;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;
      }
      break;

    case 36:
      ch = getNextCharacter(fp);
      if (ch == '&') {
        dfastate = 37;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;
      }
      break;

    case 37:
      return tokenize(getLexeme(), TK_AND, lineCount);
      break;

    case 38:
      ch = getNextCharacter(fp);
      if (ch == '@') {
        dfastate = 39;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;
      }
      break;

    case 39:
      ch = getNextCharacter(fp);
      if (ch == '@') {
        dfastate = 40;
      } else {
        dfastate = -5; // CHECK ERROR CODE
        endPtr--;
      }
      break;

    case 40:
      return tokenize(getLexeme(), TK_OR, lineCount);
      break;

    case 41:
      if (ch == '\t' || ch == ' ') {
        dfastate = 41;
        startPtr = endPtr;
        ch = getNextCharacter(fp);
      } else {
        dfastate = 42;
      }
      break;
    // CONTINUE DIRECTLY TAKES IT TO STATE 1 WITHOUT RETURNING ANYTHING?
    case 42:
      endPtr--;
      ch = getNextCharacter(fp);
      dfastate = 1;
      break;

    case 43:
      startPtr = endPtr;
      ch = getNextCharacter(fp);
      lineCount++;
      dfastate = 1;
      break;
    // WDYM ?
    case 44:
      exit(0);

    case 45:
      ch = getNextCharacter(fp);
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        dfastate = 46;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;

    case 46:
      ch = getNextCharacter(fp);
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        dfastate = 46;
      } else if (ch >= '0' && ch <= '9') {
        dfastate = 47;
      } else {
        dfastate = 48;
      }
      break;

    case 47:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 47;
      } else {
        dfastate = 48;
      }
      break;
    // CHECK THIS
    case 48:
      endPtr--;
      lexeme = getLexeme();
#ifdef DEBUG
      printf("Lexeme extracted: %s\n", lexeme); // Debug print
#endif

      if (strcmp(lexeme, "_main") == 0) {
#ifdef DEBUG
        printf("Tokenized as TK_MAIN\n");
#endif
        return tokenize(lexeme, TK_MAIN, lineCount);
      } else {
#ifdef DEBUG
        printf("Tokenized as TK_FUNID\n");
#endif
        insert(lexeme, TK_FUNID);
        return tokenize(lexeme, TK_FUNID, lineCount);
      }
      break;

    case 49:
      ch = getNextCharacter(fp);
      if (ch >= 'a' && ch <= 'z') {
        dfastate = 50;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;

    case 50:
      ch = getNextCharacter(fp);
      if (ch >= 'a' && ch <= 'z') {
        dfastate = 50;
      } else {
        dfastate = 51;
      }
      break;
    // TOKENINFO OR SYMBOL TABLE?
    case 51:
      endPtr--;
      lexeme = getLexeme();
#ifdef DEBUG
      printf("Lexeme extracted: %s\n", lexeme); // Debug print
      printf("Tokenized as TK_RUID\n");
#endif
      insert(lexeme, TK_RUID);
      return tokenize(lexeme, TK_RUID, lineCount);
      break;

    case 52:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 52;
      } else if (ch == '.') {
        dfastate = 53;
      } else {
        dfastate = 61;
      }
      break;

    case 53:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 54;
      } else {
        dfastate = 58;
      }
      break;

    case 54:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 55;
      } else {
        dfastate = -5;
        // Also retracting
        endPtr--;
      }
      break;

    case 55:
      ch = getNextCharacter(fp);
      if (ch == 'E') {
        dfastate = 56;
      } else {
        dfastate = 59;
      }
      break;

    case 56:
      ch = getNextCharacter(fp);
      if (ch == '+' || ch == '-') {
        dfastate = 57;
      } else if (ch >= '0' && ch <= '9') {
        dfastate = 60;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;

    case 57:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 60;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;
    // we haven't used a getval function as mentioned, so change implementation
    // of tokenize if needed
    case 58:
      endPtr -= 2;
      return tokenize(getLexeme(), TK_NUM, lineCount);

    case 59:
      endPtr--;
      return tokenize(getLexeme(), TK_RNUM, lineCount);

    case 60:
      ch = getNextCharacter(fp);
      if (ch >= '0' && ch <= '9') {
        dfastate = 62;
      } else {
        dfastate = -5;
        endPtr--; // CHECK
      }
      break;

    // we haven't used a getval function as mentioned, so change implementation
    // of tokenize if needed
    case 61:
      endPtr--;
      return tokenize(getLexeme(), TK_NUM, lineCount);

    case 62:
      return tokenize(getLexeme(), TK_RNUM, lineCount);

    // DO 63,64
    case 63:
      ch = getNextCharacter(fp);
      if (ch != '\n') {
        dfastate = 63;
      } else {
        dfastate = 64;
        lineCount++;
      }
      break;

    case 64:
      dfastate = 1;
      startPtr = endPtr;
      return tokenize("%", TK_COMMENT, lineCount - 1);
    }
  }

  if (dfastate < 0) {
    return error_helper(dfastate, lexeme, lineCount);
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

/*int main(void) {*/
/*  char *sourceFile = "./Lexer Test Cases/t1.txt";*/
/*  char *cleanFile = "./Lexer Test Cases/cleaned.txt";*/
/**/
/*  removeComments(sourceFile, cleanFile);*/
/**/
/*  FILE *fp = initialise(sourceFile, BUFFER_SIZE);*/
/*  if (!fp) {*/
/*    fprintf(stderr, "Failed to initialize lexer with file: %s\n", cleanFile);*/
/*    return 1;*/
/*  }*/
/**/
/*  SymTableItem currToken;*/
/*  int tokenCount = 0;*/
/*  lineCount = 1;*/
/**/
/*  while (!isEOF) {*/
/*    currToken = getToken(fp);*/
/**/
/*    if (currToken.lexeme != NULL) {*/
/*      printf("LineNo: %d , Token: %s, Lexeme: %s\n", currToken.lineCount,*/
/*             terminalStrings[currToken.token], currToken.lexeme);*/
/*      tokenCount++;*/
/*    }*/
/*  }*/
/**/
/*  printf("Total number of tokens: %d\n", tokenCount);*/
/**/
/*  fclose(fp);*/
/**/
/*  return 0;*/
/*}*/
