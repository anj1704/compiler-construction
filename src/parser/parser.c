#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
#include <stdlib.h>

extern char *terminalStrings[];
extern char *nonTerminalStrings[];
extern followDS *followOccurrence[nonTerminalCount];
extern Grammar *G;
extern nonTerminalSets firstFollowSets[nonTerminalCount];
extern ParseTable *PT;
extern gitems **itemList;
extern Stack* mainStack;
extern TreeNode* parseTreeRoot;
extern int lineCount;

gitems *createNonTerminal(nonTerminals nt) {
  gitems *item = (gitems *)malloc(sizeof(gitems));
  if (!item) {
    printf("Failed to allocate memory for Non Terminal\n");
  } else {
    item->isTer = false;
    item->v.nonT = nt;
  }
  return item;
}
gitems *createTerminal(terminals t) {
  gitems *item = (gitems *)malloc(sizeof(gitems));
  if (!item) {
    printf("Failed to allocate memory for Terminal\n");
  } else {
    item->isTer = true;
    item->v.t = t;
  }
  return item;
}

LHSNode *createLHSNode() {
  LHSNode *lhsNode = (LHSNode *)malloc(sizeof(LHSNode));
  if (!lhsNode)
    printf("Failed to allocate memory for LHS Node\n");
  else
    lhsNode->rules = NULL;
  return lhsNode;
}

RHSNode *createRHSNode() {
  RHSNode *rhsNode = (RHSNode *)malloc(sizeof(RHSNode));
  if (!rhsNode)
    printf("Failed to allocate memory for RHS Node\n");
  else {
    rhsNode->next = NULL;
  }
  return rhsNode;
}

ProductionRule *createProductionRule() {
  ProductionRule *rule = (ProductionRule *)malloc(sizeof(ProductionRule));
  if (!rule)
    printf("Failed to allocate memory for ProductionRule\n");
  else {
    rule->nextRule = NULL;
    rule->head = NULL;
  }
  return rule;
}

terminalList *createTerminalList() {
  terminalList *list = (terminalList *)malloc(sizeof(terminalList));
  if (!list)
    printf("Failed to allocate memory for Terminal List\n");
  else
    list->head = NULL;
  return list;
}

terminalNode *createTerminalNode() {
  terminalNode *node = (terminalNode *)malloc(sizeof(terminalNode));
  if (!node)
    printf("Failed to allocate memory for Terminal Node\n");
  else
    node->next = NULL;
  return node;
}

followDS *createFollowDS() {
  followDS *fol = (followDS *)malloc(sizeof(followDS));
  if (!fol)
    printf("Failed to allocate memory for Follow DS\n");
  else {
    fol->next = NULL;
    fol->occurrence = NULL;
  }
  return fol;
}

void initialiseMainStack() {
  if (mainStack) {
    cleanMainStack();
  }

  mainStack = (Stack*)malloc(sizeof(Stack));
  mainStack->size = 0;
  mainStack->head = NULL;
}

void cleanMainStack() {
  StackNode *curr = mainStack->head;
  while (curr) {
    StackNode *temp = curr->next;
    free(curr);
    curr = temp;
  }
  free(mainStack);
  mainStack = NULL;
}

void initialiseParseTreeRoot() {
  if (parseTreeRoot != NULL) {
    cleanTreeNode(parseTreeRoot);
  }

  parseTreeRoot = (TreeNode*)malloc(sizeof(TreeNode));
}

void cleanTreeNode(TreeNode *node) {
  if (node == NULL)
    return;
  /*printf("%s started\n", (node->isT) ? terminalStrings[node->v.t] : nonTerminalStrings[node->v.nonT]);*/

  if (node->firstChild != NULL)
    cleanTreeNode(node->firstChild);

  if (node->next != NULL)
    cleanTreeNode(node->next);

  if (node->stackNode != NULL)
    free(node->stackNode);

  if (node->SymTableItem != NULL)
    free(node->SymTableItem);

  /*printf("%s ended\n", (node->isT) ? terminalStrings[node->v.t] : nonTerminalStrings[node->v.nonT]);*/

  if (node != NULL)
    free(node);
  /*printf("%s 3\n", (node->isT) ? terminalStrings[node->v.t] : nonTerminalStrings[node->v.nonT]);*/
  node = NULL;
}

void addrule(nonTerminals nt, int size, gitems value[]) {
  LHSNode *lhsNode;
  if (G->rules[nt] == NULL) {
    lhsNode = createLHSNode();
    lhsNode->lhs = nt;
  } else {
    lhsNode = G->rules[nt];
  }
  ProductionRule *currentrulehead = lhsNode->rules;
  ProductionRule *newRule = createProductionRule();
  RHSNode *rhshead = createRHSNode();
  RHSNode *rhstail = rhshead;
  rhshead->v = value[0].v;
  rhshead->isT = value[0].isTer;
  for (int i = 1; i < size; i++) {
    RHSNode *rhsNode = createRHSNode();
    rhsNode->v = value[i].v;
    rhsNode->isT = value[i].isTer;
    rhstail->next = rhsNode;
    rhstail = rhstail->next;
  }
  newRule->head = rhshead;
  if (currentrulehead == NULL) {
    currentrulehead = newRule;
    lhsNode->rules = newRule;
    G->rules[nt] = lhsNode;
    return;
  }
  if (currentrulehead->nextRule != NULL) {
    while (currentrulehead->nextRule != NULL)
      currentrulehead = currentrulehead->nextRule;
  }
  currentrulehead->nextRule = newRule;

  // Add the LHSNode to the grammar
  G->rules[nt] = lhsNode;
}

void addGrammarRules() {
  gitems i1[2] = {{false, {.nonT = otherFunctions}},
                  {false, {.nonT = mainFunction}}};
  addrule(program, 2, i1);

  gitems i2[3] = {
      {true, {TK_MAIN}}, {false, {.nonT = stmts}}, {true, {TK_END}}};
  addrule(mainFunction, 3, i2);

  gitems i3a[2] = {{false, {.nonT = function}},
                   {false, {.nonT = otherFunctions}}};
  addrule(otherFunctions, 2, i3a);

  gitems i3b[1] = {{true, {EPS}}};
  addrule(otherFunctions, 1, i3b);

  gitems i4[6] = {{true, {TK_FUNID}},
                  {false, {.nonT = inputPar}},
                  {false, {.nonT = outputPar}},
                  {true, {TK_SEM}},
                  {false, {.nonT = stmts}},
                  {true, {TK_END}}};
  addrule(function, 6, i4);

  gitems i5[6] = {{true, {TK_INPUT}},
                  {true, {TK_PARAMETER}},
                  {true, {TK_LIST}},
                  {true, {TK_SQL}},
                  {false, {.nonT = parameterList}},
                  {true, {TK_SQR}}};
  addrule(inputPar, 6, i5);

  gitems i6a[6] = {{true, {TK_OUTPUT}},
                   {true, {TK_PARAMETER}},
                   {true, {TK_LIST}},
                   {true, {TK_SQL}},
                   {false, {.nonT = parameterList}},
                   {true, {TK_SQR}}};
  addrule(outputPar, 6, i6a);

  gitems i6b[1] = {{true, {EPS}}};
  addrule(outputPar, 1, i6b);

  gitems i7[3] = {{false, {.nonT = dataType}},
                  {true, {TK_ID}},
                  {false, {.nonT = remainingList}}};
  addrule(parameterList, 3, i7);

  gitems i8a[1] = {{false, {.nonT = primitiveDatatype}}};
  addrule(dataType, 1, i8a);

  gitems i8b[1] = {{false, {.nonT = constructedDatatype}}};
  addrule(dataType, 1, i8b);

  gitems i9a[1] = {{true, {TK_INT}}};
  addrule(primitiveDatatype, 1, i9a);

  gitems i9b[1] = {{true, {TK_REAL}}};
  addrule(primitiveDatatype, 1, i9b);

  gitems i10a[2] = {{true, {TK_RECORD}}, {true, {TK_RUID}}};
  addrule(constructedDatatype, 2, i10a);

  gitems i10b[2] = {{true, {TK_UNION}}, {true, {TK_RUID}}};
  addrule(constructedDatatype, 2, i10b);

  gitems i10c[1] = {{true, {TK_RUID}}};
  addrule(constructedDatatype, 1, i10c);

  gitems i11a[2] = {{true, {TK_COMMA}}, {false, {.nonT = parameterList}}};
  addrule(remainingList, 2, i11a);

  gitems i11b[1] = {{true, {EPS}}};
  addrule(remainingList, 1, i11b);

  gitems i12[4] = {{false, {.nonT = typeDefinitions}},
                   {false, {.nonT = declarations}},
                   {false, {.nonT = otherStmts}},
                   {false, {.nonT = returnStmt}}};
  addrule(stmts, 4, i12);

  gitems i13a1[2] = {{false, {.nonT = actualOrRedefined}},
                     {false, {.nonT = typeDefinitions}}};
  addrule(typeDefinitions, 2, i13a1);

  gitems i13a2[1] = {{true, {EPS}}};
  addrule(typeDefinitions, 1, i13a2);

  gitems i13b1[1] = {{false, {.nonT = typeDefinition}}};
  addrule(actualOrRedefined, 1, i13b1);

  gitems i13b2[1] = {{false, {.nonT = definetypestmt}}};
  addrule(actualOrRedefined, 1, i13b2);

  gitems i14[4] = {{true, {TK_RECORD}},
                   {true, {TK_RUID}},
                   {false, {.nonT = fieldDefinitions}},
                   {true, {TK_ENDRECORD}}};
  addrule(typeDefinition, 4, i14);

  gitems i15[4] = {{true, {TK_UNION}},
                   {true, {TK_RUID}},
                   {false, {.nonT = fieldDefinitions}},
                   {true, {TK_ENDUNION}}};
  addrule(typeDefinition, 4, i15);

  gitems i16[3] = {{false, {.nonT = fieldDefinition}},
                   {false, {.nonT = fieldDefinition}},
                   {false, {.nonT = moreFields}}};
  addrule(fieldDefinitions, 3, i16);

  gitems i17a[5] = {{true, {TK_TYPE}},
                    {false, {.nonT = fieldtype}},
                    {true, {TK_COLON}},
                    {true, {TK_FIELDID}},
                    {true, {TK_SEM}}};
  addrule(fieldDefinition, 5, i17a);

  gitems i17b[1] = {{false, {.nonT = primitiveDatatype}}};
  addrule(fieldtype, 1, i17b);

  gitems i17c[1] = {{false, {.nonT = constructedDatatype}}};
  addrule(fieldtype, 1, i17c);

  gitems i18a[2] = {{false, {.nonT = fieldDefinition}},
                    {false, {.nonT = moreFields}}};
  addrule(moreFields, 2, i18a);

  gitems i18b[1] = {{true, {EPS}}};
  addrule(moreFields, 1, i18b);

  gitems i19a[2] = {{false, {.nonT = declaration}},
                    {false, {.nonT = declarations}}};
  addrule(declarations, 2, i19a);

  gitems i19b[1] = {{true, {EPS}}};
  addrule(declarations, 1, i19b);

  gitems i20[6] = {
      {true, {TK_TYPE}}, {false, {.nonT = dataType}},      {true, {TK_COLON}},
      {true, {TK_ID}},   {false, {.nonT = globalOrNot}}, {true, {TK_SEM}}};
  addrule(declaration, 6, i20);

  gitems i21a[2] = {{true, {TK_COLON}}, {true, {TK_GLOBAL}}};
  addrule(globalOrNot, 2, i21a);

  gitems i21b[1] = {{true, {EPS}}};
  addrule(globalOrNot, 1, i21b);

  gitems i22a[2] = {{false, {.nonT = stmt}}, {false, {.nonT = otherStmts}}};
  addrule(otherStmts, 2, i22a);

  gitems i22b[1] = {{true, {EPS}}};
  addrule(otherStmts, 1, i22b);

  gitems i23a[1] = {{false, {.nonT = assignmentStmt}}};
  addrule(stmt, 1, i23a);

  gitems i23b[1] = {{false, {.nonT = iterativeStmt}}};
  addrule(stmt, 1, i23b);

  gitems i23c[1] = {{false, {.nonT = conditionalStmt}}};
  addrule(stmt, 1, i23c);

  gitems i23d[1] = {{false, {.nonT = ioStmt}}};
  addrule(stmt, 1, i23d);

  gitems i23e[1] = {{false, {.nonT = funCallStmt}}};
  addrule(stmt, 1, i23e);

  gitems i24[4] = {{false, {.nonT = singleOrRecId}},
                   {true, {TK_ASSIGNOP}},
                   {false, {.nonT = arithmeticExpression}},
                   {true, {TK_SEM}}};
  addrule(assignmentStmt, 4, i24);

  gitems i25a[2] = {{true, {TK_ID}},
                    {false, {.nonT = optionSingleConstructed}}};
  addrule(singleOrRecId, 2, i25a);

  gitems i25b[2] = {{false, {.nonT = oneExpansion}},
                    {false, {.nonT = moreExpansions}}};
  addrule(optionSingleConstructed, 2, i25b);

  gitems i25p[1] = {{true, {EPS}}};
  addrule(optionSingleConstructed, 1, i25p);

  gitems i25c[2] = {{true, {TK_DOT}}, {true, {TK_FIELDID}}};
  addrule(oneExpansion, 2, i25c);

  gitems i25d[2] = {{false, {.nonT = oneExpansion}},
                    {false, {.nonT = moreExpansions}}};
  addrule(moreExpansions, 2, i25d);

  gitems i25e[1] = {{true, {EPS}}};
  addrule(moreExpansions, 1, i25e);

  gitems i26[7] = {{false, {.nonT = outputParameters}},
                   {true, {TK_CALL}},
                   {true, {TK_FUNID}},
                   {true, {TK_WITH}},
                   {true, {TK_PARAMETERS}},
                   {false, {.nonT = inputParameters}},
                   {true, {TK_SEM}}};
  addrule(funCallStmt, 7, i26);

  gitems i27a[4] = {{true, {TK_SQL}},
                    {false, {.nonT = idList}},
                    {true, {TK_SQR}},
                    {true, {TK_ASSIGNOP}}};
  addrule(outputParameters, 4, i27a);

  gitems i27b[1] = {{true, {EPS}}};
  addrule(outputParameters, 1, i27b);

  gitems i28[3] = {
      {true, {TK_SQL}}, {false, {.nonT = idList}}, {true, {TK_SQR}}};
  addrule(inputParameters, 3, i28);

  gitems i29[7] = {{true, {TK_WHILE}},
                   {true, {TK_OP}},
                   {false, {.nonT = booleanExpression}},
                   {true, {TK_CL}},
                   {false, {.nonT = stmt}},
                   {false, {.nonT = otherStmts}},
                   {true, {TK_ENDWHILE}}};
  addrule(iterativeStmt, 7, i29);

  gitems i30[8] = {{true, {TK_IF}},
                   {true, {TK_OP}},
                   {false, {.nonT = booleanExpression}},
                   {true, {TK_CL}},
                   {true, {TK_THEN}},
                   {false, {.nonT = stmt}},
                   {false, {.nonT = otherStmts}},
                   {false, {.nonT = elsePart}}};
  addrule(conditionalStmt, 8, i30);

  gitems i31a[4] = {{true, {TK_ELSE}},
                    {false, {.nonT = stmt}},
                    {false, {.nonT = otherStmts}},
                    {true, {TK_ENDIF}}};
  addrule(elsePart, 4, i31a);

  gitems i31b[1] = {{true, {TK_ENDIF}}};
  addrule(elsePart, 1, i31b);

  gitems i32a[5] = {{true, {TK_READ}},
                    {true, {TK_OP}},
                    {false, {.nonT = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(ioStmt, 5, i32a);

  gitems i32b[5] = {{true, {TK_WRITE}},
                    {true, {TK_OP}},
                    {false, {.nonT = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(ioStmt, 5, i32b);

  gitems i33a[2] = {{false, {.nonT = term}}, {false, {.nonT = expPrime}}};
  addrule(arithmeticExpression, 2, i33a);

  gitems i33b1[3] = {{false, {.nonT = lowPrecedenceOperators}},
                     {false, {.nonT = term}},
                     {false, {.nonT = expPrime}}};
  addrule(expPrime, 3, i33b1);

  gitems i33b2[1] = {{true, {EPS}}};
  addrule(expPrime, 1, i33b2);

  gitems i33c[2] = {{false, {.nonT = factor}}, {false, {.nonT = termPrime}}};
  addrule(term, 2, i33c);

  gitems i34a[3] = {{false, {.nonT = highPrecedenceOperators}},
                    {false, {.nonT = factor}},
                    {false, {.nonT = termPrime}}};
  addrule(termPrime, 3, i34a);

  gitems i34b[1] = {{true, {EPS}}};
  addrule(termPrime, 1, i34b);

  gitems i35a1[3] = {{true, {TK_OP}},
                     {false, {.nonT = arithmeticExpression}},
                     {true, {TK_CL}}};
  addrule(factor, 3, i35a1);

  gitems i35a2[1] = {{false, {.nonT = var}}};
  addrule(factor, 1, i35a2);

  gitems i35b1[1] = {{true, {TK_MUL}}};
  addrule(highPrecedenceOperators, 1, i35b1);

  gitems i35b2[1] = {{true, {TK_DIV}}};
  addrule(highPrecedenceOperators, 1, i35b2);

  gitems i35c1[1] = {{true, {TK_PLUS}}};
  addrule(lowPrecedenceOperators, 1, i35c1);

  gitems i35c2[1] = {{true, {TK_MINUS}}};
  addrule(lowPrecedenceOperators, 1, i35c2);

  gitems i36[7] = {{true, {TK_OP}}, {false, {.nonT = booleanExpression}},
                   {true, {TK_CL}}, {false, {.nonT = logicalOp}},
                   {true, {TK_OP}}, {false, {.nonT = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(booleanExpression, 7, i36);

  gitems i37[3] = {{false, {.nonT = var}},
                   {false, {.nonT = relationalOp}},
                   {false, {.nonT = var}}};
  addrule(booleanExpression, 3, i37);

  gitems i38[4] = {{true, {TK_NOT}},
                   {true, {TK_OP}},
                   {false, {.nonT = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(booleanExpression, 4, i38);

  gitems i39a[1] = {{false, {.nonT = singleOrRecId}}};
  addrule(var, 1, i39a);

  gitems i39b[1] = {{true, {TK_NUM}}};
  addrule(var, 1, i39b);

  gitems i39c[1] = {{true, {TK_RNUM}}};
  addrule(var, 1, i39c);

  gitems i40a[1] = {{true, {TK_AND}}};
  addrule(logicalOp, 1, i40a);

  gitems i40b[1] = {{true, {TK_OR}}};
  addrule(logicalOp, 1, i40b);

  gitems i41a[1] = {{true, {TK_LT}}};
  addrule(relationalOp, 1, i41a);

  gitems i41b[1] = {{true, {TK_LE}}};
  addrule(relationalOp, 1, i41b);

  gitems i41c[1] = {{true, {TK_EQ}}};
  addrule(relationalOp, 1, i41c);

  gitems i41d[1] = {{true, {TK_GT}}};
  addrule(relationalOp, 1, i41d);

  gitems i41e[1] = {{true, {TK_GE}}};
  addrule(relationalOp, 1, i41e);

  gitems i41f[1] = {{true, {TK_NE}}};
  addrule(relationalOp, 1, i41f);

  gitems i42[3] = {{true, {TK_RETURN}},
                   {false, {.nonT = optionalReturn}},
                   {true, {TK_SEM}}};
  addrule(returnStmt, 3, i42);

  gitems i43a[3] = {
      {true, {TK_SQL}}, {false, {.nonT = idList}}, {true, {TK_SQR}}};
  addrule(optionalReturn, 3, i43a);

  gitems i43b[1] = {{true, {EPS}}};
  addrule(optionalReturn, 1, i43b);

  gitems i44[2] = {{true, {TK_ID}}, {false, {.nonT = moreIds}}};
  addrule(idList, 2, i44);

  gitems i45a[2] = {{true, {TK_COMMA}}, {false, {.nonT = idList}}};
  addrule(moreIds, 2, i45a);

  gitems i45b[1] = {{true, {EPS}}};
  addrule(moreIds, 1, i45b);

  gitems i46[5] = {{true, {TK_DEFINETYPE}},
                   {false, {.nonT = A}},
                   {true, {TK_RUID}},
                   {true, {TK_AS}},
                   {true, {TK_RUID}}};
  addrule(definetypestmt, 5, i46);

  gitems i47a[1] = {{true, {TK_RECORD}}};
  addrule(A, 1, i47a);

  gitems i47b[1] = {{true, {TK_UNION}}};
  addrule(A, 1, i47b);
}

void computeFirst(nonTerminals givenNt) {
  if (firstFollowSets[givenNt].firstSet)
    return;

#ifdef DEBUG
  printf("computeFirst start: %s\n", nonTerminalStrings[givenNt]);
#endif
  firstFollowSets[givenNt].firstSet = createTerminalList();

  LHSNode *lhs = G->rules[givenNt];
  for (ProductionRule *rule = lhs->rules; rule != NULL;
       rule = rule->nextRule) {
    for (RHSNode *rhs = rule->head; rhs != NULL; rhs = rhs->next) {
      if (rhs->isT) {
#ifdef DEBUG
        printf("terminal: %s\n", terminalStrings[rhs->v.t]);
#endif
        addTerminalTolist(firstFollowSets[givenNt].firstSet, rhs->v.t);
        break;
      } else {
#ifdef DEBUG
        printf("non terminal: %s\n", nonTerminalStrings[rhs->v.nonT]);
#endif
        computeFirst(rhs->v.nonT);
        joinTerminalList(firstFollowSets[givenNt].firstSet,
                           firstFollowSets[rhs->v.nonT].firstSet);

        // Made change to check if all non-terminals contain an EChanged
        if (containsEpsilon(firstFollowSets[rhs->v.nonT].firstSet) && (rhs->next != NULL)) {
          removeEpsilon(firstFollowSets[givenNt].firstSet);
        } else
          break;
      }
    }
  }
#ifdef DEBUG
  printf("computeFirst done: %s\n", nonTerminalStrings[givenNt]);
#endif
}

void computeFirsts() {
  for (int i = 0; i < nonTerminalCount; i++) {
    computeFirst(i);
  }
}

void addTerminalTolist(terminalList *list, terminals t) {

  if (list->head == NULL) {
    list->head = createTerminalNode();
    list->head->t = t;
  } else {
    terminalNode *cur = list->head;
    terminalNode *prev = NULL;

    while (cur != NULL) {
      if (cur->t == t) {
        return;
      }

      prev = cur;
      cur = cur->next;
    }

    terminalNode *newnode = createTerminalNode();
    newnode->t = t;
    newnode->next = NULL;

    if (prev != NULL) {
      prev->next = newnode;
    }
  }
}

void joinTerminalList(terminalList *l1, terminalList *l2) {
  if (l1 == NULL || l2 == NULL || l2->head == NULL) {
    return;
  }

  terminalNode *cur = l2->head;

  while (cur != NULL) {
    terminalNode *search = l1->head;
    bool found = false;

    while (search != NULL) {
      if (search->t == cur->t) {
        found = true;
        break;
      }
      search = search->next;
    }

    if (!found) {
      terminalNode *newnode = createTerminalNode();

      if (newnode == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
      }

      newnode->t = cur->t;
      newnode->next = NULL;

      if (l1->head == NULL) {
        l1->head = newnode;
      } else {
        terminalNode *last = l1->head;
        while (last->next != NULL) {
          last = last->next;
        }
        last->next = newnode;
      }
    }

    cur = cur->next;
  }
}

bool containsEpsilon(terminalList *givenList) {
  terminalNode *temp = givenList->head;
  while (temp != NULL) {
    if (temp->t == EPS) {
      return true;
    }
    temp = temp->next;
  }
  return false;
}

void removeEpsilon(terminalList *givenList) {
  terminalNode *temp = givenList->head;
  terminalNode *prev = NULL;
  if (temp != NULL && temp->t == EPS) {
    givenList->head = temp->next;
    free(temp);
    return;
  }
  while (temp != NULL) {
    if (temp->t == EPS) {
      break;
    }
    prev = temp;
    temp = temp->next;
  }
  if (temp == NULL)
    return;
  prev->next = temp->next;
  free(temp);
  return;
}

void computeFollow() {
  if (firstFollowSets[program].followSet == NULL) {
    firstFollowSets[program].followSet = createTerminalList();
  }

  addTerminalTolist(firstFollowSets[program].followSet, END_OF_INPUT);

  populateOccFollow();

  for (int i = 0; i < nonTerminalCount; i++) {
    findFollowset(i);
  }
}

void findFollowset(nonTerminals nt) {
  if (firstFollowSets[nt].followSet)
    return;

  firstFollowSets[nt].followSet = createTerminalList();

  followDS *followNode = followOccurrence[nt];

  while (followNode) {
    RHSNode *occurence = followNode->occurrence;
    RHSNode *nextSymbol = occurence->next;

    if(nextSymbol){
      if(nextSymbol->isT && (nextSymbol->v.t != EPS)){
        addTerminalTolist(firstFollowSets[nt].followSet, nextSymbol->v.t);
      }else{
        while(nextSymbol){
          if(!nextSymbol->isT){
           terminalList* firstSetOfNext = firstFollowSets[nextSymbol->v.nonT].firstSet;
            joinTerminallistExcEps(firstFollowSets[nt].followSet, firstSetOfNext);
            if(containsEpsilon(firstSetOfNext)){
              nextSymbol = nextSymbol->next;
            }else{
              break;
            }
          }else{
            addTerminalTolist(firstFollowSets[nt].followSet, nextSymbol->v.t);
            break;
          }
        }
        if(!nextSymbol){
          joinTerminallistExcEps(firstFollowSets[nt].followSet, firstFollowSets[followNode->parentNt].followSet);
        }
      }
    }
    else {
      joinTerminallistExcEps(firstFollowSets[nt].followSet,
                         firstFollowSets[followNode->parentNt].followSet);
    }
    followNode = followNode->next;
  }
}

void populateOccFollow() {
  for (int i = 0; i < nonTerminalCount; i++) {
    LHSNode *lhsNode = G->rules[i];

    for (ProductionRule *rule = lhsNode->rules; rule != NULL;
         rule = rule->nextRule) {

      for (RHSNode *rhs = rule->head; rhs != NULL; rhs = rhs->next) {
        if (!rhs->isT && rhs->v.nonT != lhsNode->lhs) {
          followDS *newFollow = createFollowDS();

          newFollow->occurrence = rhs;
          newFollow->parentNt = lhsNode->lhs;
          newFollow->next = followOccurrence[rhs->v.nonT];
          followOccurrence[rhs->v.nonT] = newFollow;
        }
      }
    }
  }
}

void joinTerminallistExcEps(terminalList *l1, terminalList *l2) {
  if (l1 == NULL || l2 == NULL || l2->head == NULL) {
    return;
  }

  terminalNode *cur = l2->head;

  while (cur != NULL) {
    if(cur->t == EPS){
      cur = cur -> next;
      continue;
    }
    terminalNode *search = l1->head;
    bool found = false;

    while (search != NULL) {
      if (search->t == cur->t) {
        found = true;
        break;
      }
      search = search->next;
    }

    if (!found) {
      terminalNode *newnode = (terminalNode *)malloc(sizeof(terminalNode));

      if (newnode == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
      }

      newnode->t = cur->t;
      newnode->next = NULL;

      if (l1->head == NULL) {
        l1->head = newnode;
      } else {
        terminalNode *last = l1->head;
        while (last->next != NULL) {
          last = last->next;
        }
        last->next = newnode;
      }
    }
    cur = cur->next;
  }
}

void createParseTable() {
  PT = (ParseTable *)malloc(sizeof(ParseTable));
  if (PT == NULL) {
    printf("Memory allocation failed for parse table.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nonTerminalCount; i++) {
    for (int j = 0; j < terminalCount; j++) {
      PT->table[i][j] = NULL;
      PT->isSyn[i][j] = false;
      if (j == TK_SEM || j == TK_END || j == TK_ENDIF || j == TK_ENDRECORD || j == TK_ENDUNION || j == TK_ENDWHILE || j == TK_ELSE || j == TK_SQR || j == TK_CL) {
        PT->isSyn[i][j] = true;
      }
    }
  }
}

void initiateParseTable() {
  for (int i = 0; i < nonTerminalCount; i++) {
    LHSNode *currentLhs = G->rules[i];
    if (!currentLhs) continue;

    ProductionRule *currentRule = currentLhs->rules;

    while (currentRule != NULL) {
      RHSNode *rhsNode = currentRule->head;
      int nullableChain = 1; 

      if (rhsNode == NULL) {
        terminalNode *followTerminal =
          firstFollowSets[i].followSet->head;

        while (followTerminal != NULL) {
          PT->table[i][followTerminal->t] = currentRule;
          followTerminal = followTerminal->next;
        }
        currentRule = currentRule->nextRule;
        continue;
      }

      while (rhsNode != NULL) {
        if (rhsNode->isT) {
          if (rhsNode->v.t != EPS) {
            PT->table[i][rhsNode->v.t] = currentRule;
            nullableChain = 0; 
          }
          break;
        } else {
          terminalNode *firstTerminal =
            firstFollowSets[rhsNode->v.nonT].firstSet->head;

          while (firstTerminal != NULL) {
            if (firstTerminal->t != EPS) {
              PT->table[i][firstTerminal->t] = currentRule;
            }
            firstTerminal = firstTerminal->next;
          }

          if (!containsEpsilon(firstFollowSets[rhsNode->v.nonT].firstSet)) {
            nullableChain = 0;
            terminalNode *followTerminal = firstFollowSets[i].followSet->head;
            while (followTerminal != NULL)
            {
              PT->isSyn[i][followTerminal->t] = true;
              followTerminal = followTerminal->next;
            }
            break;
          }
          rhsNode = rhsNode->next;
        }
      }
      if (nullableChain) {
        terminalNode *followTerminal =
          firstFollowSets[i].followSet->head;
        while (followTerminal != NULL) {
          PT->table[i][followTerminal->t] = currentRule;
          followTerminal = followTerminal->next;
        }
      }

      currentRule = currentRule->nextRule;
    }
  }
}

int push(int unionVal, bool isT){
  value newValue;
  if(isT){
    newValue.t = unionVal;
  }else{
    newValue.nonT = unionVal;
  }
  StackNode* node = (StackNode*)malloc(sizeof(StackNode));
  if(node == NULL){
    printf("Failed to allocate Memory for Node");
    return 0;
  }
  node->v = newValue;
  node->isT = isT;
  node->next = mainStack->head;
  mainStack->head = node;
  ++(mainStack->size);
  return 1;
}  

StackNode* top(){
  return mainStack->head;
}

int pop(){
  if(!(mainStack->head)) return 0; // stack should not be empty
  StackNode* temp = mainStack->head;
  mainStack->head = temp->next;
  /*free(temp);*/
  --(mainStack->size);
  return 1;
}

bool isEmpty(){
  return (mainStack->size <= 1) ? true : false;
}

TreeNode* pushListToStack(RHSNode* currNode, StackNode* parent, SymTableItem currToken){

  StackNode* newHead = (StackNode*)malloc(sizeof(StackNode));
  newHead->isT = currNode->isT;
  newHead->v = currNode->v;
  ++(mainStack->size);

  TreeNode* newTreeNode = (TreeNode*)malloc(sizeof(TreeNode));
  newTreeNode->isT = currNode->isT;
  newTreeNode->v = currNode->v;
  newTreeNode->parent = parent->treeNode;
  newTreeNode->next = NULL;
  parent->treeNode->firstChild = newTreeNode;
  newTreeNode->stackNode = newHead;
  newHead->treeNode = newTreeNode;

#ifdef DEBUG
  printf("Address of newTreeNode %s : %p\n", newTreeNode->isT ? terminalStrings[newTreeNode->v.t] : nonTerminalStrings[newTreeNode->v.nonT], newTreeNode);
  printf("%s\n", nonTerminalStrings[newTreeNode->parent->v.nonT]);
  printf("%s\n", newTreeNode->isT ? terminalStrings[newTreeNode->v.t] : nonTerminalStrings[newTreeNode->v.nonT]);
#endif
  StackNode* prevStackNode = NULL;
  StackNode* currStackNode = newHead;

  RHSNode* temp = currNode;
  temp = temp->next;
  while(temp != NULL){
    StackNode* tempStackNode = (StackNode*)malloc(sizeof(StackNode));
    tempStackNode->isT = temp->isT;
    tempStackNode->v = temp->v;

    TreeNode* tempTreeNode = (TreeNode*)malloc(sizeof(TreeNode));
    tempTreeNode->isT = temp->isT;
    tempTreeNode->v = temp->v;
    tempTreeNode->parent = parent->treeNode;
    tempTreeNode->firstChild = NULL;
    tempTreeNode->stackNode = tempStackNode;
    tempStackNode->treeNode = tempTreeNode;
    tempTreeNode->next = NULL;

#ifdef DEBUG
    // printf("Address of tempTreeNode %s : %p\n", tempTreeNode->isT ? terminalStrings[tempTreeNode->v.t] : nonTerminalStrings[tempTreeNode->v.nonT], tempTreeNode);
    // printf("%s\n", nonTerminalStrings[newTreeNode->parent->v.nonT]);
    // printf("%s\n", newTreeNode->isT ? terminalStrings[newTreeNode->v.t] : nonTerminalStrings[newTreeNode->v.nonT]);
#endif
    if (prevStackNode) {
      currStackNode->treeNode->next = tempTreeNode;
#ifdef DEBUG
      printf("%s\n", prevStackNode->treeNode->isT ? terminalStrings[prevStackNode->treeNode->v.t] : nonTerminalStrings[prevStackNode->treeNode->v.nonT]);
#endif
    }else{
      newTreeNode->next = tempTreeNode;
    }

    currStackNode->next = tempStackNode;
    prevStackNode = currStackNode;
    currStackNode = currStackNode->next;
    temp = temp->next;
    ++(mainStack->size);
  }
  currStackNode->next = mainStack->head;
  mainStack->head = newHead;
  return newTreeNode; 
}

void createParseTree(FILE* fp){
  push(END_OF_INPUT, 1);
  push(program, 0);

  SymTableItem currToken = getToken(fp);

  parseTreeRoot->parent = NULL;
  parseTreeRoot->firstChild = NULL;
  parseTreeRoot->next = NULL;
  parseTreeRoot->isT = false;
  parseTreeRoot->v.nonT = program;
  parseTreeRoot->token = currToken;
  parseTreeRoot->stackNode = mainStack->head;
  mainStack->head->treeNode = parseTreeRoot;

  while(!isEmpty() && currToken.eof == false){
    StackNode* currNode = top();
    printf("Token is: %s\n", terminalStrings[currToken.token]);
    if(currToken.token == TK_COMMENT){
      // CURRENT TOKEN IS A COMMENT
      currToken = getToken(fp);
      currToken.lineCount = lineCount;
      continue;
    }else if(currToken.token == TK_ERROR){
      // LEXER ERROR DETECTED
      currToken = getToken(fp);
      currToken.lineCount = lineCount;
      continue;
    }
    if(currNode->isT){
      // TOP OF THE STACK IS A TERMINAL
      if(currToken.token == currNode->v.t){
        // TOP OF STACK MATCHECD WITH INPUT TOKEN
        pop();
        currNode->treeNode->token = currToken;
        currToken = getToken(fp);
        currToken.lineCount = lineCount;
      }else{
        // PARSER ERROR DETECTED (TOP OF STACK DID NOT MATCH WITH INPUT TOKEN)
        printf("Line %d Error: The token %s for lexeme %s does not match expected token %s\n", currToken.lineCount, terminalStrings[currToken.token], currToken.lexeme, terminalStrings[currNode->v.t]);
        pop();
      }
    }else{
      // STACK TOP IS A NON-TERMINAL
      ProductionRule* pr = PT->table[currNode->v.nonT][currToken.token];
      if(pr){
        pop();
        RHSNode* temp = pr->head;
        if (temp->isT && temp->v.t == EPS) {
          TreeNode *newTreeNode = (TreeNode *)malloc(sizeof(TreeNode));
          newTreeNode->isT = true;
          newTreeNode->v.t = EPS;
          newTreeNode->parent = currNode->treeNode;
          newTreeNode->next = NULL;
          newTreeNode->firstChild = NULL;
          currNode->treeNode->firstChild = newTreeNode;
          continue;
        }
        else{
          TreeNode* firstChild = pushListToStack(temp, currNode, currToken);
          currNode->treeNode->firstChild = firstChild;
        }
      }else{
        // ENTRY IN PARSE TABLE NOT FOUND
        printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n", currToken.lineCount, terminalStrings[currToken.token], currToken.lexeme, nonTerminalStrings[currNode->v.nonT]);
        if(currToken.eof == false && mainStack->size > 1){
          if (!currNode->isT && PT->isSyn[currNode->v.nonT][currToken.token]) {
              pop();
              continue;
          }
          currToken = getToken(fp);
          currToken.lineCount = lineCount;
        }
      }
    }
  }
  if (mainStack->size == 1 && currToken.eof == true) {
    printf("Parsed the entire input\n");
  } else if (mainStack->size == 1 && currToken.eof == false) {
    printf("Error: Parsed the entire input but stack is not empty\n");
  } else if (mainStack->size != 1 && currToken.eof == true) {
    printf("Line %d Error: Stack is not empty but input is parsed\n", currToken.lineCount);
    while (mainStack->size > 1) {
      pop();
    }
  }
}

void printFirstandFollowSets() {
  printf("First and Follow Sets:\n");
  int count = 0;
  for (int i = 0; i < nonTerminalCount; i++) {
    printf("--------------------\n");
    printf("(%d) NON-TERMINAL : %s\n", i, nonTerminalStrings[i]);
    printf("First( %s ): ", nonTerminalStrings[i]);
    terminalNode *first = firstFollowSets[i].firstSet->head;
    printf("{");
    count = 0;
    while (first != NULL) {
      printf("%s,", terminalStrings[first->t]);
      ++count;
      first = first->next;
    }
    printf("} count = %d\n", count);

    printf("Follow( %s ): ", nonTerminalStrings[i]);
    terminalNode *follow = firstFollowSets[i].followSet->head;
    printf("{");
    count = 0;
    while (follow != NULL) {
      printf("%s,", terminalStrings[follow->t]);
      ++count;
      follow = follow->next;
    }
    printf("} count = %d\n", count);
  }
}

void printStack() {
  printf("\n\n Printing Stack \n\n");
  StackNode *temp = mainStack->head;
  while (temp != NULL) {
    if (temp->isT) {
      printf("Terminal: %s\n", terminalStrings[temp->v.t]);
    } else {
      printf("Non-terminal: %s\n", nonTerminalStrings[temp->v.nonT]);
    }
    temp = temp->next;
  }
  printf("\n Stack Print over\n\n");
}

void dfsHelper(TreeNode* currTreeNode, FILE* output){
  if(currTreeNode == NULL){
    return ; 
  }
  TreeNode* firstChild = currTreeNode->firstChild;
  dfsHelper(firstChild, output);
  if(currTreeNode->isT){
    if(strcmp(terminalStrings[currTreeNode->v.t], "EPS") == 0){
      fprintf(output, "|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|\n", "----", "NO_LINE", terminalStrings[currTreeNode->v.t], "NO_VAL", nonTerminalStrings[currTreeNode->parent->v.nonT], "YES", "----");
    }
    else if (currTreeNode->token.token == TK_NUM) {
      fprintf(output, "|%-25s|%-25d|%-25s|%-25d|%-25s|%-25s|%-25s|\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], currTreeNode->token.intVal, nonTerminalStrings[currTreeNode->parent->v.nonT], "YES", "----");
    } 
    else if (currTreeNode->token.token == TK_RNUM){
      fprintf(output, "|%-25s|%-25d|%-25s|%-25.2f|%-25s|%-25s|%-25s|\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], currTreeNode->token.realVal, nonTerminalStrings[currTreeNode->parent->v.nonT], "YES", "----");
    }
    else{
      fprintf(output, "|%-25s|%-25d|%-25s|%-25s|%-25s|%-25s|%-25s|\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], "NO_VAL", nonTerminalStrings[currTreeNode->parent->v.nonT], "YES", "----");
    }
  }
  else if (currTreeNode->v.nonT == program){
    fprintf(output, "|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|\n", "----", "NO_LINE", "----", "NO_VAL", "ROOT", "NO", nonTerminalStrings[currTreeNode->v.nonT]);
  }else{
    fprintf(output, "|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|\n", "----", "NO_LINE", "----", "NO_VAL", nonTerminalStrings[currTreeNode->parent->v.nonT], "NO", nonTerminalStrings[currTreeNode->v.nonT]);
  }
  fprintf(output, "|");
  for (int i = 0; i < 181; ++i) {
    fprintf(output, "-");
  }
  fprintf(output, "|\n");
  
  while(firstChild){
    firstChild = firstChild->next;
    dfsHelper(firstChild, output);
  }
}

void printTreeHelper(TreeNode* currTreeNode){
  if(currTreeNode == NULL){
    return ; 
  }
  if(currTreeNode->isT){
    printf("Terminal: %s\n", terminalStrings[currTreeNode->v.t]);
  }else{
    printf("Non-terminal: %s\n\n", nonTerminalStrings[currTreeNode->v.nonT]);
  }
  TreeNode* firstChild = currTreeNode->firstChild;
  while(firstChild){
    printTreeHelper(firstChild);
    firstChild = firstChild->next;
  }
}

void printParseTree(FILE* output){
  fprintf(output, "|");
  for (int i = 0; i < 181; ++i) {
    fprintf(output, "-");
  }
  fprintf(output, "|\n");
  fprintf(output, "|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|%-25s|\n", "Lexeme", "Line No.", "Token", "Value", "Parent Node", "Leaf Node", "Node Symbol");

  fprintf(output, "|");
  for (int i = 0; i < 181; ++i) {
    fprintf(output, "-");
  }
  fprintf(output, "|\n");
  dfsHelper(parseTreeRoot, output);
}

void printProductionRule(int nonTerminalIdx, ProductionRule *prodRule) {
  printf("<%s> ---> ", nonTerminalStrings[nonTerminalIdx]);
  RHSNode *currentRHS = prodRule->head;
  while (currentRHS != NULL) {
    if (currentRHS->isT) {
      printf("%s ", terminalStrings[currentRHS->v.t]);
    } else {
      printf("<%s> ", nonTerminalStrings[currentRHS->v.nonT]);
    }
    currentRHS = currentRHS->next;
  }
}

void printAllProductionRules(int nonTerminalIdx) {
  LHSNode *lhs = G->rules[nonTerminalIdx];
  printf("Non-terminal: %s\n", nonTerminalStrings[nonTerminalIdx]);
  ProductionRule *rule = lhs->rules;
  while (rule != NULL)
  {
    printProductionRule(nonTerminalIdx, rule);
    printf("\n");
    rule = rule->nextRule;
  }
}

void printParseTable() {
  printf("Non-terminals/Terminals,");
  for (int j = 0; j < terminalCount; j++) {
    printf("%s,", terminalStrings[j]);
  }
  printf("\n");

  for (int i = 0; i < nonTerminalCount; i++) {
    printf("%s,", nonTerminalStrings[i]);
    for (int j = 0; j < terminalCount; j++) {
      if (PT->table[i][j] != NULL) {
        printProductionRule(i, PT->table[i][j]);
        printf(",");
      } else if (PT->isSyn[i][j]) {
        printf("SYN,");
      } else {
        printf("ERROR,");
      }
    }
    printf("\n");
  }
}

// void cleanUpDfs(TreeNode* currTreeNode) {
//   if (currTreeNode == NULL) {
//     return;
//   }

//   TreeNode* firstChild = currTreeNode->firstChild;
//   while (firstChild) {
//     cleanUpDfs(firstChild);
//     firstChild = firstChild->next;
//   }
//   free(currTreeNode->stackNode);
//   free(currTreeNode);
// }

// void cleanUp() {
//   cleanUpDfs(parseTreeRoot);
//   free(mainStack);
//   for (int i = 0; i < nonTerminalCount; i++) {
//     free(firstFollowSets[i].firstSet);
//     free(firstFollowSets[i].followSet);
//   }
//   free(firstFollowSets);
//   free(followOccurrence);
//   free(PT);
//   free(G);
// }
