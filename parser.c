#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
#include <stdlib.h>

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
// Function to create and populate a gitem for terminal
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
  ;
  if (!rhsNode)
    printf("Failed to allocate memory for RHS Node\n");
  else {
    rhsNode->next = NULL;
    rhsNode->ptr = NULL;
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

void addrule(int index, nonTerminals nt, int size, gitems value[]) {
  LHSNode *lhsNode;
  if (G->rules[index - 1] == NULL) {
    lhsNode = createLHSNode();
    lhsNode->lhs = nt;
  } else {
    lhsNode = G->rules[index - 1];
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
    G->rules[index - 1] = lhsNode;
    return;
  }
  if (currentrulehead->next_rule != NULL) {
    while (currentrulehead->next_rule != NULL)
      currentrulehead = currentrulehead->next_rule;
  }
  currentrulehead->next_rule = newRule;

  // Add the LHSNode to the grammar
  G->rules[index - 1] = lhsNode;
}

void addGrammarRules() {
  gitems i1[2] = {{false, {.non_t = otherFunctions}},
                  {false, {.non_t = mainFunction}}};
  addrule(1, program, 2, i1);

  gitems i2[3] = {
      {true, {TK_MAIN}}, {false, {.non_t = stmts}}, {true, {TK_END}}};
  addrule(2, mainFunction, 3, i2);

  gitems i3a[2] = {{false, {.non_t = function}},
                   {false, {.non_t = otherFunctions}}};
  addrule(3, otherFunctions, 2, i3a);

  gitems i3b[1] = {{true, {EPS}}};
  addrule(3, otherFunctions, 1, i3b);

  gitems i4[6] = {{true, {TK_FUNID}},
                  {false, {.non_t = input_par}},
                  {false, {.non_t = output_par}},
                  {true, {TK_SEM}},
                  {false, {.non_t = stmts}},
                  {true, {TK_END}}};
  addrule(4, function, 6, i4);

  gitems i5[6] = {{true, {TK_INPUT}},
                  {true, {TK_PARAMETER}},
                  {true, {TK_LIST}},
                  {true, {TK_SQL}},
                  {false, {.non_t = parameter_list}},
                  {true, {TK_SQR}}};
  addrule(5, input_par, 6, i5);

  gitems i6a[6] = {{true, {TK_OUTPUT}},
                   {true, {TK_PARAMETER}},
                   {true, {TK_LIST}},
                   {true, {TK_SQL}},
                   {false, {.non_t = parameter_list}},
                   {true, {TK_SQR}}};
  addrule(6, output_par, 6, i6a);

  gitems i6b[1] = {{true, {EPS}}};
  addrule(6, output_par, 1, i6b);

  gitems i7[3] = {{false, {.non_t = dataType}},
                  {true, {TK_ID}},
                  {false, {.non_t = remaining_list}}};
  addrule(7, parameter_list, 3, i7);

  gitems i8a[1] = {{false, {.non_t = primitiveDatatype}}};
  addrule(8, dataType, 1, i8a);

  gitems i8b[1] = {{false, {.non_t = constructedDatatype}}};
  addrule(8, dataType, 1, i8b);

  gitems i9a[1] = {{true, {TK_INT}}};
  addrule(9, primitiveDatatype, 1, i9a);

  gitems i9b[1] = {{true, {TK_REAL}}};
  addrule(9, primitiveDatatype, 1, i9b);

  gitems i10a[2] = {{true, {TK_RECORD}}, {true, {TK_RUID}}};
  addrule(10, constructedDatatype, 2, i10a);

  gitems i10b[2] = {{true, {TK_UNION}}, {true, {TK_RUID}}};
  addrule(10, constructedDatatype, 2, i10b);

  gitems i10c[1] = {{true, {TK_RUID}}};
  addrule(10, constructedDatatype, 1, i10c);

  gitems i11a[2] = {{true, {TK_COMMA}}, {false, {.non_t = parameter_list}}};
  addrule(11, remaining_list, 2, i11a);

  gitems i11b[1] = {{true, {EPS}}};
  addrule(11, remaining_list, 1, i11b);

  gitems i12[4] = {{false, {.non_t = typeDefinitions}},
                   {false, {.non_t = declarations}},
                   {false, {.non_t = otherStmts}},
                   {false, {.non_t = returnStmt}}};
  addrule(12, stmts, 4, i12);

  gitems i13a1[2] = {{false, {.non_t = actualOrRedefined}},
                     {false, {.non_t = typeDefinitions}}};
  addrule(13, typeDefinitions, 2, i13a1);

  gitems i13a2[1] = {{true, {EPS}}};
  addrule(13, typeDefinitions, 1, i13a2);

  gitems i13b1[1] = {{false, {.non_t = typeDefinition}}};
  addrule(14, actualOrRedefined, 1, i13b1);

  gitems i13b2[1] = {{false, {.non_t = definetypestmt}}};
  addrule(14, actualOrRedefined, 1, i13b2);

  gitems i14[4] = {{true, {TK_RECORD}},
                   {true, {TK_RUID}},
                   {false, {.non_t = fieldDefinitions}},
                   {true, {TK_ENDRECORD}}};
  addrule(15, typeDefinition, 4, i14);

  gitems i15[4] = {{true, {TK_UNION}},
                   {true, {TK_RUID}},
                   {false, {.non_t = fieldDefinitions}},
                   {true, {TK_ENDUNION}}};
  addrule(15, typeDefinition, 4, i15);

  gitems i16[3] = {{false, {.non_t = fieldDefinition}},
                   {false, {.non_t = fieldDefinition}},
                   {false, {.non_t = moreFields}}};
  addrule(16, fieldDefinitions, 3, i16);

  gitems i17a[5] = {{true, {TK_TYPE}},
                    {false, {.non_t = fieldtype}},
                    {true, {TK_COLON}},
                    {true, {TK_FIELDID}},
                    {true, {TK_SEM}}};
  addrule(17, fieldDefinition, 5, i17a);

  gitems i17b[1] = {{false, {.non_t = primitiveDatatype}}};
  addrule(18, fieldtype, 1, i17b);

  gitems i17c[1] = {{false, {.non_t = constructedDatatype}}};
  addrule(18, fieldtype, 1, i17c);

  gitems i18a[2] = {{false, {.non_t = fieldDefinition}},
                    {false, {.non_t = moreFields}}};
  addrule(19, moreFields, 2, i18a);

  gitems i18b[1] = {{true, {EPS}}};
  addrule(19, moreFields, 1, i18b);

  gitems i19a[2] = {{false, {.non_t = declaration}},
                    {false, {.non_t = declarations}}};
  addrule(20, declarations, 2, i19a);

  gitems i19b[1] = {{true, {EPS}}};
  addrule(20, declarations, 1, i19b);

  gitems i20[6] = {
      {true, {TK_TYPE}}, {false, {.non_t = dataType}},      {true, {TK_COLON}},
      {true, {TK_ID}},   {false, {.non_t = global_or_not}}, {true, {TK_SEM}}};
  addrule(21, declaration, 6, i20);

  gitems i21a[2] = {{true, {TK_COLON}}, {true, {TK_GLOBAL}}};
  addrule(22, global_or_not, 2, i21a);

  gitems i21b[1] = {{true, {EPS}}};
  addrule(22, global_or_not, 1, i21b);

  gitems i22a[2] = {{false, {.non_t = stmt}}, {false, {.non_t = otherStmts}}};
  addrule(23, otherStmts, 2, i22a);

  gitems i22b[1] = {{true, {EPS}}};
  addrule(23, otherStmts, 1, i22b);

  gitems i23a[1] = {{false, {.non_t = assignmentStmt}}};
  addrule(24, stmt, 1, i23a);

  gitems i23b[1] = {{false, {.non_t = iterativeStmt}}};
  addrule(24, stmt, 1, i23b);

  gitems i23c[1] = {{false, {.non_t = conditionalStmt}}};
  addrule(24, stmt, 1, i23c);

  gitems i23d[1] = {{false, {.non_t = ioStmt}}};
  addrule(24, stmt, 1, i23d);

  gitems i23e[1] = {{false, {.non_t = funCallStmt}}};
  addrule(24, stmt, 1, i23e);

  gitems i24[4] = {{false, {.non_t = singleOrRecId}},
                   {true, {TK_ASSIGNOP}},
                   {false, {.non_t = arithmeticExpression}},
                   {true, {TK_SEM}}};
  addrule(25, assignmentStmt, 4, i24);

  gitems i25a[2] = {{true, {TK_ID}},
                    {false, {.non_t = option_single_constructed}}};
  addrule(26, singleOrRecId, 2, i25a);

  gitems i25b[3] = {{true, {EPS}},
                    {false, {.non_t = oneExpansion}},
                    {false, {.non_t = moreExpansions}}};
  addrule(27, option_single_constructed, 3, i25b);

  gitems i25c[2] = {{true, {TK_DOT}}, {true, {TK_FIELDID}}};
  addrule(28, oneExpansion, 2, i25c);

  gitems i25d[2] = {{false, {.non_t = oneExpansion}},
                    {false, {.non_t = moreExpansions}}};
  addrule(29, moreExpansions, 2, i25d);

  gitems i25e[1] = {{true, {EPS}}};
  addrule(29, moreExpansions, 1, i25e);

  gitems i26[7] = {{false, {.non_t = outputParameters}},
                   {true, {TK_CALL}},
                   {true, {TK_FUNID}},
                   {true, {TK_WITH}},
                   {true, {TK_PARAMETERS}},
                   {false, {.non_t = inputParameters}},
                   {true, {TK_SEM}}};
  addrule(30, funCallStmt, 7, i26);

  gitems i27a[5] = {{true, {TK_SQL}},
                    {false, {.non_t = idList}},
                    {true, {TK_SQR}},
                    {true, {TK_ASSIGNOP}},
                    {true, {EPS}}};
  addrule(31, outputParameters, 5, i27a);

  gitems i27b[1] = {{true, {EPS}}};
  addrule(31, outputParameters, 1, i27b);

  gitems i28[3] = {
      {true, {TK_SQL}}, {false, {.non_t = idList}}, {true, {TK_SQR}}};
  addrule(32, inputParameters, 3, i28);

  gitems i29[7] = {{true, {TK_WHILE}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}},
                   {false, {.non_t = stmt}},
                   {false, {.non_t = otherStmts}},
                   {true, {TK_ENDWHILE}}};
  addrule(33, iterativeStmt, 7, i29);

  gitems i30[8] = {{true, {TK_IF}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}},
                   {true, {TK_THEN}},
                   {false, {.non_t = stmt}},
                   {false, {.non_t = otherStmts}},
                   {false, {.non_t = elsePart}}};
  addrule(34, conditionalStmt, 8, i30);

  gitems i31a[4] = {{true, {TK_ELSE}},
                    {false, {.non_t = stmt}},
                    {false, {.non_t = otherStmts}},
                    {true, {TK_ENDIF}}};
  addrule(35, elsePart, 4, i31a);

  gitems i31b[1] = {{true, {TK_ENDIF}}};
  addrule(35, elsePart, 1, i31b);

  gitems i32a[5] = {{true, {TK_READ}},
                    {true, {TK_OP}},
                    {false, {.non_t = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(36, ioStmt, 5, i32a);

  gitems i32b[5] = {{true, {TK_WRITE}},
                    {true, {TK_OP}},
                    {false, {.non_t = var}},
                    {true, {TK_CL}},
                    {true, {TK_SEM}}};
  addrule(36, ioStmt, 5, i32b);

  gitems i33a[2] = {{false, {.non_t = term}}, {false, {.non_t = expPrime}}};
  addrule(37, arithmeticExpression, 2, i33a);

  gitems i33b1[3] = {{false, {.non_t = lowPrecedenceOperators}},
                     {false, {.non_t = term}},
                     {false, {.non_t = expPrime}}};
  addrule(38, expPrime, 3, i33b1);

  gitems i33b2[1] = {{true, {EPS}}};
  addrule(38, expPrime, 1, i33b2);

  gitems i33c[2] = {{false, {.non_t = factor}}, {false, {.non_t = termPrime}}};
  addrule(39, term, 2, i33c);

  gitems i34a[3] = {{false, {.non_t = highPrecedenceOperators}},
                    {false, {.non_t = factor}},
                    {false, {.non_t = termPrime}}};
  addrule(40, termPrime, 3, i34a);

  gitems i34b[1] = {{true, {EPS}}};
  addrule(40, termPrime, 1, i34b);

  gitems i35a1[3] = {{true, {TK_OP}},
                     {false, {.non_t = arithmeticExpression}},
                     {true, {TK_CL}}};
  addrule(41, factor, 3, i35a1);

  gitems i35a2[1] = {{false, {.non_t = var}}};
  addrule(41, factor, 1, i35a2);

  gitems i35b1[1] = {{true, {TK_MUL}}};
  addrule(42, highPrecedenceOperators, 1, i35b1);

  gitems i35b2[1] = {{true, {TK_DIV}}};
  addrule(42, highPrecedenceOperators, 1, i35b2);

  gitems i35c1[1] = {{true, {TK_PLUS}}};
  addrule(43, lowPrecedenceOperators, 1, i35c1);

  gitems i35c2[1] = {{true, {TK_MINUS}}};
  addrule(43, lowPrecedenceOperators, 1, i35c2);

  gitems i36[7] = {{true, {TK_OP}}, {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}, {false, {.non_t = logicalOp}},
                   {true, {TK_OP}}, {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(44, booleanExpression, 7, i36);

  gitems i37[3] = {{false, {.non_t = var}},
                   {false, {.non_t = relationalOp}},
                   {false, {.non_t = var}}};
  addrule(44, booleanExpression, 3, i37);

  gitems i38[4] = {{true, {TK_NOT}},
                   {true, {TK_OP}},
                   {false, {.non_t = booleanExpression}},
                   {true, {TK_CL}}};
  addrule(44, booleanExpression, 4, i38);

  gitems i39a[1] = {{false, {.non_t = singleOrRecId}}};
  addrule(45, var, 1, i39a);

  gitems i39b[1] = {{true, {TK_NUM}}};
  addrule(47, var, 1, i39b);

  gitems i39c[1] = {{true, {TK_RNUM}}};
  addrule(45, var, 1, i39c);

  gitems i40a[1] = {{true, {TK_AND}}};
  addrule(46, logicalOp, 1, i40a);

  gitems i40b[1] = {{true, {TK_OR}}};
  addrule(46, logicalOp, 1, i40b);

  gitems i41a[1] = {{true, {TK_LT}}};
  addrule(47, relationalOp, 1, i41a);

  gitems i41b[1] = {{true, {TK_LE}}};
  addrule(47, relationalOp, 1, i41b);

  gitems i41c[1] = {{true, {TK_EQ}}};
  addrule(47, relationalOp, 1, i41c);

  gitems i41d[1] = {{true, {TK_GT}}};
  addrule(47, relationalOp, 1, i41d);

  gitems i41e[1] = {{true, {TK_GE}}};
  addrule(47, relationalOp, 1, i41e);

  gitems i41f[1] = {{true, {TK_NE}}};
  addrule(47, relationalOp, 1, i41f);

  gitems i42[3] = {{true, {TK_RETURN}},
                   {false, {.non_t = optionalReturn}},
                   {true, {TK_SEM}}};
  addrule(48, returnStmt, 3, i42);

  gitems i43a[3] = {
      {true, {TK_SQL}}, {false, {.non_t = idList}}, {true, {TK_SQR}}};
  addrule(49, optionalReturn, 3, i43a);

  gitems i43b[1] = {{true, {EPS}}};
  addrule(49, optionalReturn, 1, i43b);

  gitems i44[2] = {{true, {TK_ID}}, {false, {.non_t = more_ids}}};
  addrule(50, idList, 2, i44);

  gitems i45a[2] = {{true, {TK_COMMA}}, {false, {.non_t = idList}}};
  addrule(51, more_ids, 2, i45a);

  gitems i45b[1] = {{true, {EPS}}};
  addrule(51, more_ids, 1, i45b);

  gitems i46[5] = {{true, {TK_DEFINETYPE}},
                   {false, {.non_t = A}},
                   {true, {TK_RUID}},
                   {true, {TK_AS}},
                   {true, {TK_RUID}}};
  addrule(52, definetypestmt, 5, i46);

  gitems i47a[1] = {{true, {TK_RECORD}}};
  addrule(53, A, 1, i47a);

  gitems i47b[1] = {{true, {TK_UNION}}};
  addrule(53, A, 1, i47b);
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
    if (!current_lhs)
      continue;
    ProductionRule *current_rule = current_lhs->rules;
    while (current_rule != NULL) {
      RHSNode *rhs_node = current_rule->head;
      printf("rhs_node\n");
      if (!rhs_node) {
        printf("rhs_node\n");
        exit(1);
      }
      if (rhs_node->isT) {
        if (rhs_node->v.t == EPS) {
          terminal_node *first_terminal =
              first_follow_sets[rhs_node->v.non_t].follow_set->head;
          while (first_terminal != NULL) {
            PT->table[i][first_terminal->t] = current_rule;
            first_terminal = first_terminal->next;
          }
        }
        PT->table[i][rhs_node->v.t] = current_rule;
      } else {
        terminal_node *first_terminal =
            first_follow_sets[rhs_node->v.non_t].first_set->head;
        while (first_terminal != NULL) {
          PT->table[i][first_terminal->t] = current_rule;
          first_terminal = first_terminal->next;
        }
      }
      current_rule = current_rule->next_rule;
    }
  }
}

terminal_list *compute_first_for_rhsnode(RHSNode *rhs,
                                         terminal_list *firstset) {
  if (rhs == NULL) {
    return firstset;
  }

  if (rhs->isT) {
    /*printf("terminal: %s\n", terminalStrings[rhs->v.t]);*/
    add_terminal_tolist(firstset, rhs->v.t);
  } else {
    /*printf("non terminal: %s\n", nonTerminalStrings[rhs->v.non_t]);*/
    compute_first(rhs->v.non_t);
    join_terminal_list(firstset, first_follow_sets[rhs->v.non_t].first_set);

    if (contains_epsilon(first_follow_sets[rhs->v.non_t].first_set) &&
        rhs->next != NULL) {
      remove_epsilon(first_follow_sets[rhs->v.non_t].first_set);
      /*printf("remove_epsilon for %s\n", nonTerminalStrings[rhs->v.non_t]);*/
      join_terminal_list(firstset,
                         compute_first_for_rhsnode(rhs->next, firstset));
    }
  }
  return firstset;
}

void compute_first(nonTerminals given_nt) {
  printf("compute_first started for %s\n", nonTerminalStrings[given_nt]);
  if (first_follow_sets[given_nt].first_set == NULL) {
    first_follow_sets[given_nt].first_set = createTerminalList();
  } else if (first_follow_sets[given_nt].first_set->head != NULL) {
    return;
  }

  LHSNode *lhs = G->rules[given_nt];
  for (ProductionRule *rule = lhs->rules; rule != NULL;
       rule = rule->next_rule) {
    compute_first_for_rhsnode(rule->head,
                              first_follow_sets[given_nt].first_set);
  }
  printf("compute_first ended for %s\n", nonTerminalStrings[given_nt]);
}

void compute_firsts() {
  for (int i = 0; i < nonTerminalCount; i++) {
    printf("%s\n", nonTerminalStrings[i]);
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

int main() {
  G = (Grammar *)malloc(sizeof(Grammar));
  addGrammarRules();
  printf("Grammar rules added\n");
  create_parse_table();
  printf("Created parse table\n");
  /*compute_firsts();*/
  compute_first(typeDefinition);
  /*initiate_parse_table();*/
}
