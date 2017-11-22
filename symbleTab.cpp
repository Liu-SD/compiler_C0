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

int lookup(char *ident, int local_flag, TAB_ELEMENT *element) {
    vector<TAB_ELEMENT>::iterator head, tail;
    if(local_flag) {
        head = local_tab[display].begin();
        tail = local_tab[display].end();
    } else {
        head = global_tab.begin();
        tail = global_tab.end();
    }
    for(vector<TAB_ELEMENT>::iterator iter = head; iter != tail; iter++) {
        if(!strcmp(iter->ident, ident)) {
            if(element) {
                element->kind = iter->kind;
                element->type = iter->type;
                strcpy(element->ident, iter->ident);
                element->value = iter->value;
                element->length = iter->length;
            }
            return 1;
        }
    }
    return 0;
}
