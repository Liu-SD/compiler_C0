#pragma once

#include <vector>
#include <stdlib.h>
using namespace std;

// wordsAnalysis.cpp
typedef enum {ident = 0,
              intcon, intsy, charcon, charsy, stringcon, voidsy, constsy,
              pluscon, minuscon, timescon, divcon, becomes,
              eql, neq, gtr, grq, lss, leq,
              lsmall, rsmall, lmedium, rmedium, lbig, rbig,
              comma, semicolon, colon,
              ifsy, elsesy, whilesy, switchsy, casesy, defaultsy, mainsy,
              returnsy, scanfsy, printfsy,
             } SYMBOL;

static char* SYMBOL_STRING[] = {"ident", "intcon", "instsy", "charcon",
                                "charsy", "stringcon", "voidsy", "constsy",
                                "pluscon", "minuscon", "timescon", "divcon",
                                "becomes", "eql", "neq", "gtr", "grq", "lss",
                                "leq", "lsmall", "rsmall", "lmedium", "rmedium",
                                "lbig", "rbig", "comma", "semicolon", "colon",
                                "ifsy", "elsesy", "whilesy", "switchsy", "casesy",
                                "defaultsy", "mainsy", "returnsy", "scanfsy", "printfsy"
                               };

extern int ll;
extern int lc;
extern int cc;
extern int num;
extern char ch;
extern SYMBOL sym;
extern char token[30];
extern char stringbuff[500];
extern char current_line[300];
const int keywordCount = 14;
static char* keywords[] = {
    "if", "else", "while", "switch", "case", "default", "scanf", "printf",
    "void", "main", "int", "char", "return", "const"
};
static SYMBOL keySet[] = {
    ifsy, elsesy, whilesy, switchsy, casesy, defaultsy, scanfsy, printfsy,
    voidsy, mainsy, intsy, charsy, returnsy, constsy
};

void set_file_stream(char * filepath);
// void nextCh();
void skip_current_line();
void nextSym();

// error.cpp
struct ERROR {
    int errcode;
    int errline;
    int errpos;
    char *errmsg;
    char linecontent[300];
};
extern vector<ERROR> errorlist;
void showerrormessages();
void error(int i);


// symbolTab.cpp
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
};

extern vector<TAB_ELEMENT> global_tab;
extern vector<vector<TAB_ELEMENT> > local_tab;

TAB_ELEMENT enter(char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev);

int lookup(char *ident, int local_flag, TAB_ELEMENT *element);

// analysis.cpp
void constDeclare(int lev);
