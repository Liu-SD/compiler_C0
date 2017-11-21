#include "main.h"
#include <iostream>

using namespace std;

void constDeclare(int lev) {
    if(sym != constsy)
        cout << "should be 'const'" << endl;
    nextSym();
    if(sym == intsy) {
        do {
            nextSym();
            if(sym != ident) cout << "should be ident" << endl;
            nextSym();
            if(sym != becomes) cout << "should be =" << endl;
            nextSym();
            int signal = 1;
            if(sym == minuscon || sym == pluscon) {
                if(sym == minuscon)
                    signal = -1;
                nextSym();
            }
            if(sym != intcon) cout << "should be int" << endl;
            else {
                if(!lookup(token, lev, NULL))
                    enter(token, cons, t_int, 0, signal * num, lev);
                else
                    cout << "ident repeated" << endl;
                nextSym();
            }
        }while(sym == comma);
    } else if(sym == charsy) {
        do {
            nextSym();
            if(sym != ident) cout << "should be ident" << endl;
            nextSym();
            if(sym != becomes) cout <<"should be =" << endl;
            nextSym();
            if(sym != charcon) cout << "should be char" <<endl;
            else  {
                if(!lookup(token, lev, NULL))
                    enter(token, cons, t_char, 0, num, lev);
                else
                    cout << "ident repeated" << endl;
                nextSym();
            }
        }while (sym == comma);
    } else cout << "should be 'int' or 'char'" << endl;
    if(sym != semicolon)
        cout <<"should be ;" << endl;
    nextSym();
}
