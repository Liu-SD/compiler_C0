#pragma once

#include "error.h"

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

extern void set_file_stream(char * filepath);
// void nextCh();
extern void nextSym();
