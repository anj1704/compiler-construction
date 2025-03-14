#ifndef PARSER
#define PARSER
#include "parserDef.h"
gitems* createNonTerminal(nonTerminals nt);
gitems* createTerminal(terminals t);
void populateitems();
void addGrammarRules();
void create_parse_table();
void initiate_parse_table();
void print_parse_table();
terminal_list * compute_first_for_rhsnode(RHSNode* rhs , terminal_list* firstset);
void join_terminallist_exc_eps(terminal_list* list1, terminal_list* list2);
void add_terminal_tolist(terminal_list *list , terminals t);
void join_terminal_list(terminal_list * l1 , terminal_list* l2);
void initialise_list(terminal_list* list);
bool contains_epsilon(terminal_list * given_list);
void remove_epsilon(terminal_list *given_list);
void compute_first(nonTerminals given_nt);
void compute_firsts();
void populate_occ_follow();
void find_followset(nonTerminals nt);
void join_terminallist_exc_eps(terminal_list* list1, terminal_list* list2);
void compute_follow();
StackNode* top();
int push(int union_val, bool isT);
int pop();
bool isEmpty();
TreeNode* pushListToStack(RHSNode* currNode, StackNode* parent, SymTableItem currToken);
void createParseTree(FILE *fp);
void printParseTree(FILE *output);
void dfsHelper(TreeNode* node, FILE* output);
void cleanUp();
void cleanUpDfs(TreeNode* node);
void print_parse_table();
void printStack();
void printProductionRule(int nonTerminalIdx, ProductionRule* prodRule);
void printFirstandFollowSets();
void printAllProductionRules(int nonTerminalIdx); 
#endif
