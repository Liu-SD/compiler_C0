#pragma once

#include "symbolTable.h"
#include "translate.h"

using std::string;
using std::pair;
using std::set;
using std::map;
using std::vector;
using std::regex;
using std::smatch;

typedef struct blk_node {
    set<string> use;
    set<string> def;
    set<string> in;
    set<string> out;
    vector<string> code;
    string label;

    vector<blk_node*> precursor;
    vector<blk_node*> successor;
} *blk_link;

extern void coloring_translate(vector<pair<string, string>> mcode);
