#ifndef PARSER_DEF
#define PARSER_DEF
#include "../lexer/lexer.h"

// First follow sets
typedef struct terminalNode {
  terminals t;
  struct terminalNode *next;
} terminalNode;

typedef struct terminalList {
  terminalNode *head;
} terminalList;

typedef struct nonTerminalSets {
  terminalList *firstSet;
  terminalList *followSet;
} nonTerminalSets;

// Nodes of Grammar Non-Terminals
typedef union u {
  terminals t;        // Terminal value
  nonTerminals nonT; // Non-terminal value
} value;

// parse tree
/*typedef struct treeNode {*/
/*  struct treeNode *parent;*/
/*  struct treeNode *firstChild;*/
/*  struct treeNode *rightSibling;*/
/*  SymTableItem *tokenPtr; // Pointer to the token associated with the node*/
/*  bool isT;*/
/*  value v;*/
/*} treeNode;*/

typedef struct treeNode{
  struct treeNode *parent;
  struct treeNode *firstChild;
  struct treeNode* next;
  struct StackNode* stackNode;
  SymTableItem* SymTableItem;
  bool isT;
  value v;
  SymTableItem token;
} TreeNode;

typedef struct rhsNode {
  bool isT; // Indicates whether it's a terminal or non-terminal
  value v;
  struct rhsNode *next; // Pointer to the next node in the list
} RHSNode;

// A linked list containing a production rule
typedef struct productionRule {
  RHSNode *head;                     // Pointer to the head of the list
  struct productionRule *nextRule; // Pointer to the next production rule
} ProductionRule;

// Node for RHS of production rule
typedef struct lhsNode {
  ProductionRule *rules; // Pointer to linked list of production rules
  nonTerminals lhs;      // LHS non-terminal value
} LHSNode;

typedef struct grammar {
  LHSNode *rules[nonTerminalCount];
} Grammar;

typedef struct parseTable {
  ProductionRule *table[nonTerminalCount][terminalCount];
  bool isSyn[nonTerminalCount][terminalCount];
} ParseTable;

// stack for maintaining non-terminals
/*typedef struct stack {*/
/*  LHSNode *top;*/
/*  int size;*/
/*} helperStack;*/

// follow for non-terminals initialized
typedef struct follow {
  RHSNode *occurrence;
  nonTerminals parentNt;
  struct follow *next; // Pointer to the next follow struct
} followDS;

typedef struct {
  bool isTer; // Flag indicating if it's a terminal or not
  value v;
} gitems;

/*typedef struct StackNode {*/
/*  bool isT;*/
/*  value value;*/
/*  struct StackNode *next;*/
/*} StackNode;*/

typedef struct StackNode {
  bool isT;
  value v;
  struct StackNode *next;
  struct treeNode *treeNode;
} StackNode;

typedef struct Stack {
  int size;
  StackNode* head;
} Stack;
#endif
