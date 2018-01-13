#pragma once

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern int lc;
extern int cc;
extern char current_line[5000];


const bool debug = false;


struct ERROR {
    int errcode;
    int errline;
    int errpos;
    char linecontent[5000];
};

const int SHUT_DOWN = 37;

static char *errormessages[] = {
    "illegal string format", // 0
    "no match symbol for current char", // 1
    "can't declare variable after function declaration", // 2
    "program start with const or var or func declaration", // 3
    "missing `,` or `;` if it's var declaration, missing `(` or `{` if it's func declaration", // 4
    "should be function name after `void`", // 5
    "declaration start with `void`, `char`, `int`", // 6
    "missing `(` or `{`", // 7
    "should be `const`", // 8
    "should be identity", // 9
    "should be `=`", // 10
    "should be int number and larger than 0", // 11
    "ident repeated or invalid", // 12
    "should be char", // 13
    "should be `int` or `char`", // 14
    "should be `;`", // 15
    "should be `]`", // 16
    "should be `)`", // 17
    "should be `{`", // 18
    "can't be left value", // 19
    "identity not defined", // 20
    "should be `}`", // 21
    "should be `(`", // 22
    "missing `else`", // 23
    "missing `case`", // 24
    "should be `:`", // 25
    "parameter not matched", // 26
    "return statement not exist", // 27
    "should't be array", // 28
    "should be array", // 29
    "return type not matched", // 30
    "should be int", // 31
    "should be `[`", // 32
    "can't call void function in expression", // 33
    "switch type not matched", // 34
    "main is not callable", // 35
    "index out of range", // 36


    "reach end of file", // 37
    "file not exist" // 38
};

extern std::vector<ERROR> errorlist;
extern void showerrormessages();
extern void error(int i);

extern void where(bool inout, char *s);
