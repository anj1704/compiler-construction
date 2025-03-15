#ifndef PARSER
#define PARSER

#include "parserDef.h"

/* ----------------- Memory Allocation Functions ----------------- */

/*
 * Function: createNonTerminal
 * ---------------------------
 * Creates a new non-terminal grammar item.
 *
 * Input:
 * - nt: The non-terminal identifier.
 *
 * Output:
 * - Returns a pointer to a newly allocated non-terminal item.
 */
gitems* createNonTerminal(nonTerminals nt);

/*
 * Function: createTerminal
 * -------------------------
 * Creates a new terminal grammar item.
 *
 * Input:
 * - t: The terminal identifier.
 *
 * Output:
 * - Returns a pointer to a newly allocated terminal item.
 */
gitems* createTerminal(terminals t);

/*
 * Function: createLHSNode
 * ------------------------
 * Creates an LHS node for grammar rules.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated LHS node.
 */
LHSNode* createLHSNode();

/*
 * Function: createRHSNode
 * ------------------------
 * Creates an RHS node for grammar rules.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated RHS node.
 */
RHSNode* createRHSNode();

/*
 * Function: createProductionRule
 * ------------------------------
 * Creates a new production rule.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated production rule.
 */
ProductionRule* createProductionRule();

/*
 * Function: createTerminalList
 * ----------------------------
 * Creates a new list for storing terminals.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated terminal list.
 */
terminalList* createTerminalList();

/*
 * Function: createTerminalNode
 * ----------------------------
 * Creates a new node for storing a terminal in a list.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated terminal node.
 */
terminalNode* createTerminalNode();

/*
 * Function: createFollowDS
 * -------------------------
 * Creates a new follow set data structure.
 *
 * Input: None
 *
 * Output:
 * - Returns a pointer to a newly allocated follow set structure.
 */
followDS* createFollowDS();

/* ----------------- Grammar Construction Functions ----------------- */

/*
 * Function: addGrammarRules
 * -------------------------
 * Populates the grammar with production rules.
 *
 * Input: None
 *
 * Output: None
 */
void addGrammarRules();

/*
 * Function: addrule
 * -----------------
 * Adds a new rule to the grammar.
 *
 * Input:
 * - nt: The LHS non-terminal.
 * - size: The number of items in the RHS.
 * - value: Array of grammar items for the RHS.
 *
 * Output: None
 */
void addrule(nonTerminals nt, int size, gitems value[]);

/* ----------------- First & Follow Set Computation ----------------- */

/*
 * Function: computeFirsts
 * ------------------------
 * Computes the FIRST sets for all non-terminals.
 *
 * Input: None
 *
 * Output: None
 */
void computeFirsts();

/*
 * Function: computeFollow
 * ------------------------
 * Computes the FOLLOW sets for all non-terminals.
 *
 * Input: None
 *
 * Output: None
 */
void computeFollow();

/*
 * Function: findAllOccurences
 * ----------------------------
 * Populates the follow occurrence data structure.
 *
 * Input: None
 *
 * Output: None
 */
void findAllOccurences();

/*
 * Function: findFollowset
 * ------------------------
 * Computes the FOLLOW set for a given non-terminal.
 *
 * Input:
 * - nt: The non-terminal whose FOLLOW set is to be computed.
 *
 * Output: None
 */
void findFollowset(nonTerminals nt);

/* ----------------- Parse Table Functions ----------------- */

/*
 * Function: createParseTable
 * ---------------------------
 * Allocates memory for and initializes the parse table.
 * It sets up default values and marks synchronization entries.
 *
 * Input: None
 *
 * Output: None
 */
void createParseTable();
/*
 * Function: initiateParseTable
 * ----------------------------
 * Populates the parse table using computed FIRST and FOLLOW sets.
 * It assigns production rules to corresponding entries in the table.
 *
 * Input: None
 *
 * Output: None
 */
void initiateParseTable();

/* ----------------- Parser Stack Management ----------------- */

/*
 * Function: initialiseMainStack
 * -----------------------------
 * Initializes the parsing stack by allocating memory and setting initial values.
 *
 * Input: None
 *
 * Output: None
 */
void initialiseMainStack();
/*
 * Function: cleanMainStack
 * ------------------------
 * Frees all memory associated with the parsing stack and resets it.
 *
 * Input: None
 *
 * Output: None
 */
void cleanMainStack();
/*
 * Function: push
 * --------------
 * Pushes a terminal or non-terminal onto the stack.
 *
 * Input:
 * - unionVal: The value of the terminal or non-terminal.
 * - isT: Boolean flag indicating whether it is a terminal.
 *
 * Output:
 * - Returns 1 on success, 0 on failure.
 */
int push(int unionVal, bool isT);
/*
 * Function: pop
 * -------------
 * Removes the top item from the stack.
 *
 * Input: None
 *
 * Output:
 * - Returns 1 if successful, 0 if the stack is empty.
 */
int pop();
/*
 * Function: isEmpty
 * -----------------
 * Checks if the parsing stack is empty.
 *
 * Input: None
 *
 * Output:
 * - Returns true if the stack is empty, false otherwise.
 */
bool isEmpty();

/* ----------------- Parse Tree Construction ----------------- */

/*
 * Function: createParseTree
 * --------------------------
 * Parses the input file and constructs a parse tree using a stack-based LL(1) parsing approach.
 *
 * Input:
 * - fp: FILE pointer to the source code.
 *
 * Output: None
 */
void createParseTree(FILE *fp);
/*
 * Function: printParseTree
 * -------------------------
 * Prints the parse tree in a formatted manner to the given output file.
 *
 * Input:
 * - output: FILE pointer to the output file.
 *
 * Output: None
 */
void printParseTree(FILE *output);
/*
 * Function: cleanTreeNode
 * ------------------------
 * Recursively frees all memory associated with a parse tree node and its children.
 *
 * Input:
 * - node: Pointer to the tree node to be freed.
 *
 * Output: None
 */
void cleanTreeNode(TreeNode* node);

/*
 * Function: pushListToStack
 * --------------------------
 * Pushes a sequence of grammar symbols (RHS of a production rule) onto the parsing stack
 * and creates corresponding tree nodes in the parse tree.
 *
 * Input:
 * - currNode: Pointer to the RHS node of the production rule.
 * - parent: Pointer to the stack node representing the parent non-terminal.
 * - currToken: The current token being processed.
 *
 * Output:
 * - Returns a pointer to the first child tree node created.
 */
TreeNode* pushListToStack(RHSNode* currNode, StackNode* parent, SymTableItem currToken);

/* ----------------- Utility Functions ----------------- */

/*
 * Function: addTerminalTolist
 * ---------------------------
 * Adds a terminal to a terminal list.
 *
 * Input:
 * - list: The terminal list.
 * - t: The terminal to be added.
 *
 * Output: None
 */
void addTerminalTolist(terminalList *list , terminals t);

/*
 * Function: joinTerminalList
 * ---------------------------
 * Merges two terminal lists.
 *
 * Input:
 * - l1: The first terminal list.
 * - l2: The second terminal list.
 * - includeEps: Whether to include EPS in the merge.
 *
 * Output: None
 */
void joinTerminalList(terminalList * l1 , terminalList* l2, bool includeEps);

/*
 * Function: containsEpsilon
 * ---------------------------
 * Checks if a terminal list contains EPS.
 *
 * Input:
 * - givenList: The terminal list to check.
 *
 * Output:
 * - Returns true if EPS is found, false otherwise.
 */
bool containsEpsilon(terminalList * givenList);

/*
 * Function: removeEpsilon
 * ------------------------
 * Removes EPS from a terminal list if present.
 *
 * Input:
 * - givenList: The terminal list.
 *
 * Output: None
 */
void removeEpsilon(terminalList *givenList);

/* ----------------- Debugging Functions ----------------- */

/*
 * Function: printParseTable
 * -------------------------
 * Prints the parse table in a tabular format, displaying the mapping of non-terminals
 * to their corresponding production rules for various terminals.
 *
 * Input: None
 *
 * Output: None
 */
void printParseTable();
/*
 * Function: printFirstandFollowSets
 * ---------------------------------
 * Prints the computed FIRST and FOLLOW sets for all non-terminals.
 *
 * Input: None
 *
 * Output: None
 */
void printFirstandFollowSets();

#endif
