#pragma once

#include "lexicalAnalysis.h"
#include "symbolTable.h"

extern void constDeclare(int lev);

extern TAB_ELEMENT* global_varOrFunc();
extern void global_varDelclare(SYMBOL_TYPE type);
