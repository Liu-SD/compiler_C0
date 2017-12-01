#include "error.h"
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// using namespace std;

std::vector<ERROR> errorlist;

const bool debug = true;

void showerrormessages() {
    if(!errorlist.size())
        return;
    std::cout << std::endl << "error messages of program:" << std:: endl;
    for(int i = 0; i < errorlist.size(); i++) {
        ERROR e = errorlist[i];
        std::cout << i << "  line " << e.errline
                  << ", position " << e.errpos << std::endl
                  << e.linecontent << errormessages[e.errcode]
                  << std::endl << std::endl;
        // printf("%2d : line: %d, position: %d \"%s\" %s\n", i, e.errline, e.errpos, e.linecontent, errormessages[e.errcode]);
    }
}

void error(int i) {
    ERROR err;
    err.errcode = i;
    err.errline = lc + 1;
    err.errpos = cc - 1;
    strcpy(err.linecontent, current_line);
    errorlist.push_back(err);
    if(debug || i >= SHUT_DOWN) {
        showerrormessages();
        exit(-1);
    }
}



void where(bool inout, char *s) {
    //return;
    static int layer = 0;
    using namespace std;
    if(inout) {
        for (int i = 0; i < layer; i++)cout << "  ";
        cout << "Enter <" << s << ">" << endl;
        layer++;
    } else {
        layer--;
        for (int i = 0; i <layer; i++) cout << "  ";
        cout << "Leave <" << s << ">" << endl;
    }
}
