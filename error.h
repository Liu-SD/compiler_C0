#pragma once

#include <vector>
// using namespace std;

extern int lc;
extern int cc;
extern char current_line[300];

struct ERROR {
    int errcode;
    int errline;
    int errpos;
    // char *errmsg;
    char linecontent[300];
};

const int SHUT_DOWN = 2;

static char *errormessages[] = {
    "illegal string format", // 0
    "no match symbol for current char", // 1

    "reach end of file", // 2
    "file not exist" // 3
};

extern std::vector<ERROR> errorlist;
extern void showerrormessages();
extern void error(int i);

extern void tmp_error(char *s);

extern void where(bool inout, char *s);
