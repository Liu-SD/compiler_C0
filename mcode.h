#pragma once

#include <map>
#include <fstream>

#include "symbolTable.h"


extern std::string int2str(int i);

extern std::map<std::string, int> label_tab;
extern std::vector<std::string> code_tab;

extern int mcode_lc();

extern std::string newLabel();
extern int setLabelVal(std::string label, int val);
extern std::string newTmpVar();
extern void emit(std::string z, std::string x, std::string op, std::string y); // z = x op y
extern void emit(std::string x, std::string y, std::string z); // compare y label
extern void emit(std::string x, std::string y); // push y    call y
extern void emit(std::string x); // mark stack

extern void showMcode();

typedef std::vector<std::pair<std::string, std::string>> EMBEDTAB;

extern EMBEDTAB embeddingLabel();

extern void printMcode(std::string path);
