#include "declaration.h"


void constDeclare(int lev, TAB_ELEMENT* tab) {
    where(true, "constDeclare");
    if(sym != constsy) {
        error(8);
        // 不会报错，进入constDeclare之前保证开头为const
    }
    nextSym();
    if(sym == intsy) {
        do {
            nextSym();
            if(sym != ident) {
                error(9);
                skip({ident, comma, semicolon});
                if(sym == comma)
                    continue;
                else if(sym == semicolon) {
                    nextSym();
                    return;
                }

            }
            nextSym();
            if(sym != becomes) {
                error(10);
                skip({becomes, intcon, minuscon, pluscon, comma, semicolon});
                if(sym == becomes)
                    nextSym();
                else if(sym == comma)
                    continue;
                else if(sym == semicolon) {
                    nextSym();
                    return;
                }
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
                skip({intcon, semicolon, comma});
                if(sym == semicolon) {
                    nextSym();
                    return;
                }

                else if(sym == comma)
                    continue;
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
                skip({ident, comma, semicolon});
                if(sym == comma)
                    continue;
                else if(sym == semicolon) {
                    nextSym();
                    return;
                }
            }
            nextSym();
            if(sym != becomes) {
                error(10);
                skip({becomes, charcon, comma, semicolon});
                if(sym == becomes)
                    nextSym();
                else if(sym == comma)
                    continue;
                else if(sym == semicolon) {
                    nextSym();
                    return;
                }
            } else
                nextSym();
            if(sym != charcon) {
                error(13);
                skip({charcon, comma, semicolon});
                if(sym == comma)
                    continue;
                else if(sym == semicolon) {
                    nextSym();
                    return;
                }
            }
            if(strlen(token) && !lookup(token, lev, NULL) && !(tab && !strcmp(token, tab->ident)))
                enter(token, cons, t_char, 0, num, lev);
            else
                error(12);
            nextSym();
        } while (sym == comma);
    } else {
        error(14);
        skip({semicolon});
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
        skip({comma, semicolon, lsmall, lbig});
        if(sym == lsmall || sym == lbig)
            return enter("", func, type, 0, 0, 0);
        else
            return enter("", var, type, 0, 0, 0);
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
            skip({rmedium, comma, semicolon, lsmall, lbig});
            if(sym == lsmall || sym == lbig)
                return enter("", func, type, 0, 0, 0);
            else if(sym == semicolon || sym == comma)
                return enter("", var, type, length, 0, 0);
        } else {
            length = num;
            nextSym();
        }
        if(sym != rmedium) {
            error(16);
            skip({comma, semicolon, rmedium, lsmall, lbig});
            if(sym == rmedium)
                nextSym();
        } else
            nextSym();
    }
    where(false, "global_varOrFunc");

    if(sym == comma || sym == semicolon)
        return enter(token, var, type, length, 0, 0);
    else if (sym == lsmall || sym == lbig) {
        emit(type == t_int ? "int": "char", std::string(token) + "()");
        return enter(token, func, type, 0, 0, 0);
    } else {
        error(15);
        skip({comma, semicolon, lsmall, lbig});
        if(sym == comma || sym == semicolon)
            return enter("", var, type, length, 0, 0);
        else if (sym == lsmall || sym == lbig)
            return enter("", func, type, 0, 0, 0);

    }
}

void global_varDeclare(SYMBOL_TYPE type) {
    where(true, "global_varDeclare");
    while(sym == comma) {
        nextSym();
        if(sym != ident) {
            error(9);
            skip({comma, semicolon, ident});
            if(sym == comma || sym == semicolon)
                continue;
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
                skip({rmedium, semicolon, comma});
                if(sym == semicolon || sym == comma)
                    continue;
            } else {
                length = num;
                nextSym();
            }
            if(sym != rmedium) {
                error(16);
                skip({rmedium, semicolon, comma});
                if(sym == semicolon || sym == comma)
                    continue;
                else
                    nextSym();
            } else nextSym();
        }
        enter(token, var, type, length, 0, 0);

    }
    if(sym != semicolon) {
        error(15);
        skip({semicolon});
    }
    nextSym();
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
            skip({ident, comma, semicolon});
            if(sym == comma || sym == semicolon)
                continue;
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
                skip({rmedium, comma, semicolon});
                if(sym == comma || sym == semicolon)
                    continue;
            } else {
                length = num;
                nextSym();
            }
            if(sym != rmedium) {
                error(16);
                skip({rmedium, comma, semicolon});
                if(sym == comma || sym == semicolon)
                    continue;
                else
                    nextSym();
            } else nextSym();
        }
        enter(token, var, t, length, 0, 1);
    } while(sym == comma);
    if(sym != semicolon) {
        error(15);
        skip({semicolon});
        nextSym();
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
                else {
                    error(14);
                    skip({intsy, charsy, rsmall, comma});
                    if(sym == rsmall || sym == comma)
                        continue;
                }
                nextSym();
                if(sym != ident) {
                    error(9);
                    skip({ident, rsmall, comma});
                    if(sym == rsmall || sym == comma)
                        continue;
                }
                if(lookup(token, true, NULL)) {
                    error(12);
                }
                enter(token, para, t, 0, 0, 1);
                // emit("para", t == t_int ? "int": "char", std::string(token));
                ++paracount;
                nextSym();
            } while(sym == comma);
        }
        if(sym != rsmall) {
            error(17);
            skip({rsmall, lbig});
            if(sym == rsmall)
                nextSym();
        } else nextSym();
    }
    // 2. 参数个数反填。
    tab->length = paracount;
    // 3. 读到lbig开始调用compoundStatement，同样将指针传过去。
    if(sym != lbig) {
        error(18);
        skip({lbig});
    }
    nextSym();
    compoundStatement(tab);
    if(sym == rbig)
        nextSym();
    else {
        error(17);
        skip({rbig});
        nextSym();
    }
    where(false, "global_funcDeclare");
}
