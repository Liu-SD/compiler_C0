#pragma once

#include "symbolTable.h"
#include "error.h"
#include "lexicalAnalysis.h"
#include "mcode.h"
#include <string>

extern void constDeclare(int lev, TAB_ELEMENT *tab);
extern void local_varDeclare(TAB_ELEMENT *tab);

extern void compoundStatement(TAB_ELEMENT *tab);
extern void statementArray(TAB_ELEMENT *tab);
extern void statement(TAB_ELEMENT *tab);

extern void ifStatement(TAB_ELEMENT *tab);
extern void whileStatement(TAB_ELEMENT *tab);
extern void switchStatement(TAB_ELEMENT *tab);
extern void scanfStatement(TAB_ELEMENT *tab);
extern void printfStatement(TAB_ELEMENT *tab);
extern void assignStatement(TAB_ELEMENT *tab);
extern void callStatement(TAB_ELEMENT *tab);
extern void returnStatement(TAB_ELEMENT *tab);

extern void expression(SYMBOL_TYPE &type, std::string &res);
extern void term(SYMBOL_TYPE &type, std::string &res);
extern void factor(SYMBOL_TYPE &type, std::string &res);
