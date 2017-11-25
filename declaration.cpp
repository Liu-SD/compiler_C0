#include "declaration.h"
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
        } while(sym == comma);
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
        } while (sym == comma);
    } else cout << "should be 'int' or 'char'" << endl;
    if(sym != semicolon)
        cout <<"should be ;" << endl;
    nextSym();
}

TAB_ELEMENT* global_varOrFunc() {
    SYMBOL_TYPE type = sym == intsy ? t_int : t_char;
    nextSym();
    if(sym != ident) cout << "should be ident" << endl;
    if(lookup(token, false, NULL)) cout << "ident repeated" << endl;
    nextSym();
    int length = 0;
    if(sym == lmedium) {
        nextSym();
        if(sym != intcon || num == 0) cout << "array length should be int and larger than 0" << endl;
        length = num;
        nextSym();
        if(sym != rmedium) cout << "should be rmedium" << endl;
        nextSym();
    }
    if(sym == comma || sym == semicolon)
        return enter(token, var, type, length, 0, 0);
    else if (sym == lsmall || sym == lbig)
        return enter(token, func, type, 0, 0, 0);
    else return NULL;
}

void global_varDelclare(SYMBOL_TYPE type) {
    while(sym == comma) {
        nextSym();
        if(sym != ident) cout << "should be ident" << endl;
        if(lookup(token, false, NULL)) cout << "ident repeated" << endl;
        nextSym();
        int length = 0;
        if(sym == lmedium) {
            nextSym();
            if(sym != intcon || num == 0) cout << "array length should be int and larger than 0" << endl;
            length = num;
            nextSym();
            if(sym != rmedium) cout << "should be rmedium" << endl;
            nextSym();
        }
        enter(token, var, type, length, 0, 0);
    }
    if(sym == semicolon)nextSym();
    else cout << "should end with semicolon" << endl;
}

