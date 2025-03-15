#ifndef PARSER
#define PARSER
#include "parserDef.h"
gitems* createNonTerminal(nonTerminals nt);
gitems* createTerminal(terminals t);
void populateitems();
void addGrammarRules();
void createParseTable();
void initiateParseTable();
void initialiseMainStack();
void initialiseParseTreeRoot();
void printParseTable();
terminalList * computeFirstForRhsnode(RHSNode* rhs , terminalList* firstset);
void joinTerminallistExcEps(terminalList* list1, terminalList* list2);
void addTerminalTolist(terminalList *list , terminals t);
void joinTerminalList(terminalList * l1 , terminalList* l2);
void initialiseList(terminalList* list);
bool containsEpsilon(terminalList * givenList);
void removeEpsilon(terminalList *givenList);
void computeFirst(nonTerminals givenNt);
void computeFirsts();
void populateOccFollow();
void findFollowset(nonTerminals nt);
void joinTerminallistExcEps(terminalList* list1, terminalList* list2);
void computeFollow();
StackNode* top();
int push(int unionVal, bool isT);
int pop();
bool isEmpty();
TreeNode* pushListToStack(RHSNode* currNode, StackNode* parent, SymTableItem currToken);
void createParseTree(FILE *fp);
void printParseTree(FILE *output);
void dfsHelper(TreeNode* node, FILE* output);
void cleanUp();
void cleanUpDfs(TreeNode* node);
void cleanTreeNode(TreeNode* node);
void cleanMainStack();
void printParseTable();
void printStack();
void printProductionRule(int nonTerminalIdx, ProductionRule* prodRule);
void printFirstandFollowSets();
void printAllProductionRules(int nonTerminalIdx); 
#endif
