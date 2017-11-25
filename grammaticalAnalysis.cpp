#include "grammaticalAnalysis.h"
#include <iostream>

using namespace std;

SYMBOL_TYPE global_type;
TAB_ELEMENT* global_tabelement;

void stat_A() {
    constDeclare(0);
}

void stat_B() {
    global_tabelement = global_varOrFunc();
}

void stat_C() {
    nextSym();
    global_tabelement = enter(token, func, t_void, 0, 0, 0);
}

void stat_D() {
    global_varDelclare(global_tabelement->type);
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
    global_tabelement = global_varOrFunc();
    if(global_tabelement->kind != func) cout << "no var after function declaration" << endl;
}

void stat_H() {
    // TODO
    nextSym();
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
        cout << char(status + 'A') << endl;
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
