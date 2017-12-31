#pragma once

#include "symbolTable.h"
#include "translate.h"

/*
�Ĵ��������ͼ��ɫ�㷨��
1. ���顣
2. ���ӡ�
3. ����use, def��
4. ����out��in��active
    out = U in
    in = use U (out - def)
    active = in U out = use U out
5. ����active�����ͻͼ����ɫ�㷨ʵ�֡�
    ͬһ����active�����ڵı���Ϊ��ͻ������
    �õ������Ĵ�����Ӧ��δ�ܷ��䵽�Ĵ����Ļ�Ծ�����������
6. ����Ĵ�����
    for each block:
        active�����еı���������5�з����������Ĵ�����
        def - out�����еı�����ʹ��active���ϱ���ʣ��Ĵ�����

- �ֲ��������Ĵ�����
- ���鲻��local_var_pool��
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
