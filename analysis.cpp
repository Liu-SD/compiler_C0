#include "main.h"
#include <iostream>

using namespace std;

void constDeclare(int lev) {
    if(sym != constsy)
        cout << "should be 'const'" << endl;
    nextSym();
    if(sym == intcon) {
        do {
            nextCh();
            if(sym != ident)
                cout << "should be ident" << endl;
            nextCh();
            if(sym != becomes)
                cout << "should be =" << endl;
            nextCh();
            if(sym != intsy)
                cout << "should be int" << endl;
            else {
                if(!lookup(token, NULL, 0))
                    enter(token, cons, t_int, 0, num, lev);
            }
        }while(sym == comma);
    } else if(sym == charcon) {

    } else
    cout << "should be 'int' or 'char'" << endl;
}
