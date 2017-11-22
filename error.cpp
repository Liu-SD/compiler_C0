#include "main.h"
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

vector<ERROR> errorlist;

const int SHUT_DOWN = 2;

char *errormessages[] = {
    "illegal string format", // 0
    "no match symbol for current char", // 1

    "reach end of file", // 2
    "file not exist" // 3
};

void showerrormessages() {
    printf("\nerror messages of program:\n");
    for(int i = 0; i < errorlist.size(); i++) {
        ERROR e = errorlist[i];
        printf("%2d : line: %d, position: %d \"%s\" %s\n", i, e.errline, e.errpos, e.linecontent, errormessages[e.errcode]);
    }
}



void error(int i) {
    ERROR err;
    err.errcode = i;
    err.errline = lc + 1;
    err.errpos = cc - 1;
    err.errmsg = NULL;
    strcpy(err.linecontent, current_line);
    errorlist.push_back(err);
    if(i >= SHUT_DOWN) {
        showerrormessages();
        exit(-1);
    }
    /*
    else {
        skip_current_line();
    }
    */
}
