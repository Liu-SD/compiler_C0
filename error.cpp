#include "error.h"
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

vector<ERROR> errorlist;

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
