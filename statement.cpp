#include "statement.h"

#include <set>
#include <iostream>

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
    // TODO
    // 1. const声明
    while(sym == constsy)
        constDeclare(1, tab);
    // 2. 局部变量声明
    while(sym == intsy || sym == charsy)
        local_varDeclare(tab);

        /*
    int i = 1;
    while(i) {
        nextSym();
        if(sym == lbig)i++;
        if(sym == rbig)i--;
    }
    return;

*/
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
                cout << "69const type can't be left value" << endl;
                while(sym != semicolon)nextSym();
                nextSym();
                break;
            }
        } else cout << "74ident not defined" << endl;
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
        if(sym != rbig) cout << "94should end with right brace" << endl;
        else nextSym();
        break;
    }
    where(false, "statement");
}



void ifStatement(TAB_ELEMENT *tab) {
    where(true, "ifStatement");
    nextSym();
    if(sym != lsmall) cout << "103should be left parent" << endl;
    nextSym();
    SYMBOL_TYPE exptype;
    expression(exptype, NULL);
    if(sym == lss || sym == leq || sym == eql || sym == neq|| sym == grq || sym == gtr) {
        nextSym();
        expression(exptype, NULL);
    }

    if(sym != rsmall) cout << "108should be right parent" << endl;
    nextSym();
    statement(tab);
    if(sym != elsesy) cout << "111should be else" << endl;
    nextSym();
    statement(tab);
    where(false, "ifStatement");
}

void whileStatement(TAB_ELEMENT *tab) {
    where(true, "whileStatement");
    nextSym();
    if (sym != lsmall) cout << "118should be left parent" << endl;
    nextSym();
    SYMBOL_TYPE exptype;
    expression(exptype, NULL);
    if(sym == lss || sym == leq || sym == eql || sym == grq || sym == gtr) {
        nextSym();
        expression(exptype, NULL);
    }
    if(sym != rsmall) cout << "123should be right parent" << endl;
    nextSym();
    statement(tab);
    where(false, "whileStatement");
}

void switchStatement(TAB_ELEMENT *tab) {
    where(true, "switchStatement");
    nextSym();
    if(sym != lsmall) cout << "130should be left parent" << endl;
    nextSym();
    SYMBOL_TYPE exptype;
    expression(exptype, NULL);
    if(sym != rsmall) cout << "134should be right parent" << endl;
    nextSym();
    if(sym != lbig) cout << "136should be left brace" << endl;
    nextSym();
    if(sym != casesy) cout << "139should be case" << endl;
    do {
        nextSym();
        if(sym != charcon && sym != intcon) cout << "142should be ident" << endl;
        nextSym();
        if(sym != colon) cout << "144should be colon" << endl;
        nextSym();
        statement(tab);
    } while(sym == casesy);
        if(sym == defaultsy) {
            nextSym();
            if(sym != colon)cout << "150should be colon" << endl;
            nextSym();
            statement(tab);
        }
    if(sym != rbig) cout << "154switch should end with right brace" << endl;
    nextSym();
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
    while(sym != semicolon) nextSym();
    nextSym();
    where(false, "printfStatement");
}

void assignStatement(TAB_ELEMENT *tab) {
    where(true, "assignStatement");
    while(sym != becomes) nextSym();
    nextSym();
    SYMBOL_TYPE exptype;
    expression(exptype, NULL);
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
    while(sym != semicolon) nextSym();
    nextSym();
    where(false, "returnStatement");
}



void expression(SYMBOL_TYPE &type, char* res) {
    where(true, "expression");
    if(sym == pluscon || sym == minuscon) nextSym();
    SYMBOL_TYPE t;
    term(t, NULL);
    while(sym == pluscon || sym == minuscon) {
        nextSym();
        term(t, NULL);
    }
    where(false, "expression");
}

void term(SYMBOL_TYPE &type, char* res) {
    where(true, "term");
    SYMBOL_TYPE t;
    factor(t, NULL);
    while(sym == timescon || sym == divcon) {
        nextSym();
        factor(t, NULL);
    }
    where(false, "term");
}

void factor(SYMBOL_TYPE &type, char* res) {
    where(true, "factor");
    SYMBOL_TYPE t;
    switch (sym) {
    case ident:
        TAB_ELEMENT lkup;
        if(lookup(token, true, &lkup) || lookup(token, false, &lkup)) {
            if(lkup.kind == func) {
                if(lkup.length) {
                    nextSym();
                    do {
                        expression(t, NULL);
                        nextSym();
                    } while(sym == comma);
                }
                nextSym();
            }
            else if(lkup.kind == var || lkup.kind == para) {
                if(lkup.length) {
                    nextSym();
                    nextSym();
                    expression(t, NULL);
                }
                nextSym();
            } else if(lkup.kind == cons) nextSym();
        } else cout << "228ident not defined" << endl;
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
        expression(t, NULL);
        nextSym();
    }
    where(false, "factor");
}
