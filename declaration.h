#pragma once

#include "lexicalAnalysis.h"
#include "symbolTable.h"
#include "statement.h"
#include "mcode.h"

extern void constDeclare(int lev);
extern void constDeclare(int lev, TAB_ELEMENT *tab);

extern TAB_ELEMENT* global_varOrFunc();
extern void global_varDeclare(SYMBOL_TYPE type);
extern void global_funcDeclare(TAB_ELEMENT *ele);

extern void local_varDeclare(TAB_ELEMENT *tab);
