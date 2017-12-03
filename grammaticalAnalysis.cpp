#include "grammaticalAnalysis.h"
#include <iostream>

using namespace std;

/*
global_tabelement指针指向状态B或状态F中读到并放到全局符号表中的元素。
如果是函数，它将会在状态E中被修改length，即参数个数。
如果是变量，它将向状态D提供变量的类型(t_int or t_char)，用于逗号后面的变量填表。
*/
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
    emit("");
    emit("void", std::string(token) + "()");
}

void stat_D() {
    global_varDeclare(global_tabelement->type);
}

void stat_E() {
    global_funcDeclare(global_tabelement);
}

void stat_F() {
    nextSym();
}

void stat_G() {
    global_tabelement = global_varOrFunc();
    if(global_tabelement->kind != func) error(2);
}

void stat_H() {
    nextSym();
    global_funcDeclare(global_tabelement);
}

void NFA_program() {
    where(true, "NFA_program");
    typedef enum {A, B, C, D, E, F, G, H} STAT;
    STAT status = A;
    while(status != H) {
        // cout << char(status + 'A') << endl;
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
            } else {
                error(3);
                // skip to [constsy, voidsy, intsy, charsy]
            }
            break;

        case B:
            if(sym == comma || sym == semicolon) {
                status = D;
                stat_D();
            } else if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else {
                error(4);
                // skip to [comma, semicolon, lsmall, lbig]
            }
            break;

        case C:
            if(sym == mainsy) {
                status = H;
                stat_H();
            } else if(sym == ident) {
                status = F;
                stat_F();
            } else {
                error(5);
                // skip to [ident]
            }
            break;

        case D:
            if(sym == voidsy) {
                status = C;
                stat_C();
            } else if(sym == intsy || sym == charsy) {
                status = B;
                stat_B();
            } else {
                error(6);
                // skip to [intsy, charsy, voidsy]
            }
            break;

        case E:
            if(sym == voidsy) {
                status = C;
                stat_C();
            } else if(sym == intsy || charsy) {
                status = G;
                stat_G();
            } else {
                error(6);
                // skip to [intsy, charsy, voidsy]
            }
            break;

        case F:
            if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else {
                error(7);
                // skip to [lsmall, lbig, intsy, charsy, statementbeg]
            }
            break;

        case G:
            if(sym == lsmall || sym == lbig) {
                status = E;
                stat_E();
            } else {
                error(7);
                // skip to [lsmall, lbig, intsy, charsy, statemetnbeg]
            }
            break;
        }
    }
    where(false, "NFA_program");
}
