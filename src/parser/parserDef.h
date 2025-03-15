#ifndef PARSER_DEF
#define PARSER_DEF
#include "../lexer/lexer.h"

/* Node with terminal value */
typedef struct terminalNode {
  terminals t;
  struct terminalNode *next;
} terminalNode;

/* List of terminal nodes */
typedef struct terminalList {
  terminalNode *head;
} terminalList;

/* First and follow set for a given non-terminal */
typedef struct nonTerminalSets {
  terminalList *firstSet;
  terminalList *followSet;
} nonTerminalSets;

/* Value of token - terminal or non-terminal */
typedef union u {
  terminals t;        // Terminal value
  nonTerminals nonT;  // Non-terminal value
} value;

/* Parse tree node */
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

/* Node for a element in RHS of production rule */
typedef struct rhsNode {
  bool isT; 
  value v;
  struct rhsNode *next; 
} RHSNode;

/* Linked list of a production rule */
typedef struct productionRule {
  RHSNode *head;                   
  struct productionRule *nextRule; 
} ProductionRule;

/* Node for a element in LHS of production rule which starts with non-terminal lhs */
typedef struct lhsNode {
  ProductionRule *rules; 
  nonTerminals lhs;      
} LHSNode;

/* An item in a grammar rule - terminal or non-terminal */
typedef struct {
  bool isTer; 
  value v;
} gitems;

/* Grammar - list of all production rules */
typedef struct grammar {
  LHSNode *rules[nonTerminalCount];
} Grammar;

/* Parse Table */
typedef struct parseTable {
  ProductionRule *table[nonTerminalCount][terminalCount];
  bool isSyn[nonTerminalCount][terminalCount];
} ParseTable;

/* Data structure to compute follow sets */
typedef struct follow {
  RHSNode *occurrence;
  nonTerminals parentNt;
  struct follow *next; 
} followDS;

/* Node of parsing stack */
typedef struct StackNode {
  bool isT;
  value v;
  struct StackNode *next;
  struct treeNode *treeNode;
} StackNode;

/* Stack */
typedef struct Stack {
  int size;
  StackNode* head;
} Stack;
#endif
