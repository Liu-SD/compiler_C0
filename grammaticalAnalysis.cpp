#include "grammaticalAnalysis.h"
#include <iostream>

using namespace std;

/*
global_tabelementָ��ָ��״̬B��״̬F�ж������ŵ�ȫ�ַ��ű��е�Ԫ�ء�
����Ǻ�������������״̬E�б��޸�length��������������
����Ǳ�����������״̬D�ṩ����������(t_int or t_char)�����ڶ��ź���ı������
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
    if(global_tabelement->kind != func) cout << "no var after function declaration" << endl;
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
        //cout << char(status + 'A') << endl;
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
            break;
        }
    }
    where(false, "NFA_program");
}
