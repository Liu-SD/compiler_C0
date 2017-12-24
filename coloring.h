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
using std::cout;
using std::endl;

typedef struct blk_node {
    set<string> use;
    set<string> def;
    set<string> in;
    set<string> out;
    vector<string> code;
    string label;
    string jump_label;
    string branch_label;
    bool ret_blk = false;

    set<blk_node*> precursor;
    set<blk_node*> successor;
    bool inUpdate;
} *blk_link;

extern void coloring_translate(vector<pair<string, string>> mcode);
