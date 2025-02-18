#ifndef PARSER_DEF
#define PARSER_DEF
#include "lexer.h"

// First follow sets
typedef struct terminal_node {
  terminals t;
  struct terminal_node *next;
} terminal_node;

typedef struct terminal_list {
  terminal_node *head;
} terminal_list;

typedef struct non_terminal_sets {
  terminal_list *first_set;
  terminal_list *follow_set;
} non_terminal_sets;

// Nodes of Grammar Non-Terminals
typedef union u {
  terminals t;        // Terminal value
  nonTerminals non_t; // Non-terminal value
} value;

// parse tree
typedef struct tree_node {
  struct tree_node *parent;
  struct tree_node *firstChild;
  struct tree_node *rightSibling;
  SymTableItem *token_ptr; // Pointer to the token associated with the node
  bool isT;
  value v;
} treeNode;

typedef struct rhs_node {
  bool isT; // Indicates whether it's a terminal or non-terminal
  value v;
  struct rhs_node *next; // Pointer to the next node in the list
  treeNode *ptr;
} RHSNode;

// A linked list containing a production rule
typedef struct production_rule {
  RHSNode *head;                     // Pointer to the head of the list
  struct production_rule *next_rule; // Pointer to the next production rule
} ProductionRule;

// Node for RHS of production rule
typedef struct lhs_node {
  ProductionRule *rules; // Pointer to linked list of production rules
  nonTerminals lhs;      // LHS non-terminal value
} LHSNode;

typedef struct grammar {
  LHSNode *rules[nonTerminalCount];
} Grammar;

typedef struct parse_table {
  ProductionRule *table[nonTerminalCount][terminalCount];
} ParseTable;

// stack for maintaining non-terminals
typedef struct stack {
  LHSNode *top;
  int size;
} helperStack;

// follow for non-terminals initialized
typedef struct follow {
  RHSNode *occurrence;
  nonTerminals parent_nt;
  struct follow *next; // Pointer to the next follow struct
} followDS;

typedef struct {
  bool isTer; // Flag indicating if it's a terminal or not
  value v;
} gitems;

typedef struct StackNode {
  bool isT;
  value value;
  struct StackNode *next;
} StackNode;

#endif
