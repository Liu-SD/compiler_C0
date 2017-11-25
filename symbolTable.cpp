#include "symbolTable.h"
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

vector<TAB_ELEMENT> global_tab;
vector<vector<TAB_ELEMENT> > local_tab;
int display = -1;
int offset = 0;

const int FUNC_OFFSET = 3;// area for return value, return address, previous $fp

vector<string> mcode;

ostream &operator << (ostream &out, TAB_ELEMENT ele) {
    const char *k = ele.kind == var ? " var" :
        ele.kind == func ? "func" :
            ele.kind == cons ? "cons" : "para";
    const char *t = ele.type == t_int ? " int" :
        ele.type == t_char ? "char" : "void";
    char s[60];
    sprintf(s, "|%10s|\t%s|\t%s|\t%3d|", ele.ident, k, t, ele.length);
    out << s;
    return out;
}


TAB_ELEMENT* enter(char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev) {
    TAB_ELEMENT t;
    strcpy(t.ident, ident);
    t.kind = kind;
    t.type = type;
    t.value = value;
    t.length = length;
    if(lev) local_tab[display].push_back(t);
    else global_tab.push_back(t);
    // if function, new display area
    if(kind == func) {
        t.value = ++display;
        local_tab.push_back(vector<TAB_ELEMENT>());
        offset = FUNC_OFFSET;
    }

    // if parameter or variable, add offset
    if(kind == para || kind == var) {
        t.value = offset;
        if(length) offset += length;
        else ++offset;
    }
    return &t;
}

int lookup(char *ident, int local_flag, TAB_ELEMENT *element) {
    vector<TAB_ELEMENT>::iterator head, tail;
    if(local_flag) {
        head = local_tab[display].begin();
        tail = local_tab[display].end();
    } else {
        head = global_tab.begin();
        tail = global_tab.end();
    }
    for(vector<TAB_ELEMENT>::iterator iter = head; iter != tail; iter++) {
        if(!strcmp(iter->ident, ident)) {
            if(element) {
                element->kind = iter->kind;
                element->type = iter->type;
                strcpy(element->ident, iter->ident);
                element->value = iter->value;
                element->length = iter->length;
            }
            return 1;
        }
    }
    return 0;
}

void show_tables() {
    char tab_head[60];
    sprintf(tab_head, "|%10s|\t%4s|\t%4s|\t%3s|", "ident", "kind", "type", "len");
    cout << endl << "global_table: " << endl;
    cout << " ---------------------------------- " << endl;
    cout << tab_head << endl;
    cout << " ---------------------------------- " << endl;
    for(int i = 0; i < global_tab.size(); i++)
        cout << global_tab[i] << endl;
    cout << " ---------------------------------- " << endl << endl << "local_table: " << endl;
    cout << " ---------------------------------- " << endl;
    cout << tab_head << endl;
    for (int i = 0; i < local_tab.size(); i++){
        cout << " -----------------" << i << "---------------- " << endl;
        for (int j = 0; j < local_tab[i].size(); j++)
            cout << local_tab[i][j] << endl;
    }
    cout << " ---------------------------------- " << endl;
}

/*
about offset:
        .
        .
        .
|   return value  |
 -----------------
 -----------------
|                 |
|operate num stack|
 -----------------
|                 |
| local variables |
 -----------------
|                 |
|   parameters    |
 -----------------
|    prev $fp     |
 -----------------
| return address  |
 -----------------
|  return value   |
 -----------------
 -----------------
|                 |
| global variable |
 -----------------
*/


