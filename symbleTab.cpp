#include "main.h"
#include <vector>
#include <string>
#include <string.h>

using namespace std;

vector<TAB_ELEMENT> global_tab;
vector<vector<TAB_ELEMENT> > local_tab;
int display = -1;

vector<string> mcode;

TAB_ELEMENT enter(char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev) {
    TAB_ELEMENT t;
    strcpy(t.ident, ident);
    t.kind = kind;
    t.type = type;
    t.value = value;
    t.length = length;
    if(lev) local_tab[display].push_back(t);
    else global_tab.push_back(t);
    if(kind == func) {
        ++display;
        local_tab.push_back(vector<TAB_ELEMENT>());
    }
    return t;
}

int lookup(char *ident, TAB_ELEMENT &element, int &lev) {
    for(vector<TAB_ELEMENT>::iterator iter = local_tab[display].begin(); iter != local_tab[display].end(); iter++) {
        if(!strcmp(iter->ident, ident)) {
            element = *iter;
            lev = 1;
            return 1;
        }
    }
    for (vector<TAB_ELEMENT>::iterator iter = global_tab.begin(); iter != global_tab.end(); iter++) {
        if(!strcmp(iter->ident, ident)) {
            element = *iter;
            lev = 0;
            return 1;
        }
    }
    return 0;
}
