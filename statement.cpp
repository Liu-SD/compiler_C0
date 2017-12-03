#include "statement.h"

#include <set>
#include <iostream>

#define isCompareOp(x) ((sym) == lss || (sym) == leq || (sym) == eql || (sym) == neq|| (sym) == grq || (sym) == gtr)

using namespace std;

bool returnExist;


bool statementBeginSym(SYMBOL sym) {
    static SYMBOL l[] = {
        ifsy, whilesy, switchsy,
        ident, mainsy, returnsy,
        semicolon, scanfsy, printfsy, lbig
    };
    static set<SYMBOL> begSet(l, l + 10);
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
                while(sym != semicolon)nextSym();
                nextSym();
                break;
            }
        } else {
            error(20);
            do
                nextSym();
            while(!statementBeginSym(sym));
        }
        break;
    case mainsy:
        callStatement(tab);
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
        } else nextSym();
        break;
    }
    where(false, "statement");
}



void ifStatement(TAB_ELEMENT *tab) {
    where(true, "ifStatement");

    nextSym();
    if(sym != lsmall) {
        error(22);
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
            emit("GTEZ", t, jump_to_else);
            break;
        case leq:
            emit("GTZ", t, jump_to_else);
            break;
        case eql:
            emit("BNEZ", t, jump_to_else);
            break;
        case neq:
            emit("BEZ", t, jump_to_else);
            break;
        case grq:
            emit("LTZ", t, jump_to_else);
            break;
        case gtr:
            emit("LTEZ", t, jump_to_else);
            break;
        }
    } else {
        emit("BEZ", name_left, jump_to_else);
    }

    // end compare expression

    if(sym != rsmall) {
        error(17);
    } else
        nextSym();
    statement(tab);
    std::string jump_to_end = newLabel();
    emit("JMP", jump_to_end);
    setLabelVal(jump_to_else, mcode_lc());
    if(sym != elsesy) {
        error(23);
    }
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
            emit("GTEZ", t, jump_to_end);
            break;
        case leq:
            emit("GTZ", t, jump_to_end);
            break;
        case eql:
            emit("BNEZ", t, jump_to_end);
            break;
        case neq:
            emit("BEZ", t, jump_to_end);
            break;
        case grq:
            emit("LTZ", t, jump_to_end);
            break;
        case gtr:
            emit("LTEZ", t, jump_to_end);
            break;
        }
    } else {
        emit("BEZ", name_left, jump_to_end);
    }

    // end compare expression



    if(sym != rsmall) {
        error(17);
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
    } else nextSym();
    SYMBOL_TYPE type_switch;
    std::string name_switch;
    expression(type_switch, name_switch);
    if(sym != rsmall) {
        error(17);
    } else nextSym();
    if(sym != lbig) {
        error(18);
    } else nextSym();
    if(sym != casesy) {
        error(24);
    }
    std::string jump_to_next_case;
    std::string jump_to_end = newLabel();
    do {
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
            if(signal == -1) {
                std::string t0 = newTmpVar();
                emit(t0, int2str(-1), "*", name_switch);
                std::string t1 = newTmpVar();
                emit(t1, t0, "-", int2str(num));
                emit("BNEZ", t1, jump_to_next_case);
            } else {
                std::string t0 = newTmpVar();
                emit(t0, name_switch, "-", int2str(num));
                emit("BNEZ", t0, jump_to_next_case);
            }
            nextSym();
        } else {
            error(11);
            error(13);
        }


        if(sym != colon) {
            error(25);
        } else
            nextSym();
        statement(tab);
        emit("JMP", jump_to_end);


    } while(sym == casesy);
    if(sym == defaultsy) {
        setLabelVal(jump_to_next_case, mcode_lc());

        nextSym();
        if(sym != colon) {
            error(25);
        } else
            nextSym();
        statement(tab);
    } else {
        setLabelVal(jump_to_next_case, mcode_lc());
    }
    setLabelVal(jump_to_end, mcode_lc());
    if(sym != rbig) {
        error(21);
    } else nextSym();
    where(false, "switchStatement");
}

void scanfStatement(TAB_ELEMENT *tab) {
    where(true, "scanfStatement");
    nextSym();
    if(sym != lsmall)
        error(22);
    do {
        TAB_ELEMENT ele;
        nextSym();
        if(sym != ident)
            error(9);
        else if(!lookup(token, true, &ele) && !lookup(token, false, &ele))
            error(20);
        else if(ele.kind != para && ele.kind != var)
            error(19);
        else if(ele.length)
            error(28);
        else
            emit("SCANF", token);
        nextSym();
    } while(sym == comma);
    if(sym != rsmall)
        error(17);
    else
        nextSym();
    if(sym != semicolon)
        error(15);
    else
        nextSym();
    where(false, "scnafStatement");
}

void printfStatement(TAB_ELEMENT *tab) {
    where(true, "printfStatement");
    SYMBOL_TYPE type_exp;
    std::string name_exp;
    nextSym();
    nextSym();
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
    nextSym();
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
        if(sym != rmedium)
            error(16);
        else nextSym();
    } else {
        if(ele_left.length) {
            error(28);
        }
    }
    if(sym != becomes) error(10);
    nextSym();
    std::string name_right;
    SYMBOL_TYPE type_right;
    expression(type_right, name_right);

    if(ele_left.type == t_char && type_right == t_int)
        std::cout << "warning: converting int into char may lose informations" << std::endl;

    emit(ident_name, "=", name_right);
    if(sym != semicolon)
        error(15);
    else
        nextSym();

    where(false, "assignStatement");
}

void callStatement(TAB_ELEMENT *tab) {
    where(true, "callStatement");
    std::string funcname(token);
    TAB_ELEMENT funcele;
    lookup(token, false, &funcele);
    emit("MKS");
    nextSym();

    if(funcele.length == 0) {
        if(sym != semicolon)
            error(15);
        else
            nextSym();
    } else {
        int para_count = 0;
        do {
            nextSym();
            std::string name_para;
            SYMBOL_TYPE type_para;
            expression(type_para, name_para);

            if(type_para != local_tab[funcele.value][para_count].type)
                error(26);

            emit("PUSH", name_para);
            ++para_count;
        } while(sym == comma);
        if(para_count != funcele.length)
            error(26);
        if(sym != rsmall)
            error(17);
        else
            nextSym();
        if(sym != semicolon)
            error(15);
        else
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
        if(sym != rsmall)
            error(17);
        else
            nextSym(); // right parent
    } else {
        if(tab->type != t_void)
            error(30);
    }
    if(sym == semicolon)
        nextSym(); // semi
    else error(15);
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
    switch (sym) {
    case ident:
        TAB_ELEMENT lkup;
        if(lookup(token, true, &lkup) || lookup(token, false, &lkup)) {
            if(lkup.kind == func) {
                if(lkup.type == t_void)
                    error(33);
                else
                    type = lkup.type;
                nextSym();
                emit("MKS");
                if(lkup.length) {
                    if(sym != lsmall)
                        error(22);
                    int para_count = 0;
                    do {
                        nextSym();
                        std::string para_name;
                        SYMBOL_TYPE para_type;
                        expression(para_type, para_name);

                        if(local_tab[lkup.value][para_count].type != para_type)
                            error(26);

                        emit("PUSH", para_name);
                        ++para_count;

                    } while(sym == comma);
                    if(para_count != lkup.length)
                        error(26);
                    if(sym != rsmall)
                        error(17);
                    else
                        nextSym();
                }
                emit("CAL", std::string(lkup.ident));
                res = newTmpVar();
                emit(res, "=", "RET");

            } else if(lkup.kind == var || lkup.kind == para) {
                nextSym();

                if(lkup.length) {
                    if(sym != lmedium)
                        error(32);
                    else
                        nextSym();
                    std::string index_name;
                    SYMBOL_TYPE index_type;
                    expression(index_type, index_name);
                    if(index_type != t_int)
                        error(31);
                    res = std::string(lkup.ident) + "[" + index_name + "]";
                    type = lkup.type;

                    if(sym != rmedium)
                        error(16);
                    else nextSym();
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
                while(sym != rsmall)nextSym();
                nextSym();
            } else if(sym == lmedium) {
                while(sym != rmedium)nextSym();
                nextSym();
            }
        }
        break;
    case pluscon:
    case minuscon:
        nextSym();
        if(sym != intcon || num == 0)
            error(11);
        type = t_int;
        res = int2str(num);
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
        else
            error(17);
    }
    where(false, "factor");
}
