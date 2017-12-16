#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <set>

typedef enum {
    var, para, func, cons
} SYMBOL_KIND;

typedef enum {
    t_void, t_char, t_int, t_tmp
} SYMBOL_TYPE;

struct TAB_ELEMENT {
    char ident[30]; // name of ident
    SYMBOL_TYPE type;
    SYMBOL_KIND kind;
    int length; // 0 if not array
    int value;

    friend std::ostream &operator << (std::ostream &out, TAB_ELEMENT ele);
};

extern std::vector<TAB_ELEMENT> global_tab;
extern std::vector<std::vector<TAB_ELEMENT> > local_tab;


extern TAB_ELEMENT* enter(const char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev);

extern int lookup(char *ident, int local_flag, TAB_ELEMENT *element);

extern void show_tables();


// delete var that not show in reserved_var;
// add var to global_tab if it's in reserved_var but not in symbol_table
// change offset
extern void update_symbol_table(std::set<std::string> reserved_var);
