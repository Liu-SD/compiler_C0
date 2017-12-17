#pragma once

#include "symbolTable.h"
#include "mcode.h"
#include "translate.h"

#include <cassert>
#include <regex>
#include <set>


using std::string;
using std::vector;
using std::map;
using std::set;
using std::regex;
using std::pair;
using std::smatch;

extern vector<pair<string, string>> optimize_codes;
extern set<string> reserved_var;

typedef enum { _null, _add, _sub, _mul, _div, _arr_get } optype;

struct quadruple {
	string label;
	string code;
	string value;
	string left;
	string right;
	optype op;
};

typedef struct dag_node {
	bool inQ = false;
	bool leaf;
	string leaf_var_name;
	optype op;
	dag_node* left;
	dag_node* right;
	vector<dag_node*> parent;
} *dag_link;



extern void dag_optimize();

extern void printMcode_optimized();
