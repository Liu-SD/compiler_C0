#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include "lexicalAnalysis.h"

#define isnum(x) ((x) >= '0' && (x) <= '9')
#define isalpha(x) ((x) == '_' || (x) >='a' && (x) <= 'z' || (x) >= 'A' && (x) <= 'Z')
#define isop(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/')
#define isblank(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == 13)
#define tolow(s) do {int i = -1; while(s[++i]) s[i] = tolower(s[i]);} while(0)

// using namespace std;

std::ifstream *fin;

int ll;
int lc;
int cc;
int num;
char ch;
SYMBOL sym;
char token[30];
char stringbuff[500];
char current_line[300];

void nextCh() {
    while(cc == ll) {
        if(!fin->getline(current_line, 300)) {
            ch = -1;
            return;
        }
        current_line[strlen(current_line) + 1] = 0;
        current_line[strlen(current_line)] = '\n';
        lc++;
        cc = 0;
        ll = strlen(current_line);
    }
    ch = current_line[cc++];
}

void set_file_stream(char * filepath) {
    fin = new std::ifstream(filepath);
    if(!*fin) {
        error(27);
    }
    lc = 0;
    cc = 0;
    nextCh();
}


void nextSym() {
    if(sym == eof) error(26);

    while(isblank(ch)) nextCh();

    if(ch == '+') {
        sym = pluscon;
        nextCh();
    } else if(ch == '-') {
        sym = minuscon;
        nextCh();
    } else if(ch == '*') {
        sym = timescon;
        nextCh();
    } else if(ch == '/') {
        sym = divcon;
        nextCh();
    }


    else if(ch == '<') {
        sym = lss;
        nextCh();
        if(ch == '=') {
            sym = leq;
            nextCh();
        }
    } else if(ch == '>') {
        sym = gtr;
        nextCh();
        if(ch == '=') {
            sym = grq;
            nextCh();
        }
    } else if(ch == '=') {
        sym = becomes;
        nextCh();
        if(ch == '=') {
            sym = eql;
            nextCh();
        }
    } else if(ch == '!') {
        nextCh();
        if(ch == '=') {
            sym = neq;
            nextCh();
        } else error(1);
    }


    else if(ch == '(') {
        sym = lsmall;
        nextCh();
    } else if(ch == ')') {
        sym = rsmall;
        nextCh();
    } else if(ch == '[') {
        sym = lmedium;
        nextCh();
    } else if(ch == ']') {
        sym = rmedium;
        nextCh();
    } else if(ch == '{') {
        sym = lbig;
        nextCh();
    } else if(ch == '}') {
        sym = rbig;
        nextCh();
    }


    else if(ch == ',') {
        sym = comma;
        nextCh();
    } else if(ch == ';') {
        sym = semicolon;
        nextCh();
    } else if(ch == ':') {
        sym = colon;
        nextCh();
    }


    else if(isnum(ch)) {
        num = 0;
        do {
            num = num * 10 + ch - '0';
            nextCh();
        } while(isnum(ch));
        sym = intcon;
    }


    else if(ch == '\'') {
        nextCh();
        sym = charcon;
        if(isop(ch) || isnum(ch) || isalpha(ch)) {
            num = ch;
            nextCh();
            if(ch == '\'')nextCh();
            else error(0);
        } else error(0);
    } else if(ch == '"') {
        nextCh();
        sym = stringcon;
        int i = 0;
        while(ch >= 32 && ch <= 126 && ch != 34) {
            stringbuff[i++] = ch;
            nextCh();
        }
        stringbuff[i] = 0;
        if(ch == '"')nextCh();
        else error(0);
    }



    else if(isalpha(ch)) {
        int i = 0;
        do {
            token[i++] = ch;
            nextCh();
        } while(isalpha(ch) || isnum(ch));
        token[i] = 0;
        tolow(token);
        sym = ident;
        for (int i = 0; i < keywordCount; i++) {
            if(!strcmp(token, keywords[i])) {
                sym = keySet[i];
                break;
            }
        }
    }

    else if(ch == -1) {
        sym = eof;
    }

    else {
        error(1);
    }
}
