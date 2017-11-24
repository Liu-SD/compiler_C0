#include "grammaticalAnalysis.h"
#include <iostream>

using namespace std;

SYMBOL_TYPE type_from_B;

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


void stat_A() {
    constDeclare(0);
}

void stat_B() {
    type_from_B = sym == intsy ? t_int : t_char;
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
        enter(token, var, type_from_B, length, 0, 0);
    else if (sym == lsmall || sym == lbig)
        enter(token, func, type_from_B, 0, 0, 0);
}

void stat_C() {
    nextSym();
}

void stat_D() {
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
        enter(token, var, type_from_B, length, 0, 0);
    }
    if(sym == semicolon)nextSym();
    else cout << "should end with semicolon" << endl;
}

void stat_E() {
    // TODO
    if(sym == lsmall) {
        while(sym != rsmall)nextSym();
    }
    nextSym();
    if(sym != lbig) cout << "error here" << endl;
    int i = 1;
    while(i) {
        nextSym();
        if(sym == lbig) i++;
        if(sym == rbig) i--;
    }
    nextSym();
}

void stat_F() {
    nextSym();
}

void stat_G() {
    // TODO
    cout << SYMBOL_STRING[sym] << endl;
    nextSym();
    cout << SYMBOL_STRING[sym] << endl;
    nextSym();
    cout << SYMBOL_STRING[sym] << endl;
}

void stat_H() {
    nextSym();
    nextSym();
    int i = 1;
    while(i) {
        nextSym();
        if(sym == lbig) i++;
        if(sym == rbig) i--;
    }
}

void NFA_program() {
    typedef enum {A, B, C, D, E, F, G, H} STAT;
    STAT status = A;
    while(status != H) {
        switch (status) {
        case A:

            if(sym == constsy) {
                status = A;
                stat_A();
            } else if(sym == voidsy) {
                status = C;
                stat_C();
            } else if(sym == intsy || sym == charsy) {
                status = B;
                stat_B();
            } else cout << "error in status A" << endl;
            break;
        case B:
            if(sym == comma || sym == semicolon) {
                status = D;
                stat_D();
            } else if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else cout << "error in status B" << endl;
            break;
        case C:
            if(sym == mainsy) {
                status = H;
                stat_H();
            } else if(sym == ident) {
                status = F;
                stat_F();
            } else cout << "error in status C" << endl;
            break;
        case D:
            if(sym == voidsy) {
                status = C;
                stat_C();
            } else if(sym == intsy || sym == charsy) {
                status = B;
                stat_B();
            } else cout << "error in status D" << endl;
            break;
        case E:
            if(sym == voidsy) {
                status = C;
                stat_C();
            } else if(sym == intsy || charsy) {
                status = G;
                stat_G();
            } else cout << "error in status E" << endl;
            break;
        case F:
            if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else cout << "error in status F" << endl;
            break;
        case G:
            if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else cout << "error in status G" << endl;
        }
    }
}
