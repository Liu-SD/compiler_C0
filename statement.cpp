#include "statement.h"

#define isCompareOp(x) ((sym) == lss || (sym) == leq || (sym) == eql || (sym) == neq|| (sym) == grq || (sym) == gtr)
#define STATBEGSYM ifsy, whilesy, switchsy, ident, mainsy, returnsy, semicolon, scanfsy, printfsy, lbig

// using namespace std;

bool returnExist;


bool statementBeginSym(SYMBOL sym) {
    static SYMBOL l[] = {
        ifsy, whilesy, switchsy,
        ident, mainsy, returnsy,
        semicolon, scanfsy, printfsy, lbig
    };
    static std::set<SYMBOL> begSet(l, l + 10);
    return begSet.find(sym) != begSet.end();
}

void compoundStatement(TAB_ELEMENT *tab) {
    where(true, "compoundStatement");
    returnExist = false;
    // 1. const声明
    while(sym == constsy)
        constDeclare(1, tab);
    // 2. 局部变量声明
    while(sym == intsy || sym == charsy)
        local_varDeclare(tab);
    // 3. 调用statement
    statementArray(tab);
    if(tab->type != t_void && !returnExist)
        error(27);
    emit("RETURN");
    where(false, "compoundStatement");
}

void statementArray(TAB_ELEMENT *tab) {
    where(true, "statementArray");
    while(statementBeginSym(sym))
        statement(tab);
    where(false, "statementArray");
}

void statement(TAB_ELEMENT *tab) {
    where(true, "statement");
    switch (sym) {
    case ifsy:
        ifStatement(tab);
        break;
    case whilesy:
        whileStatement(tab);
        break;
    case switchsy:
        switchStatement(tab);
        break;
    case ident:
        // lookup and switch case
        TAB_ELEMENT lkup;
        if(lookup(token, true, &lkup) || lookup(token, false, &lkup)) {
            switch (lkup.kind) {
            case var:
            case para:
                assignStatement(tab);
                break;
            case func:
                callStatement(tab);
                break;
            case cons:
                error(19);
                skip({semicolon, rbig});
                if(sym == semicolon)
                    nextSym();
                break;
            }
        } else {
            error(20);
            skip({semicolon, rbig});
            if(sym == semicolon)
                nextSym();
        }
        break;
    case mainsy:
        //callStatement(tab);
        error(35);
        skip({semicolon, rbig});
        if(sym == semicolon)
            nextSym();
        break;
    case returnsy:
        returnStatement(tab);
        break;
    case semicolon:
        nextSym();
        break;
    case scanfsy:
        scanfStatement(tab);
        break;
    case printfsy:
        printfStatement(tab);
        break;
    case lbig:
        nextSym();
        statementArray(tab);
        if(sym != rbig) {
            error(21);
            skip({rbig});
        }
        nextSym();
        break;
    }
    where(false, "statement");
}



void ifStatement(TAB_ELEMENT *tab) {
    where(true, "ifStatement");

    nextSym();
    if(sym != lsmall) {
        error(22);
        skip({lsmall});
        nextSym();
    } else
        nextSym();

    // compare expression
    SYMBOL_TYPE type_left;
    std::string name_left;
    expression(type_left, name_left);
    std::string jump_to_else = newLabel();
    if(isCompareOp(sym)) {
        SYMBOL op = sym;
        nextSym();
        SYMBOL_TYPE type_right;
        std::string name_right;
        expression(type_right, name_right);
        std::string t = newTmpVar();
        emit(t, name_left, "-", name_right);
        switch (op) {
        case lss:
            emit("GEZ", t, jump_to_else);
            break;
        case leq:
            emit("GZ", t, jump_to_else);
            break;
        case eql:
            emit("NEZ", t, jump_to_else);
            break;
        case neq:
            emit("EZ", t, jump_to_else);
            break;
        case grq:
            emit("LZ", t, jump_to_else);
            break;
        case gtr:
            emit("LEZ", t, jump_to_else);
            break;
        }
    } else {
        emit("EZ", name_left, jump_to_else);
    }

    // end compare expression

    if(sym != rsmall) {
        error(17);
        skip({rsmall, STATBEGSYM});
        if(sym == rsmall)
            nextSym();
    } else
        nextSym();
    statement(tab);
    std::string jump_to_end = newLabel();
    emit("JMP", jump_to_end);
    setLabelVal(jump_to_else, mcode_lc());
    if(sym != elsesy) {
        error(23);
        skip({elsesy, STATBEGSYM});
        if(sym == elsesy)
            nextSym();
    } else
        nextSym();
    statement(tab);
    setLabelVal(jump_to_end, mcode_lc());
    where(false, "ifStatement");
}

void whileStatement(TAB_ELEMENT *tab) {
    where(true, "whileStatement");
    nextSym();
    if (sym != lsmall) {
        error(22);
        skip({lsmall});
        nextSym();
    } else nextSym();

    std::string beforeCompare = newLabel();
    setLabelVal(beforeCompare, mcode_lc());

    // compare expression
    SYMBOL_TYPE type_left;
    std::string name_left;
    expression(type_left, name_left);
    std::string jump_to_end = newLabel();
    if(isCompareOp(sym)) {
        SYMBOL op = sym;
        nextSym();
        SYMBOL_TYPE type_right;
        std::string name_right;
        expression(type_right, name_right);
        std::string t = newTmpVar();
        emit(t, name_left, "-", name_right);
        switch (op) {
        case lss:
            emit("GEZ", t, jump_to_end);
            break;
        case leq:
            emit("GZ", t, jump_to_end);
            break;
        case eql:
            emit("NEZ", t, jump_to_end);
            break;
        case neq:
            emit("EZ", t, jump_to_end);
            break;
        case grq:
            emit("LZ", t, jump_to_end);
            break;
        case gtr:
            emit("LEZ", t, jump_to_end);
            break;
        }
    } else {
        emit("EZ", name_left, jump_to_end);
    }

    // end compare expression



    if(sym != rsmall) {
        error(17);
        skip({rsmall, STATBEGSYM});
        if(sym == rsmall)
            nextSym();
    } else nextSym();
    statement(tab);

    emit("JMP", beforeCompare);
    setLabelVal(jump_to_end, mcode_lc());
    where(false, "whileStatement");
}

void switchStatement(TAB_ELEMENT *tab) {
    where(true, "switchStatement");
    nextSym();
    if(sym != lsmall) {
        error(22);
        skip({lsmall});
        nextSym();
    } else nextSym();
    SYMBOL_TYPE type_switch;
    std::string name_switch;
    expression(type_switch, name_switch);
    if(sym != rsmall) {
        error(17);
        skip({rsmall, lbig});
        if(sym == rsmall)
            nextSym();
    } else nextSym();
    if(sym != lbig) {
        error(18);
        skip({lbig, casesy});
        if(sym == lbig)
            nextSym();
    } else nextSym();
    if(sym != casesy) {
        error(24);
        skip({casesy});
    }
    std::string jump_to_next_case;
    std::string jump_to_end = newLabel();
    while(sym == casesy) {
        nextSym();
        if(!jump_to_next_case.empty())
            setLabelVal(jump_to_next_case, mcode_lc());
        jump_to_next_case = newLabel();

        int signal = 1;
        if(sym == pluscon || sym == minuscon) {
            if(sym == minuscon) signal = -1;
            nextSym();
            if(sym != intcon || num == 0)
                error(11);
        }
        if(sym == intcon || sym == charcon) {
            if(sym == intcon && type_switch == t_char || sym == charcon && type_switch == t_int)
                error(34);
            if(signal == -1) {
                // std::string t0 = newTmpVar();
                // emit(t0, "=", int2str(-1 * num));
                std::string t0 = newTmpVar();
                emit(t0, int2str(-1 * num), "-", name_switch);
                emit("NEZ", t0, jump_to_next_case);
            } else {
                std::string t0 = newTmpVar();
                emit(t0, int2str(num), "-", name_switch);
                emit("NEZ", t0, jump_to_next_case);
            }
            nextSym();
        } else {
            error(11);
            error(13);
            skip({colon});
        }


        if(sym != colon) {
            error(25);
            skip({colon, STATBEGSYM});
            if(sym == colon)
                nextSym();
        } else
            nextSym();
        statement(tab);
        emit("JMP", jump_to_end);

    }
    if(sym == defaultsy) {
        setLabelVal(jump_to_next_case, mcode_lc());

        nextSym();
        if(sym != colon) {
            error(25);
            skip({colon, STATBEGSYM});
            if(sym == colon)
                nextSym();
        } else
            nextSym();
        statement(tab);
    } else {
        setLabelVal(jump_to_next_case, mcode_lc());
    }
    setLabelVal(jump_to_end, mcode_lc());
    if(sym != rbig) {
        error(21);
        skip({rbig});
        nextSym();
    } else nextSym();
    where(false, "switchStatement");
}

void scanfStatement(TAB_ELEMENT *tab) {
    where(true, "scanfStatement");
    nextSym();
    if(sym != lsmall) {
        error(22);
        skip({lsmall});
    }
    do {
        TAB_ELEMENT ele;
        nextSym();
        if(sym != ident) {
            error(9);
            skip({comma, rsmall});
        } else if(!lookup(token, true, &ele) && !lookup(token, false, &ele)) {
            error(20);
            skip({comma, rsmall});
        } else if(ele.kind != para && ele.kind != var) {
            error(19);
            skip({comma, rsmall});
        } else if(ele.length) {
            error(28);
            skip({comma, rsmall});
        } else if(ele.type = t_int)
            emit("SCANFN", token);
        else
            emit("SCANFC", token);
        nextSym();
    } while(sym == comma);
    if(sym != rsmall) {
        error(17);
        skip({rsmall, semicolon});
        if(sym == rsmall)
            nextSym();
    } else
        nextSym();
    if(sym != semicolon) {
        error(15);
        skip({semicolon, STATBEGSYM, rbig});
    }
    if(sym == semicolon)
        nextSym();
    else
        nextSym();
    where(false, "scnafStatement");
}

void printfStatement(TAB_ELEMENT *tab) {
    where(true, "printfStatement");
    SYMBOL_TYPE type_exp;
    std::string name_exp;
    nextSym();
    if(sym == lsmall)
        nextSym();
    else {
        error(22);
        skip({lsmall});
    }
    if(sym == stringcon) {
        emit("PRINTFS", "<" + std::string(stringbuff) + ">");
        nextSym();
        if(sym == comma) {
            nextSym();
            expression(type_exp, name_exp);
            if(type_exp == t_char)
                emit("PRINTFC", name_exp);
            else
                emit("PRINTFN", name_exp);
        }

    } else {
        expression(type_exp, name_exp);
        if(type_exp == t_char)
            emit("PRINTFC", name_exp);
        else
            emit("PRINTFN", name_exp);
    }
    if(sym != rsmall) {
        error(17);
        skip({rsmall, semicolon});
        if(sym == rsmall)
            nextSym();
    } else
        nextSym();
    if(sym != semicolon) {
        error(15);
        skip({semicolon});
    }
    nextSym();
    where(false, "printfStatement");
}

void assignStatement(TAB_ELEMENT *tab) {
    where(true, "assignStatement");

    std::string ident_name(token);
    TAB_ELEMENT ele_left;
    if(!lookup(token, true, &ele_left) && !lookup(token, false, &ele_left))
        error(20);

    nextSym();
    if(sym == lmedium) {
        if(ele_left.length == 0)
            error(29);
        nextSym();
        std::string name_index;
        SYMBOL_TYPE type_index;
        expression(type_index, name_index);
        ident_name = ident_name + "[" + name_index + "]";
        if(sym != rmedium) {
            error(16);
            skip({rmedium, becomes});
            if(sym == rmedium)
                nextSym();
        } else nextSym();
    } else {
        if(ele_left.length) {
            error(28);
        }
    }
    if(sym != becomes) {
        error(10);
        skip({becomes, eql});
        nextSym();
    }
    nextSym();
    std::string name_right;
    SYMBOL_TYPE type_right;
    expression(type_right, name_right);

    if(ele_left.type == t_char && type_right == t_int)
        std::cout << lc << "\t" << cc << "\t" << "warning: converting int into char may loss informations" << std::endl;

    emit(ident_name, "=", name_right);
    if(sym != semicolon) {
        error(15);
        skip({semicolon, STATBEGSYM, rbig});
        if(sym == semicolon)
            nextSym();
    } else
        nextSym();

    where(false, "assignStatement");
}

void callStatement(TAB_ELEMENT *tab) {
    where(true, "callStatement");
    std::string funcname(token);
    TAB_ELEMENT funcele;
    lookup(token, false, &funcele);
    nextSym();

    if(funcele.length == 0) {
        if(sym != semicolon) {
            error(15);
            skip({semicolon});
            nextSym();
        } else
            nextSym();
    } else {
        int para_count = 0;
        std::vector<std::string> para_names;
        do {
            nextSym();
            std::string name_para;
            SYMBOL_TYPE type_para;
            expression(type_para, name_para);

            if(type_para != local_tab[funcele.value][para_count].type)
                error(26);

            // emit("PUSH", name_para, int2str(para_count));
            para_names.push_back(name_para);
            ++para_count;
        } while(sym == comma);
        if(para_count != funcele.length)
            error(26);

        for(int i = 0; i < para_names.size(); i++)
            emit("PUSH", para_names[i], int2str(i));

        if(sym != rsmall) {
            error(17);
            skip({rsmall, semicolon});
            if(sym == rsmall)
                nextSym();
        } else
            nextSym();
        if(sym != semicolon) {
            error(15);
            skip({semicolon, STATBEGSYM, rbig});
            if(sym == semicolon)
                nextSym();
        } else
            nextSym();
    }
    emit("CAL", funcname);

    where(false, "callStatement");
}

void returnStatement(TAB_ELEMENT *tab) {
    where(true, "returnStatement");
    returnExist = true;
    SYMBOL_TYPE type;
    std::string name;
    nextSym(); // returnsy
    if(sym == lsmall) {
        nextSym(); // left parent
        expression(type, name);
        emit("RETV", name);
        if(type != tab->type) {
            error(30);
        }
        if(sym != rsmall) {
            error(17);
            skip({rsmall, semicolon});
            if(sym == rsmall)
                nextSym();
        } else
            nextSym(); // right parent
    } else {
        if(tab->type != t_void)
            error(22);
    }
    if(sym == semicolon)
        nextSym(); // semi
    else {
        error(15);
        skip({semicolon, STATBEGSYM, rbig});
        if(sym == semicolon)
            nextSym();
    }
    emit("RETURN");
    where(false, "returnStatement");
}



void expression(SYMBOL_TYPE &type, std::string &res) {
    where(true, "expression");
    std::string name = newTmpVar();
    // SYMBOL_TYPE type;
    int signal = 1;
    if(sym == pluscon || sym == minuscon) {
        if(sym == minuscon) signal = -1;
        nextSym();
    }
    SYMBOL_TYPE term_type;
    std::string term_name;
    term(term_type, term_name);
    type = term_type;
    if(signal == -1) {
        type = t_int;
        emit(name, "-1", "*", term_name);
    } else {
        emit(name, "=", term_name);
    }

    while(sym == pluscon || sym == minuscon) {
        SYMBOL op = sym;
        nextSym();
        term(term_type, term_name);
        if(op == pluscon)
            emit(name, name, "+", term_name);
        else
            emit(name, name, "-", term_name);
        type = t_int;
    }

    res = name;

    where(false, "expression");
}

void term(SYMBOL_TYPE &type, std::string &res) {
    where(true, "term");
    std::string name = newTmpVar();
    std::string fac_name;
    factor(type, fac_name);
    emit(name, "=", fac_name);
    while(sym == timescon || sym == divcon) {
        SYMBOL op = sym;
        nextSym();
        SYMBOL_TYPE fac_type;
        factor(fac_type, fac_name);
        if(op == timescon)
            emit(name, name, "*", fac_name);
        else
            emit(name, name, "/", fac_name);
        type = t_int;
    }
    res = name;
    where(false, "term");
}

void factor(SYMBOL_TYPE &type, std::string &res) {
    where(true, "factor");
    SYMBOL op;
    switch (sym) {
    case ident:
        TAB_ELEMENT lkup;
        if(lookup(token, true, &lkup) || lookup(token, false, &lkup)) {
            if(lkup.kind == func) {
                if(lkup.type == t_void) {
                    error(33);
                    type = t_int; // 随便给它一个类型
                } else
                    type = lkup.type;
                nextSym();
                if(lkup.length) {
                    if(sym != lsmall) {
                        error(22);
                        if(sym == lmedium) {
                            skip({rmedium});
                            nextSym();
                        }
                        res = "";
                        return;
                    }
                    int para_count = 0;
                    std::vector<std::string> para_names;
                    do {
                        nextSym();
                        std::string para_name;
                        SYMBOL_TYPE para_type;
                        expression(para_type, para_name);

                        if(local_tab[lkup.value][para_count].type != para_type)
                            error(26);

                        //emit("PUSH", para_name, int2str(para_count));
                        para_names.push_back(para_name);
                        ++para_count;

                    } while(sym == comma);
                    if(para_count != lkup.length)
                        error(26);
                    for(int i = 0; i < para_names.size(); i++)
                        emit("PUSH", para_names[i], int2str(i));
                    if(sym != rsmall) {
                        error(17);
                        skip({rsmall});
                        nextSym();
                    } else
                        nextSym();
                }
                emit("CAL", std::string(lkup.ident));
                res = newTmpVar();
                emit(res, "=", "RET");

            } else if(lkup.kind == var || lkup.kind == para) {
                nextSym();

                if(lkup.length) {
                    if(sym != lmedium) {
                        error(32);
                        if(sym == lsmall) {
                            skip({rsmall});
                            nextSym();
                        }
                        type = t_int;
                        res = "";
                    } else
                        nextSym();
                    std::string index_name;
                    SYMBOL_TYPE index_type;
                    expression(index_type, index_name);
                    if(index_type != t_int)
                        error(31);
                    res = std::string(lkup.ident) + "[" + index_name + "]";
                    type = lkup.type;

                    if(sym != rmedium) {
                        error(16);
                        skip({rmedium});
                        nextSym();
                    } else nextSym();
                } else {
                    type = lkup.type;
                    res = std::string(lkup.ident);
                }


            } else if(lkup.kind == cons) {
                type = lkup.type;
                res = std::string(lkup.ident);
                nextSym();
            }
        } else {
            error(20);
            nextSym();
            if(sym == lsmall) {
                skip({rsmall});
                nextSym();
            } else if(sym == lmedium) {
                skip({rmedium});
                nextSym();
            }
        }
        break;
    case pluscon:
    case minuscon:
        op = sym;
        nextSym();
        if(sym != intcon || num == 0) {
            error(11);
            skip({intcon, pluscon, minuscon, timescon, divcon});
            if(sym != intcon) {
                type = t_int;
                res = "";
                return;
            }
        }
        type = t_int;
        res = int2str(op == pluscon ? num : -num);
        nextSym();
        break;
    case intcon:
        type = t_int;
        res = int2str(num);
        nextSym();
        break;
    case charcon:
        type = t_char;
        res = int2str(num);
        nextSym();
        break;
    case lsmall:
        nextSym();
        expression(type, res);
        if(sym == rsmall)
            nextSym();
        else {
            error(17);
            skip({rsmall});
            nextSym();
        }
    }
    where(false, "factor");
}
