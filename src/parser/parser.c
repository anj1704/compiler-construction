#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
#include <stdlib.h>

extern char *terminalStrings[];
extern char *nonTerminalStrings[];
extern followDS *follow_occurrence[nonTerminalCount];
extern Grammar *G;
extern non_terminal_sets first_follow_sets[nonTerminalCount];
extern ParseTable *PT;
extern gitems **itemList;
extern Stack* mainStack;
extern TreeNode* parseTreeRoot;

gitems *createNonTerminal(nonTerminals nt) {
  gitems *item = (gitems *)malloc(sizeof(gitems));
  if (!item) {
    printf("Failed to allocate memory for Non Terminal\n");
  } else {
    item->isTer = false;
    item->v.non_t = nt;
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
    rule->next_rule = NULL;
    rule->head = NULL;
  }
  return rule;
}

terminal_list *createTerminalList() {
  terminal_list *list = (terminal_list *)malloc(sizeof(terminal_list));
  if (!list)
    printf("Failed to allocate memory for Terminal List\n");
  else
    list->head = NULL;
  return list;
}

terminal_node *createTerminalNode() {
  terminal_node *node = (terminal_node *)malloc(sizeof(terminal_node));
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
  if (currentrulehead->next_rule != NULL) {
    while (currentrulehead->next_rule != NULL)
      currentrulehead = currentrulehead->next_rule;
  }
  currentrulehead->next_rule = newRule;

  // Add the LHSNode to the grammar
  G->rules[nt] = lhsNode;
}

void addGrammarRules() {
  gitems i1[2] = {{false, {.non_t = otherFunctions}},
                  {false, {.non_t = mainFunction}}};
  addrule(program, 2, i1);

  gitems i2[3] = {
      {true, {TK_MAIN}}, {false, {.non_t = stmts}}, {true, {TK_END}}};
  addrule(mainFunction, 3, i2);

  gitems i3a[2] = {{false, {.non_t = function}},
                   {false, {.non_t = otherFunctions}}};
  addrule(otherFunctions, 2, i3a);

  gitems i3b[1] = {{true, {EPS}}};
  addrule(otherFunctions, 1, i3b);

  gitems i4[6] = {{true, {TK_FUNID}},
                  {false, {.non_t = input_par}},
                  {false, {.non_t = output_par}},
                  {true, {TK_SEM}},
                  {false, {.non_t = stmts}},
                  {true, {TK_END}}};
  addrule(function, 6, i4);

  gitems i5[6] = {{true, {TK_INPUT}},
                  {true, {TK_PARAMETER}},
                  {true, {TK_LIST}},
                  {true, {TK_SQL}},
                  {false, {.non_t = parameter_list}},
                  {true, {TK_SQR}}};
  addrule(input_par, 6, i5);

  gitems i6a[6] = {{true, {TK_OUTPUT}},
                   {true, {TK_PARAMETER}},
                   {true, {TK_LIST}},
                   {true, {TK_SQL}},
                   {false, {.non_t = parameter_list}},
                   {true, {TK_SQR}}};
  addrule(output_par, 6, i6a);

  gitems i6b[1] = {{true, {EPS}}};
  addrule(output_par, 1, i6b);

  gitems i7[3] = {{false, {.non_t = dataType}},
                  {true, {TK_ID}},
                  {false, {.non_t = remaining_list}}};
  addrule(parameter_list, 3, i7);

  gitems i8a[1] = {{false, {.non_t = primitiveDatatype}}};
  addrule(dataType, 1, i8a);

  gitems i8b[1] = {{false, {.non_t = constructedDatatype}}};
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

  gitems i11a[2] = {{true, {TK_COMMA}}, {false, {.non_t = parameter_list}}};
  addrule(remaining_list, 2, i11a);

  gitems i11b[1] = {{true, {EPS}}};
  addrule(remaining_list, 1, i11b);

  gitems i12[4] = {{false, {.non_t = typeDefinitions}},
                   {false, {.non_t = declarations}},
                   {false, {.non_t = otherStmts}},
                   {false, {.non_t = returnStmt}}};
  addrule(stmts, 4, i12);

  gitems i13a1[2] = {{false, {.non_t = actualOrRedefined}},
                     {false, {.non_t = typeDefinitions}}};
  addrule(typeDefinitions, 2, i13a1);

  gitems i13a2[1] = {{true, {EPS}}};
  addrule(typeDefinitions, 1, i13a2);

  gitems i13b1[1] = {{false, {.non_t = typeDefinition}}};
  addrule(actualOrRedefined, 1, i13b1);

  gitems i13b2[1] = {{false, {.non_t = definetypestmt}}};
  addrule(actualOrRedefined, 1, i13b2);

  gitems i14[4] = {{true, {TK_RECORD}},
                   {true, {TK_RUID}},
                   {false, {.non_t = fieldDefinitions}},
                   {true, {TK_ENDRECORD}}};
  addrule(typeDefinition, 4, i14);

  gitems i15[4] = {{true, {TK_UNION}},
                   {true, {TK_RUID}},
                   {false, {.non_t = fieldDefinitions}},
                   {true, {TK_ENDUNION}}};
  addrule(typeDefinition, 4, i15);

  gitems i16[3] = {{false, {.non_t = fieldDefinition}},
                   {false, {.non_t = fieldDefinition}},
                   {false, {.non_t = moreFields}}};
  addrule(fieldDefinitions, 3, i16);

  gitems i17a[5] = {{true, {TK_TYPE}},
                    {false, {.non_t = fieldtype}},
                    {true, {TK_COLON}},
                    {true, {TK_FIELDID}},
                    {true, {TK_SEM}}};
  addrule(fieldDefinition, 5, i17a);

  gitems i17b[1] = {{false, {.non_t = primitiveDatatype}}};
  addrule(fieldtype, 1, i17b);

  gitems i17c[1] = {{false, {.non_t = constructedDatatype}}};
  addrule(fieldtype, 1, i17c);

  gitems i18a[2] = {{false, {.non_t = fieldDefinition}},
                    {false, {.non_t = moreFields}}};
  addrule(moreFields, 2, i18a);

  gitems i18b[1] = {{true, {EPS}}};
  addrule(moreFields, 1, i18b);

  gitems i19a[2] = {{false, {.non_t = declaration}},
                    {false, {.non_t = declarations}}};
  addrule(declarations, 2, i19a);

  gitems i19b[1] = {{true, {EPS}}};
  addrule(declarations, 1, i19b);

  gitems i20[6] = {
      {true, {TK_TYPE}}, {false, {.non_t = dataType}},      {true, {TK_COLON}},
      {true, {TK_ID}},   {false, {.non_t = global_or_not}}, {true, {TK_SEM}}};
  addrule(declaration, 6, i20);

  gitems i21a[2] = {{true, {TK_COLON}}, {true, {TK_GLOBAL}}};
  addrule(global_or_not, 2, i21a);

  gitems i21b[1] = {{true, {EPS}}};
  addrule(global_or_not, 1, i21b);

  gitems i22a[2] = {{false, {.non_t = stmt}}, {false, {.non_t = otherStmts}}};
  addrule(otherStmts, 2, i22a);

  gitems i22b[1] = {{true, {EPS}}};
  addrule(otherStmts, 1, i22b);

  gitems i23a[1] = {{false, {.non_t = assignmentStmt}}};
  addrule(stmt, 1, i23a);

  gitems i23b[1] = {{false, {.non_t = iterativeStmt}}};
  addrule(stmt, 1, i23b);

  gitems i23c[1] = {{false, {.non_t = conditionalStmt}}};
  addrule(stmt, 1, i23c);

  gitems i23d[1] = {{false, {.non_t = ioStmt}}};
  addrule(stmt, 1, i23d);

  gitems i23e[1] = {{false, {.non_t = funCallStmt}}};
  addrule(stmt, 1, i23e);

  gitems i24[4] = {{false, {.non_t = singleOrRecId}},
                   {true, {TK_ASSIGNOP}},
                   {false, {.non_t = arithmeticExpression}},
                   {true, {TK_SEM}}};
  addrule(assignmentStmt, 4, i24);

  gitems i25a[2] = {{true, {TK_ID}},
                    {false, {.non_t = option_single_constructed}}};
  addrule(singleOrRecId, 2, i25a);

  gitems i25b[2] = {{false, {.non_t = oneExpansion}},
                    {false, {.non_t = moreExpansions}}};
  addrule(option_single_constructed, 2, i25b);

  gitems i25p[1] = {{true, {EPS}}};
  addrule(option_single_constructed, 1, i25p);

  gitems i25c[2] = {{true, {TK_DOT}}, {true, {TK_FIELDID}}};
  addrule(oneExpansion, 2, i25c);

  gitems i25d[2] = {{false, {.non_t = oneExpansion}},
                    {false, {.non_t = moreExpansions}}};
  addrule(moreExpansions, 2, i25d);

  gitems i25e[1] = {{true, {EPS}}};
  addrule(moreExpansions, 1, i25e);

  gitems i26[7] = {{false, {.non_t = outputParameters}},
                   {true, {TK_CALL}},
                   {true, {TK_FUNID}},
                   {true, {TK_WITH}},
                   {true, {TK_PARAMETERS}},
                   {false, {.non_t = inputParameters}},
                   {true, {TK_SEM}}};
  addrule(funCallStmt, 7, i26);

  gitems i27a[4] = {{true, {TK_SQL}},
                    {false, {.non_t = idList}},
                    {true, {TK_SQR}},
                    {true, {TK_ASSIGNOP}}};
  addrule(outputParameters, 4, i27a);

  gitems i27b[1] = {{true, {EPS}}};
  addrule(outputParameters, 1, i27b);

  gitems i28[3] = {
      {true, {TK_SQL}}, {false, {.non_t = idList}}, {true, {TK_SQR}}};
  addrule(inputParameters, 3, i28);

  gitems i29[7] = {{true, {TK_WHILE}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}},
                   {false, {.non_t = stmt}},
                   {false, {.non_t = otherStmts}},
                   {true, {TK_ENDWHILE}}};
  addrule(iterativeStmt, 7, i29);

  gitems i30[8] = {{true, {TK_IF}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}},
                   {true, {TK_THEN}},
                   {false, {.non_t = stmt}},
                   {false, {.non_t = otherStmts}},
                   {false, {.non_t = elsePart}}};
  addrule(conditionalStmt, 8, i30);

  gitems i31a[4] = {{true, {TK_ELSE}},
                    {false, {.non_t = stmt}},
                    {false, {.non_t = otherStmts}},
                    {true, {TK_ENDIF}}};
  addrule(elsePart, 4, i31a);

  gitems i31b[1] = {{true, {TK_ENDIF}}};
  addrule(elsePart, 1, i31b);

  gitems i32a[5] = {{true, {TK_READ}},
                    {true, {TK_OP}},
                    {false, {.non_t = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(ioStmt, 5, i32a);

  gitems i32b[5] = {{true, {TK_WRITE}},
                    {true, {TK_OP}},
                    {false, {.non_t = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(ioStmt, 5, i32b);

  gitems i33a[2] = {{false, {.non_t = term}}, {false, {.non_t = expPrime}}};
  addrule(arithmeticExpression, 2, i33a);

  gitems i33b1[3] = {{false, {.non_t = lowPrecedenceOperators}},
                     {false, {.non_t = term}},
                     {false, {.non_t = expPrime}}};
  addrule(expPrime, 3, i33b1);

  gitems i33b2[1] = {{true, {EPS}}};
  addrule(expPrime, 1, i33b2);

  gitems i33c[2] = {{false, {.non_t = factor}}, {false, {.non_t = termPrime}}};
  addrule(term, 2, i33c);

  gitems i34a[3] = {{false, {.non_t = highPrecedenceOperators}},
                    {false, {.non_t = factor}},
                    {false, {.non_t = termPrime}}};
  addrule(termPrime, 3, i34a);

  gitems i34b[1] = {{true, {EPS}}};
  addrule(termPrime, 1, i34b);

  gitems i35a1[3] = {{true, {TK_OP}},
                     {false, {.non_t = arithmeticExpression}},
                     {true, {TK_CL}}};
  addrule(factor, 3, i35a1);

  gitems i35a2[1] = {{false, {.non_t = var}}};
  addrule(factor, 1, i35a2);

  gitems i35b1[1] = {{true, {TK_MUL}}};
  addrule(highPrecedenceOperators, 1, i35b1);

  gitems i35b2[1] = {{true, {TK_DIV}}};
  addrule(highPrecedenceOperators, 1, i35b2);

  gitems i35c1[1] = {{true, {TK_PLUS}}};
  addrule(lowPrecedenceOperators, 1, i35c1);

  gitems i35c2[1] = {{true, {TK_MINUS}}};
  addrule(lowPrecedenceOperators, 1, i35c2);

  gitems i36[7] = {{true, {TK_OP}}, {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}, {false, {.non_t = logicalOp}},
                   {true, {TK_OP}}, {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(booleanExpression, 7, i36);

  gitems i37[3] = {{false, {.non_t = var}},
                   {false, {.non_t = relationalOp}},
                   {false, {.non_t = var}}};
  addrule(booleanExpression, 3, i37);

  gitems i38[4] = {{true, {TK_NOT}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(booleanExpression, 4, i38);

  gitems i39a[1] = {{false, {.non_t = singleOrRecId}}};
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
                   {false, {.non_t = optionalReturn}},
                   {true, {TK_SEM}}};
  addrule(returnStmt, 3, i42);

  gitems i43a[3] = {
      {true, {TK_SQL}}, {false, {.non_t = idList}}, {true, {TK_SQR}}};
  addrule(optionalReturn, 3, i43a);

  gitems i43b[1] = {{true, {EPS}}};
  addrule(optionalReturn, 1, i43b);

  gitems i44[2] = {{true, {TK_ID}}, {false, {.non_t = more_ids}}};
  addrule(idList, 2, i44);

  gitems i45a[2] = {{true, {TK_COMMA}}, {false, {.non_t = idList}}};
  addrule(more_ids, 2, i45a);

  gitems i45b[1] = {{true, {EPS}}};
  addrule(more_ids, 1, i45b);

  gitems i46[5] = {{true, {TK_DEFINETYPE}},
                   {false, {.non_t = A}},
                   {true, {TK_RUID}},
                   {true, {TK_AS}},
                   {true, {TK_RUID}}};
  addrule(definetypestmt, 5, i46);

  gitems i47a[1] = {{true, {TK_RECORD}}};
  addrule(A, 1, i47a);

  gitems i47b[1] = {{true, {TK_UNION}}};
  addrule(A, 1, i47b);
}

void compute_first(nonTerminals given_nt) {
  if (first_follow_sets[given_nt].first_set)
    return;

#ifdef DEBUG
  printf("compute_first start: %s\n", nonTerminalStrings[given_nt]);
#endif
  first_follow_sets[given_nt].first_set = createTerminalList();

  LHSNode *lhs = G->rules[given_nt];
  for (ProductionRule *rule = lhs->rules; rule != NULL;
       rule = rule->next_rule) {
    for (RHSNode *rhs = rule->head; rhs != NULL; rhs = rhs->next) {
      if (rhs->isT) {
#ifdef DEBUG
        printf("terminal: %s\n", terminalStrings[rhs->v.t]);
#endif
        add_terminal_tolist(first_follow_sets[given_nt].first_set, rhs->v.t);
        break;
      } else {
#ifdef DEBUG
        printf("non terminal: %s\n", nonTerminalStrings[rhs->v.non_t]);
#endif
        compute_first(rhs->v.non_t);
        join_terminal_list(first_follow_sets[given_nt].first_set,
                           first_follow_sets[rhs->v.non_t].first_set);

        // Made change to check if all non-terminals contain an EChanged
        if (contains_epsilon(first_follow_sets[rhs->v.non_t].first_set) && (rhs->next != NULL)) {
          remove_epsilon(first_follow_sets[given_nt].first_set);
        } else
          break;
      }
    }
  }
#ifdef DEBUG
  printf("compute_first done: %s\n", nonTerminalStrings[given_nt]);
#endif
}

void compute_firsts() {
  for (int i = 0; i < nonTerminalCount; i++) {
    compute_first(i);
  }
}

void add_terminal_tolist(terminal_list *list, terminals t) {

  if (list->head == NULL) {
    list->head = createTerminalNode();
    list->head->t = t;
  } else {
    terminal_node *cur = list->head;
    terminal_node *prev = NULL;

    while (cur != NULL) {
      if (cur->t == t) {
        return;
      }

      prev = cur;
      cur = cur->next;
    }

    terminal_node *newnode = createTerminalNode();
    newnode->t = t;
    newnode->next = NULL;

    if (prev != NULL) {
      prev->next = newnode;
    }
  }
}

void join_terminal_list(terminal_list *l1, terminal_list *l2) {
  if (l1 == NULL || l2 == NULL || l2->head == NULL) {
    return;
  }

  terminal_node *cur = l2->head;

  while (cur != NULL) {
    terminal_node *search = l1->head;
    bool found = false;

    while (search != NULL) {
      if (search->t == cur->t) {
        found = true;
        break;
      }
      search = search->next;
    }

    if (!found) {
      terminal_node *newnode = createTerminalNode();

      if (newnode == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
      }

      newnode->t = cur->t;
      newnode->next = NULL;

      if (l1->head == NULL) {
        l1->head = newnode;
      } else {
        terminal_node *last = l1->head;
        while (last->next != NULL) {
          last = last->next;
        }
        last->next = newnode;
      }
    }

    cur = cur->next;
  }
}

bool contains_epsilon(terminal_list *given_list) {
  terminal_node *temp = given_list->head;
  while (temp != NULL) {
    if (temp->t == EPS) {
      return true;
    }
    temp = temp->next;
  }
  return false;
}

void remove_epsilon(terminal_list *given_list) {
  terminal_node *temp = given_list->head;
  terminal_node *prev = NULL;
  if (temp != NULL && temp->t == EPS) {
    given_list->head = temp->next;
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

void compute_follow() {
  if (first_follow_sets[program].follow_set == NULL) {
    first_follow_sets[program].follow_set = createTerminalList();
  }

  add_terminal_tolist(first_follow_sets[program].follow_set, END_OF_INPUT);

  populate_occ_follow();

  for (int i = 0; i < nonTerminalCount; i++) {
    find_followset(i);
  }
}

void find_followset(nonTerminals nt) {
  if (first_follow_sets[nt].follow_set)
    return;

  first_follow_sets[nt].follow_set = createTerminalList();

  followDS *followNode = follow_occurrence[nt];

  while (followNode) {
    RHSNode *occurence = followNode->occurrence;
    RHSNode *nextSymbol = occurence->next;

    if(nextSymbol){
      if(nextSymbol->isT && (nextSymbol->v.t != EPS)){
        add_terminal_tolist(first_follow_sets[nt].follow_set, nextSymbol->v.t);
      }else{
        while(nextSymbol){
          if(!nextSymbol->isT){
           terminal_list* first_set_of_next = first_follow_sets[nextSymbol->v.non_t].first_set;
            join_terminallist_exc_eps(first_follow_sets[nt].follow_set, first_set_of_next);
            if(contains_epsilon(first_set_of_next)){
              nextSymbol = nextSymbol->next;
            }else{
              break;
            }
          }else{
            add_terminal_tolist(first_follow_sets[nt].follow_set, nextSymbol->v.t);
            break;
          }
        }
        if(!nextSymbol){
          join_terminallist_exc_eps(first_follow_sets[nt].follow_set, first_follow_sets[followNode->parent_nt].follow_set);
        }
      }
    }
    else {
      join_terminallist_exc_eps(first_follow_sets[nt].follow_set,
                         first_follow_sets[followNode->parent_nt].follow_set);
    }
    followNode = followNode->next;
  }
}

void populate_occ_follow() {
  for (int i = 0; i < nonTerminalCount; i++) {
    LHSNode *lhsNode = G->rules[i];

    for (ProductionRule *rule = lhsNode->rules; rule != NULL;
         rule = rule->next_rule) {

      for (RHSNode *rhs = rule->head; rhs != NULL; rhs = rhs->next) {
        if (!rhs->isT && rhs->v.non_t != lhsNode->lhs) {
          followDS *newFollow = createFollowDS();

          newFollow->occurrence = rhs;
          newFollow->parent_nt = lhsNode->lhs;
          newFollow->next = follow_occurrence[rhs->v.non_t];
          follow_occurrence[rhs->v.non_t] = newFollow;
        }
      }
    }
  }
}

void join_terminallist_exc_eps(terminal_list *l1, terminal_list *l2) {
  if (l1 == NULL || l2 == NULL || l2->head == NULL) {
    return;
  }

  terminal_node *cur = l2->head;

  while (cur != NULL) {
    if(cur->t == EPS){
      cur = cur -> next;
      continue;
    }
    terminal_node *search = l1->head;
    bool found = false;

    while (search != NULL) {
      if ((search->t == cur->t)) {
        found = true;
        break;
      }
      search = search->next;
    }

    if (!found) {
      terminal_node *newnode = (terminal_node *)malloc(sizeof(terminal_node));

      if (newnode == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        exit(EXIT_FAILURE);
      }

      newnode->t = cur->t;
      newnode->next = NULL;

      if (l1->head == NULL) {
        l1->head = newnode;
      } else {
        terminal_node *last = l1->head;
        while (last->next != NULL) {
          last = last->next;
        }
        last->next = newnode;
      }
    }
    cur = cur->next;
  }
}

void create_parse_table() {
  PT = (ParseTable *)malloc(sizeof(ParseTable));
  if (PT == NULL) {
    printf("Memory allocation failed for parse table.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nonTerminalCount; i++) {
    for (int j = 0; j < terminalCount; j++) {
      PT->table[i][j] = NULL;
    }
  }
}

void initiate_parse_table() {
  for (int i = 0; i < nonTerminalCount; i++) {
    LHSNode *current_lhs = G->rules[i];
    if (!current_lhs) continue;

    ProductionRule *current_rule = current_lhs->rules;

    while (current_rule != NULL) {
      RHSNode *rhs_node = current_rule->head;
      int nullable_chain = 1; 

      if (rhs_node == NULL) {
        terminal_node *follow_terminal =
          first_follow_sets[i].follow_set->head;

        while (follow_terminal != NULL) {
          PT->table[i][follow_terminal->t] = current_rule;
          follow_terminal = follow_terminal->next;
        }
        current_rule = current_rule->next_rule;
        continue;
      }

      while (rhs_node != NULL) {
        if (rhs_node->isT) {
          if (rhs_node->v.t != EPS) {
            PT->table[i][rhs_node->v.t] = current_rule;
            nullable_chain = 0; 
          }
          break;
        } else {
          terminal_node *first_terminal =
            first_follow_sets[rhs_node->v.non_t].first_set->head;

          while (first_terminal != NULL) {
            if (first_terminal->t != EPS) {
              PT->table[i][first_terminal->t] = current_rule;
            }
            first_terminal = first_terminal->next;
          }

          if (!contains_epsilon(first_follow_sets[rhs_node->v.non_t].first_set)) {
            nullable_chain = 0;
            break;
          }
          rhs_node = rhs_node->next;
        }
      }
      if (nullable_chain) {
        terminal_node *follow_terminal =
          first_follow_sets[i].follow_set->head;
        while (follow_terminal != NULL) {
          PT->table[i][follow_terminal->t] = current_rule;
          follow_terminal = follow_terminal->next;
        }
      }

      current_rule = current_rule->next_rule;
    }
  }
}

int push(int union_val, bool isT){
  value newValue;
  if(isT){
    newValue.t = union_val;
  }else{
    newValue.non_t = union_val;
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


TreeNode* pushListToStack(RHSNode* currNode, TreeNode* parent){

  StackNode* newHead = (StackNode*)malloc(sizeof(StackNode));
  newHead->isT = currNode->isT;
  newHead->v = currNode->v;
  ++(mainStack->size);

  TreeNode* newTreeNode = (TreeNode*)malloc(sizeof(TreeNode));
  newTreeNode->isT = currNode->isT;
  newTreeNode->v = currNode->v;
  newTreeNode->parent = parent;
  newTreeNode->next = NULL;
  parent->firstChild = newTreeNode;
  newTreeNode->stackNode = newHead;
  newHead->treeNode = newTreeNode;

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
    tempTreeNode->parent = parent;
    tempTreeNode->firstChild = NULL;
    tempTreeNode->stackNode = tempStackNode;
    tempStackNode->treeNode = tempTreeNode;

    if(prevStackNode) tempTreeNode->next = prevStackNode->treeNode;
    else tempTreeNode->next = NULL;

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

void printStack() {
  printf("\n\n Printing Stack \n\n");
  StackNode *temp = mainStack->head;
  while (temp != NULL) {
    if (temp->isT) {
      printf("Terminal: %s\n", terminalStrings[temp->v.t]);
    } else {
      printf("Non-terminal: %s\n", nonTerminalStrings[temp->v.non_t]);
    }
    temp = temp->next;
  }
  printf("\n Stack Print over\n\n");
}

void createParseTree(FILE* fp){
  push(END_OF_INPUT, 1);
  push(program, 0);

  SymTableItem currToken = getToken(fp);

  parseTreeRoot->parent = NULL;
  parseTreeRoot->firstChild = NULL;
  parseTreeRoot->next = NULL;
  parseTreeRoot->isT = false;
  parseTreeRoot->v.non_t = program;
  parseTreeRoot->token = currToken;
  parseTreeRoot->stackNode = mainStack->head;
  mainStack->head->treeNode = parseTreeRoot;

  // printf("Lexeme : %s Line count : %d\n\n", currToken.lexeme, currToken.lineCount);
  while(!isEmpty() || currToken.eof == false){
    StackNode* currNode = top();
    printf("From tree : %s\n", currNode->treeNode->isT ? terminalStrings[currNode->v.t] : nonTerminalStrings[currNode->v.non_t]);
    printf("Parent of tree : %s\n", currNode->treeNode->parent ? nonTerminalStrings[currNode->treeNode->parent->v.non_t] : "ROOT");
    printf("Right sibling of node : %s\n", currNode->treeNode->next ? currNode->treeNode->next->isT ? terminalStrings[currNode->treeNode->next->v.t] : nonTerminalStrings[currNode->treeNode->next->v.non_t] : "----");
    printf("From stack : %s\n\n\n", currNode->isT ? terminalStrings[currNode->v.t] : nonTerminalStrings[currNode->v.non_t]);
    /*if(currNode->isT){*/
    /*  printf("Terminal: %s\n\n", terminalStrings[currNode->v.t]);*/
    /*}else{*/
    /*  printf("Non-terminal: %s\n\n", nonTerminalStrings[currNode->v.non_t]);*/
    /*}*/
    if(currNode->isT){
      if(currToken.token == currNode->v.t){
        pop();

        TreeNode *terminalTreeNode = (TreeNode*)malloc(sizeof(TreeNode));
        terminalTreeNode->isT = true;
        terminalTreeNode->v.t = currNode->v.t;
        terminalTreeNode->token = currToken;
        terminalTreeNode->parent = currNode->treeNode;
        terminalTreeNode->next = NULL;
        terminalTreeNode->firstChild = NULL;
        currNode->treeNode->firstChild = terminalTreeNode;

        currToken = getToken(fp);
      }else{
        printf("Throws error\n");
      }
    }else{
      ProductionRule* pr = PT->table[currNode->v.non_t][currToken.token];
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
          TreeNode* firstChild = pushListToStack(temp, currNode->treeNode);
          currNode->treeNode->firstChild = firstChild;
        }
      }else{
        printf("Rule does not exist in the parse table\n");
        break;
      }
    }
  }
  if (mainStack->size == 1 && currToken.eof == true) {
    printf("Parsed the entire tree\n");
  } else {
    printf("Error in parsing\n");
  }
}

void printFirstandFollowSets() {
  printf("First and Follow Sets:\n");
  int count = 0;
  for (int i = 0; i < nonTerminalCount; i++) {
    printf("--------------------\n");
    printf("(%d) NON-TERMINAL : %s\n", i, nonTerminalStrings[i]);
    printf("First( %s ): ", nonTerminalStrings[i]);
    terminal_node *first = first_follow_sets[i].first_set->head;
    printf("{");
    count = 0;
    while (first != NULL) {
      printf("%s,", terminalStrings[first->t]);
      ++count;
      first = first->next;
    }
    printf("} count = %d\n", count);

    printf("Follow( %s ): ", nonTerminalStrings[i]);
    terminal_node *follow = first_follow_sets[i].follow_set->head;
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

void dfsHelper(TreeNode* currTreeNode){
  if(currTreeNode == NULL){
    return ; 
  }
  // if(currTreeNode->isT){
  //   printf("Terminal: %s\n\n", terminalStrings[currTreeNode->v.t]);
  // }else{
  //   printf("Non-terminal: %s\n\n", nonTerminalStrings[currTreeNode->v.non_t]);
  // }
  TreeNode* firstChild = currTreeNode->firstChild;
  int count = 0;
  
  if (firstChild) ++count;
  while(firstChild){
    count++;
    // dfsHelper(firstChild);
    firstChild = firstChild->next;
  }

  printf("Number of kids = %d\n", count);
  firstChild = currTreeNode->firstChild;
  for (int i = 0; i < count-1; ++i) {
    dfsHelper(firstChild);
    firstChild = firstChild->next;
  }
  if(currTreeNode->isT){
    if (currTreeNode->token.token == TK_NUM) {
      printf("\t%s\t%d\t%s\t%d\t%s\t%s\t%s\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], currTreeNode->token.intVal, nonTerminalStrings[currTreeNode->parent->v.non_t], "yes", "----"); 
    } 
    else if (currTreeNode->token.token == TK_RNUM){
      printf("\t%s\t%d\t%s\t%d\t%s\t%s\t%s\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], currTreeNode->token.realVal, nonTerminalStrings[currTreeNode->parent->v.non_t], "yes", "----"); 
    }
    printf("\t%s\t%d\t%s\t%d\t%s\t%s\t%s\n", currTreeNode->token.lexeme, currTreeNode->token.lineCount, terminalStrings[currTreeNode->v.t], -1, nonTerminalStrings[currTreeNode->parent->v.non_t],"yes","----"); 
  }else if (currTreeNode->v.non_t == program){
    printf("\t%s\t%d\t%s\t%d\t%s\t%s\t%s\n", "----", -1, nonTerminalStrings[currTreeNode->v.non_t], -1, "ROOT","no", nonTerminalStrings[currTreeNode->v.non_t]);
  }else{
    printf("\t%s\t%d\t%s\t%d\t%s\t%s\t%s\n", "----", -1, nonTerminalStrings[currTreeNode->v.non_t], -1, nonTerminalStrings[currTreeNode->parent->v.non_t],"no", nonTerminalStrings[currTreeNode->v.non_t]);
  }
  dfsHelper(firstChild);
}

void printParseTree(){
  dfsHelper(parseTreeRoot);
}

void printProductionRule(int nonTerminalIdx, ProductionRule *prodRule) {
  printf("<%s> ---> ", nonTerminalStrings[nonTerminalIdx]);
  RHSNode *currentRHS = prodRule->head;
  while (currentRHS != NULL) {
    if (currentRHS->isT) {
      printf("%s ", terminalStrings[currentRHS->v.t]);
    } else {
      printf("<%s> ", nonTerminalStrings[currentRHS->v.non_t]);
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
    rule = rule->next_rule;
  }
}

void print_parse_table() {
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
      } else {
        printf("-,");
      }
    }
    printf("\n");
  }
}

// void print_parse_table() {
//   /*printf("Parse Table:\n");*/
//   printf("Non-terminals/Terminals,");
//   for (int j = 0; j < terminalCount; j++) {
//     printf("%s,", terminalStrings[j]);
//   }
//   printf("\n");

//   for (int i = 0; i < nonTerminalCount; i++) {
//     printf("%s,", nonTerminalStrings[i]);
//     for (int j = 0; j < terminalCount; j++) {
//       if (PT->table[i][j] != NULL) {
//         if (PT->table[i][j]->head->isT) {
//           // printf("%s,", terminalStrings[PT->table[i][j]->head->v.t]);
//           printProductionRule(i, PT->table[i][j]);
//         } else {
//           // printf("%s,",
//           nonTerminalStrings[PT->table[i][j]->head->v.non_t]);
//         }
//       } else {
//         printf("NULL,");
//       }
//     }
//     printf("\n");
//   }
// }
/*bool createParseTree(FILE *fp) {*/
/*  StackNode *startNode = (StackNode *)malloc(sizeof(StackNode));*/
/*  if (startNode == NULL) {*/
/*    fprintf(stderr, "Memory allocation failed for StackNode.\n");*/
/*    exit(EXIT_FAILURE);*/
/*  }*/
/*  startNode->isT = true;*/
/*  startNode->value.t = END_OF_INPUT;*/
/*  startNode->next = NULL;*/
/*  StackNode *parsingStack = startNode;*/
/**/
/*  StackNode *programNode = (StackNode *)malloc(sizeof(StackNode));*/
/*  if (programNode == NULL) {*/
/*    fprintf(stderr, "Memory allocation failed for StackNode\n");*/
/*    exit(EXIT_FAILURE);*/
/*  }*/
/*  programNode->isT = false;*/
/*  programNode->value.non_t = program;*/
/*  programNode->next = parsingStack;*/
/*  parsingStack = programNode;*/
/**/
/*  root = (treeNode *)malloc(sizeof(treeNode));*/
/*  if (root == NULL) {*/
/*    fprintf(stderr, "Memory allocation failed for root node.\n");*/
/*    exit(EXIT_FAILURE);*/
/*  }*/
/*  root->parent = NULL;*/
/*  root->firstChild = NULL;*/
/*  root->rightSibling = NULL;*/
/*  root->isT = false;*/
/*  root->v.non_t = program;*/
/*  treeNode *currentParent = root;*/
/*  SymTableItem currentToken = getToken(fp);*/
/**/
/*  while (parsingStack != NULL) {*/
/*    StackNode *top = parsingStack;*/
/*    parsingStack = parsingStack->next;*/
/**/
/*    if (top->isT && top->value.t == END_OF_INPUT) {*/
/*      break;*/
/*    }*/
/*    if (!top->isT) {*/
/*      printf("non Terminal: %s\n", nonTerminalStrings[top->value.non_t]);*/
/*      ProductionRule *productionRule =*/
/*          PT->table[top->value.non_t][currentToken.token];*/
/*      if (productionRule == NULL) {*/
/*        continue;*/
/*        fprintf(stderr,*/
/*                "Error: No production rule found for non-terminal '%s' and "*/
/*                "token '%s' at line %d\n",*/
/*                nonTerminalStrings[top->value.non_t],*/
/*                terminalStrings[currentToken.token], currentToken.lineCount);*/
/*        return false;*/
/*      }*/
/**/
/*      RHSNode *currentRHS = productionRule->head;*/
/*      while (currentRHS != NULL) {*/
/*        StackNode *rhsNode = (StackNode *)malloc(sizeof(StackNode));*/
/*        if (rhsNode == NULL) {*/
/*          fprintf(stderr, "Memory allocation failed for StackNode.\n");*/
/*          exit(EXIT_FAILURE);*/
/*        }*/
/*        rhsNode->isT = currentRHS->isT;*/
/*        rhsNode->value = currentRHS->v;*/
/*        rhsNode->next = parsingStack;*/
/*        parsingStack = rhsNode;*/
/*        currentRHS = currentRHS->next;*/
/*      }*/
/*    } else {*/
/*      printf("Terminal: %s\n", terminalStrings[top->value.t]);*/
/*      if (top->value.t != currentToken.token) {*/
/*        fprintf(stderr, "Error: Unexpected token at '%s' at line %d\n",*/
/*                terminalStrings[currentToken.token], currentToken.lineCount);*/
/*        return false;*/
/*      }*/
/**/
/*      treeNode *newNode = (treeNode *)malloc(sizeof(treeNode));*/
/*      if (newNode == NULL) {*/
/*        fprintf(stderr, "Memory allocation failed for tree node.\n");*/
/*        exit(EXIT_FAILURE);*/
/*      }*/
/*      newNode->parent = currentParent;*/
/*      newNode->firstChild = NULL;*/
/*      newNode->rightSibling = NULL;*/
/*      newNode->isT = true;*/
/*      newNode->v.t = currentToken.token;*/
/*      if (currentParent->firstChild == NULL) {*/
/*        currentParent->firstChild = newNode;*/
/*      } else {*/
/*        treeNode *sibling = currentParent->firstChild;*/
/*        while (sibling->rightSibling != NULL) {*/
/*          sibling = sibling->rightSibling;*/
/*        }*/
/*        sibling->rightSibling = newNode;*/
/*      }*/
/*    }*/
/*    currentToken = getToken(fp);*/
/*  }*/
/*  if (parsingStack == NULL) {*/
/*    printf("Parsing Successful.\n");*/
/*    return true;*/
/*  } else {*/
/*    fprintf(stderr, "Error: Unexpected end of input\n");*/
/*    return false;*/
/*  }*/
/*}*/
/**/
/*void printParseTree(treeNode *node, int depth) {*/
/*  if (node == NULL) {*/
/*    return;*/
/*  }*/
/*  // Print current node*/
/*  printParseTree(node->firstChild, depth + 1);*/
/*  for (int i = 0; i < depth; i++) {*/
/*    printf("  "); // Indent according to depth*/
/*  }*/
/**/
/*  if (node->isT) {*/
/*    printf("Terminal: %s\n",*/
/*           terminalStrings[node->v.t]); // define function getTokenName This*/
/*                                        // function takes a token type*/
/*                                        // (enumeration value) as input and*/
/*                                        // returns a string representing the*/
/*                                        // name or description of that token*/
/*  } else {*/
/*    printf("Non-terminal: %s\n", nonTerminalStrings[node->v.non_t]);*/
/*  }*/
/**/
/*  // Print children recursively*/
/*  printParseTree(node->rightSibling,*/
/*                 depth); // Print right sibling at same depth*/
/*}*/
