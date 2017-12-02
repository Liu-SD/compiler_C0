#include "declaration.h"
#include <string.h>
#include <iostream>
using namespace std;


void constDeclare(int lev, TAB_ELEMENT* tab) {
    where(true, "constDeclare");
    if(sym != constsy) {
        error(8);
    }
    nextSym();
    if(sym == intsy) {
        do {
            nextSym();
            if(sym != ident) {
                error(9);
                token[0] = 0; // change to a null token
            }
            nextSym();
            if(sym != becomes) {
                error(10);
            } else
                nextSym();
            int signal = 1;
            if(sym == minuscon || sym == pluscon) {
                if(sym == minuscon)
                    signal = -1;
                nextSym();
                if(sym != intcon || num == 0) {
                    error(11);
                }
            }
            if(sym != intcon) {
                error(11);
            }

            if(!lookup(token, lev, NULL) && !(tab && !strcmp(token, tab->ident)))
                enter(token, cons, t_int, 0, signal * num, lev);
            else {
                error(12);
            }
            nextSym();

        } while(sym == comma);
    } else if(sym == charsy) {
        do {
            nextSym();
            if(sym != ident) {
                error(9);
                token[0] = 0;
            }
            nextSym();
            if(sym != becomes) {
                error(10);
            }
            nextSym();
            if(sym != charcon) {
                error(13);
            }
            if(strlen(token) && !lookup(token, lev, NULL) && !(tab && !strcmp(token, tab->ident)))
                enter(token, cons, t_char, 0, num, lev);
            else
                error(12);
            nextSym();
        } while (sym == comma);
    } else {
        error(14);
        // skip to [intsy, charsy, semicolon]
    }
    if(sym != semicolon)
        error(15);
    else
        nextSym(); // maybe missing semicolon
    where(false, "constDeclare");
}

void constDeclare(int lev) {
    constDeclare(lev, NULL);
}


TAB_ELEMENT* global_varOrFunc() {
    where(true, "global_varOrFunc");
    SYMBOL_TYPE type = sym == intsy ? t_int : t_char;
    nextSym();
    if(sym != ident) {
        error(9);
        token[0] = 0;
    }
    if(lookup(token, false, NULL)) {
        error(12);
    }
    nextSym();
    int length = 0;
    if(sym == lmedium) {
        nextSym();
        if(sym != intcon || num == 0) {
            error(11);
            num = 1;
        }
        length = num;
        nextSym();
        if(sym != rmedium) {
            error(16);
        }
        nextSym();
    }
    where(false, "global_varOrFunc");

    if(sym == comma || sym == semicolon)
        return enter(token, var, type, length, 0, 0);
    else if (sym == lsmall || sym == lbig){

        emit(type == t_int ? "int": "char", std::string(token) + "()");
        return enter(token, func, type, 0, 0, 0);
    }
    else return NULL;
}

void global_varDeclare(SYMBOL_TYPE type) {
    where(true, "global_varDeclare");
    while(sym == comma) {
        nextSym();
        if(sym != ident) {
            error(9);
            token[0] = 0;
        }
        if(lookup(token, false, NULL)) {
            error(12);
        }
        nextSym();
        int length = 0;
        if(sym == lmedium) {
            nextSym();
            if(sym != intcon || num == 0) {
                error(11);
            }
            length = num;
            nextSym();
            if(sym != rmedium) {
                error(16);
            } else nextSym();
        }
        enter(token, var, type, length, 0, 0);

    }
    if(sym == semicolon)nextSym();
    else error(15);
    where(false, "global_varDeclare");
}

void local_varDeclare(TAB_ELEMENT *tab) {
    where(true, "local_varDeclare");
    SYMBOL_TYPE t;
    if(sym == intsy) t = t_int;
    else if(sym == charsy) t = t_char;
    do {
        nextSym();
        if(sym != ident) {
            error(9);
            token[0] = 0;
        }
        if(lookup(token, true, NULL) || !strcmp(token, tab->ident)) {
            error(12);
        }
        nextSym();
        int length = 0;
        if(sym == lmedium) {
            nextSym();
            if(sym != intcon || num == 0) {
                error(11);
            }
            length = num;
            nextSym();
            if(sym != rmedium) {
                error(16);
            } else nextSym();
        }
        enter(token, var, t, length, 0, 1);
    } while(sym == comma);
    if(sym != semicolon) {
        error(15);
    } else nextSym();
    where(false, "local_varDeclare");
}

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
                if(sym != ident) {
                    error(9);
                    token[0] = 0;
                }
                if(lookup(token, true, NULL)) {
                    error(12);
                }
                enter(token, para, t, 0, 0, 1);
                emit("para", t == t_int ? "int": "char", std::string(token));
                ++paracount;
                nextSym();
            } while(sym == comma);
        }
        if(sym != rsmall) {
            error(17);
        } else nextSym();
    }
    // 2. 参数个数反填。
    tab->length = paracount;
    // 3. 读到lbig开始调用compoundStatement，同样将指针传过去。
    if(sym == lbig) {
        nextSym();
        compoundStatement(tab);
        if(sym == rbig)
            nextSym();
        else {
            error(17);
        }
    } else {
        error(18);
    }
    where(false, "global_funcDeclare");
}
