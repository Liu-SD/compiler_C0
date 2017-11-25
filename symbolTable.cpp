#include "symbolTable.h"
#include <vector>
#include <string>
#include <iostream>
#include <string.h>

using namespace std;

vector<TAB_ELEMENT> global_tab;
vector<vector<TAB_ELEMENT> > local_tab;
int display = -1;
int offset = 0;

const int FUNC_OFFSET = 3;// area for return value, return address, previous $fp

vector<string> mcode;

TAB_ELEMENT* enter(char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev) {
    TAB_ELEMENT t;
    strcpy(t.ident, ident);
    t.kind = kind;
    t.type = type;
    t.value = value;
    t.length = length;
    t.func_display_num = -1;
    if(lev) local_tab[display].push_back(t);
    else global_tab.push_back(t);
    // if function, new display area
    if(kind == func) {
        t.func_display_num = ++display;
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
                element->func_display_num = iter->func_display_num;
            }
            return 1;
        }
    }
    return 0;
}

void show_tables() {
    cout << "global_table: " << endl;
    for(int i = 0; i < global_tab.size(); i++)
        cout << global_tab[i].ident << endl;
    cout << "local_table: " << endl;
    for (int i = 0; i < local_tab.size(); i++){
        cout << "display " << i << endl;
        for (int j = 0; j < local_tab[i].size(); j++)
            cout << local_tab[i][j].ident << endl;
    }
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


