#include "coloring.h"

void coloring(vector<pair<string, string>>::iterator begin, vector<pair<string,string>>::iterator end) {
    // get local symbol table ltab
    vector<TAB_ELEMENT> ltab;
    int h = begin->second.find(' ') + 1;
    int t = begin->second.find('(');
    string func_name = begin->second.substr(h, t - h);
    for(int i = 0; i < global_tab.size(); i++) {
        if(global_tab[i].kind == func && !strcmp(global_tab[i].ident, func_name.c_str())) {
            ltab = local_tab[global_tab[i].value];
            break;
        }
    }

    // divide into blocks and link them
    vector<blk_link> blk_list;
    for(vector<pair<string, string>>::iterator iter = begin; iter != end; iter++) {

    }
}

void coloring_translate(vector<pair<string, string>> mcode) {
    vector<pair<string, string>>::iterator p = mcode.begin();
    for(vector<pair<string, string>>::iterator iter = mcode.begin(); iter != mcode.end(); iter++) {
        if(regex_match(iter->second, rfunc)) {
            if(p != iter)coloring(p, iter);
            p = iter;
        }
    }
    coloring(p, mcode.end());
}
