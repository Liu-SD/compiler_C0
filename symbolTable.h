#pragma once

#include <vector>

using namespace std;

typedef enum {
    var, para, func, cons
} SYMBOL_KIND;

typedef enum {
    t_void, t_char, t_int
} SYMBOL_TYPE;

struct TAB_ELEMENT {
    char ident[30]; // name of ident
    SYMBOL_TYPE type;
    SYMBOL_KIND kind;
    int length; // 0 if not array
    int value;
    int func_display_num;
};

extern vector<TAB_ELEMENT> global_tab;
extern vector<vector<TAB_ELEMENT> > local_tab;

extern TAB_ELEMENT* enter(char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev);

extern int lookup(char *ident, int local_flag, TAB_ELEMENT *element);
