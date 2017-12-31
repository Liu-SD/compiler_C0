#pragma once

#include "symbolTable.h"
#include "translate.h"

/*
寄存器分配的图着色算法：
1. 划块。
2. 连接。
3. 计算use, def。
4. 计算out，in，active
    out = U in
    in = use U (out - def)
    active = in U out = use U out
5. 根据active构造冲突图，着色算法实现。
    同一个块active集合内的变量为冲突变量。
    得到变量寄存器对应表。未能分配到寄存器的活跃变量不填如表。
6. 分配寄存器。
    for each block:
        active集合中的变量：根据5中分析结果分配寄存器。
        def - out集合中的变量，使用active集合变量剩余寄存器。

- 局部变量进寄存器。
- 数组不进local_var_pool。
*/


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
    set<string> active; // active = in U out = use U out
    vector<string> code;
    string label;
    string jump_label;
    string branch_label;
    bool ret_blk = false;

    set<blk_node*> precursor;
    set<blk_node*> successor;
    bool inUpdate;

    map<string, int> register_distribute;
} *blk_link;

extern void coloring_translate(vector<pair<string, string>> mcode);

extern vector<blk_link> blk_list;
extern vector<TAB_ELEMENT> ltab;
extern void to_tcode(string func_name);
extern void printTcode_coloring(string path);
