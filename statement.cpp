#include "statement.h"

#include <set>
#include <iostream>

#define isCompareOp(x) ((sym) == lss || (sym) == leq || (sym) == eql || (sym) == neq|| (sym) == grq || (sym) == gtr)

using namespace std;


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
    // 1. const声明
    while(sym == constsy)
        constDeclare(1, tab);
    // 2. 局部变量声明
    while(sym == intsy || sym == charsy)
        local_varDeclare(tab);
    // 3. 调用statement
    statementArray(tab);
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

    // temp
    nextSym();
    if(sym != lsmall) {
        error(22);
    } else
        nextSym();
    SYMBOL_TYPE exptype;
    std::string s;
    expression(exptype, s);
    if(isCompareOp(sym)) {
        nextSym();
        expression(exptype, s);
    }

    if(sym != rsmall) {
        error(17);
    } else
        nextSym();
    statement(tab);
    if(sym != elsesy) {
        error(23);
    }
    nextSym();
    statement(tab);

    where(false, "ifStatement");
}

void whileStatement(TAB_ELEMENT *tab) {
    where(true, "whileStatement");
    nextSym();
    if (sym != lsmall) {
        error(22);
    } else nextSym();
    SYMBOL_TYPE exptype;
    std::string s;
    expression(exptype, s);
    if(isCompareOp(sym)) {
        nextSym();
        expression(exptype, s);
    }
    if(sym != rsmall) {
        error(17);
    } else nextSym();
    statement(tab);
    where(false, "whileStatement");
}

void switchStatement(TAB_ELEMENT *tab) {
    where(true, "switchStatement");
    nextSym();
    if(sym != lsmall) {
        error(22);
    } else nextSym();
    SYMBOL_TYPE exptype;
    std::string s;
    expression(exptype, s);
    if(sym != rsmall) {
        error(17);
    } else nextSym();
    if(sym != lbig) {
        error(18);
    } else nextSym();
    if(sym != casesy) {
        error(24);
    }
    do {
        nextSym();
        if(sym != charcon && sym != intcon) {
            error(11);
            error(13);
        } else nextSym();
        if(sym != colon) {
            error(25);
        } else
            nextSym();
        statement(tab);
    } while(sym == casesy);
    if(sym == defaultsy) {
        nextSym();
        if(sym != colon) {
            error(25);
        } else
            nextSym();
        statement(tab);
    }
    if(sym != rbig) {
        error(21);
    } else nextSym();
    where(false, "switchStatement");
}

void scanfStatement(TAB_ELEMENT *tab) {
    where(true, "scanfStatement");
    while(sym != semicolon) nextSym();
    nextSym();
    where(false, "scnafStatement");
}

void printfStatement(TAB_ELEMENT *tab) {
    where(true, "printfStatement");
    SYMBOL_TYPE t;
    std::string s;
    nextSym();
    nextSym();
    if(sym == stringcon) {
        nextSym();
        if(sym == comma) {
            nextSym();
            expression(t, s);
        }

    } else {
        expression(t, s);
    }
    nextSym();
    nextSym();
    where(false, "printfStatement");
}

void assignStatement(TAB_ELEMENT *tab) {
    where(true, "assignStatement");
    while(sym != becomes) nextSym();
    nextSym();
    SYMBOL_TYPE exptype;
    std::string s;
    expression(exptype, s);
    nextSym();
    where(false, "assignStatement");
}

void callStatement(TAB_ELEMENT *tab) {
    where(true, "callStatement");
    while(sym != semicolon) nextSym();
    nextSym();
    where(false, "callStatement");
}

void returnStatement(TAB_ELEMENT *tab) {
    where(true, "returnStatement");
    SYMBOL_TYPE t;
    std::string s;
    nextSym(); // returnsy
    if(sym != semicolon) {
        nextSym(); // left parent
        expression(t, s);
        nextSym(); // right parent
    }
    nextSym(); // semi
    where(false, "returnStatement");
}



void expression(SYMBOL_TYPE &type, std::string &res) {
    where(true, "expression");
    if(sym == pluscon || sym == minuscon) nextSym();
    SYMBOL_TYPE t;
    std::string s;
    term(t, s);
    while(sym == pluscon || sym == minuscon) {
        nextSym();
        term(t, s);
    }
    where(false, "expression");
}

void term(SYMBOL_TYPE &type, std::string &res) {
    where(true, "term");
    SYMBOL_TYPE t;
    std::string s;
    factor(t, s);
    while(sym == timescon || sym == divcon) {
        nextSym();
        factor(t, s);
    }
    where(false, "term");
}

void factor(SYMBOL_TYPE &type, std::string &res) {
    where(true, "factor");
    SYMBOL_TYPE t;
    std::string s;
    switch (sym) {
    case ident:
        TAB_ELEMENT lkup;
        if(lookup(token, true, &lkup) || lookup(token, false, &lkup)) {
            if(lkup.kind == func) {
                if(lkup.length) {
                    nextSym();
                    do {
                        nextSym();
                        expression(t, s);
                    } while(sym == comma);
                }
                nextSym();
            } else if(lkup.kind == var || lkup.kind == para) {
                if(lkup.length) {
                    nextSym();
                    nextSym();
                    expression(t, s);
                }
                nextSym();
            } else if(lkup.kind == cons) nextSym();
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
        nextSym();
        break;
    case intcon:
        nextSym();
        break;
    case charcon:
        nextSym();
        break;
    case lsmall:
        nextSym();
        expression(t, s);
        nextSym();
    }
    where(false, "factor");
}
