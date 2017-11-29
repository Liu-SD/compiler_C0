#include "declaration.h"
#include <string.h>
#include <iostream>
using namespace std;


void constDeclare(int lev, TAB_ELEMENT* tab) {
    where(true, "constDeclare");
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
                if(sym != intcon || num == 0)
                    cout << "should be int number and larger than 0 after +/-" << endl;
            }
            if(sym != intcon)
                cout << "should be int" << endl;
            else {
                if(!lookup(token, lev, NULL) && !(tab && !strcmp(token, tab->ident)))
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
                if(!lookup(token, lev, NULL) && !(tab && !strcmp(token, tab->ident)))
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
    where(false, "constDeclare");
}

void constDeclare(int lev) {
    constDeclare(lev, NULL);
}


TAB_ELEMENT* global_varOrFunc() {
    where(true, "global_varOrFunc");
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
    where(false, "global_varOrFunc");
    if(sym == comma || sym == semicolon)
        return enter(token, var, type, length, 0, 0);
    else if (sym == lsmall || sym == lbig)
        return enter(token, func, type, 0, 0, 0);
    else return NULL;
}

void global_varDeclare(SYMBOL_TYPE type) {
    where(true, "global_varDeclare");
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
    where(false, "global_varDeclare");
}

void local_varDeclare(TAB_ELEMENT *tab) {
    where(true, "local_varDeclare");
    SYMBOL_TYPE t;
    if(sym == intsy) t = t_int;
    else if(sym == charsy) t = t_char;
    do {
        nextSym();
        if(sym != ident) cout << "should be ident" << endl;
        if(lookup(token, true, NULL) || !strcmp(token, tab->ident)) cout << "ident repeated" << endl;
        nextSym();
        int length = 0;
        if(sym == lmedium) {
            nextSym();
            if(sym != intcon || num == 0) cout << "array length should be int and larger than zero" << endl;
            length = num;
            nextSym();
            if(sym != rmedium) cout << "should be right bracket" << endl;
            nextSym();
        }
        enter(token, var, t, length, 0, 1);
    } while(sym == comma);
    if(sym != semicolon) cout << "should end with semicolon" << endl;
    nextSym();
    where(false, "local_varDeclare");
}
/*
void functionBody(TAB_ELEMENT *tab) {
    if(sym != lbig) cout << "error here" << endl;

    // TODO
    // 1. const声明
    nextSym();
    while(sym == constsy)
        constDeclare(1, tab);
    // 2. 局部变量声明
    while(sym == intsy || sym == charsy)
        local_varDeclare(tab);
    // 3. 调用statement
    statementArray(tab);
    nextSym();
}
*/
void global_funcDeclare(TAB_ELEMENT *tab) {
    where(true, "global_funcDeclare");
    // TODO
    // 1. 读参数压到符号表中。
    int paracount = 0;
    if(sym == lsmall) {
        if(!strcmp(tab->ident, "main")) {
            nextSym();
        } else {
            do {
                nextSym();
                SYMBOL_TYPE t;
                if(sym == intsy) t = t_int;
                else if(sym == charsy) t = t_char;
                nextSym();
                if(sym != ident) cout << "should be ident" << endl;
                if(lookup(token, true, NULL)) cout << "ident repeated" << endl;
                enter(token, para, t, 0, 0, 1);
                ++paracount;
                nextSym();
            } while(sym == comma);
        }
        if(sym != rsmall) cout << "should be right parent" << endl;
        else nextSym();
    }
    // 2. 参数个数反填。
    tab->length = paracount;
    // 3. 读到lbig开始调用compoundStatement，同样将指针传过去。
    if(sym == lbig) {
        nextSym();
        compoundStatement(tab);
        if(sym == rbig)
            nextSym();
        else tmp_error("function body should end with right brace");
    } else cout << "function body should start with left brace" << endl;
    where(false, "global_funcDeclare");
}
