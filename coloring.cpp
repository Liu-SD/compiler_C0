#include "coloring.h"

#include <cstdlib>
#include <cassert>

vector<blk_link> blk_list;

vector<TAB_ELEMENT> ltab;
map<string, int> local_var_pool;
map<string, int> global_const_pool;
map<string, int> local_const_pool;

void enter_use(blk_link blk, string var) {
    if(local_const_pool.find(var) != local_const_pool.end()) {
        string cd = *blk->code.rbegin();
        int i = cd.find(var);
        *blk->code.rbegin() = cd.replace(i, var.size(), int2str(local_const_pool[var]));
        return;
    }
    if(global_const_pool.find(var) != global_const_pool.end()) {
        string cd = *blk->code.rbegin();
        int i = cd.find(var);
        *blk->code.rbegin() = cd.replace(i, var.size(), int2str(global_const_pool[var]));
        return;
    }
    if(local_var_pool.find(var) == local_var_pool.end())
        return;
    if(blk->def.find(var) != blk->def.end())
        return;
    blk->use.insert(var);
    blk->in.insert(var); // in = use U (out - def) all var that is in use must be in in
}

void enter_def(blk_link blk, string var) {
    if(local_var_pool.find(var) == local_var_pool.end())
        return;
    if(blk->use.find(var) != blk->use.end())
        return;
    blk->def.insert(var);
}

void update_out(blk_link blk) {
    for(set<blk_link>::iterator i = blk->successor.begin(); i != blk->successor.end(); i++)
        for(set<string>::iterator j = (*i)->in.begin(); j != (*i)->in.end(); j++)
            blk->out.insert(*j);
}

bool update_in(blk_link blk) {
    int sz = blk->in.size();
    for(set<string>::iterator iter = blk->out.begin(); iter != blk->out.end(); iter++) {
        if(blk->def.find(*iter) == blk->def.end())
            blk->in.insert(*iter);
    }
    return sz < blk->in.size();
}

void build_blk_list(vector<pair<string, string>>::iterator begin, vector<pair<string,string>>::iterator end) {
    blk_list.clear();
    blk_link blk = NULL;
    for(vector<pair<string, string>>::iterator iter = begin; iter != end; iter++) {
        if(!iter->first.empty() || iter == begin) {
            blk = new blk_node;
            blk->label = iter->first;
            blk_list.push_back(blk);
        }

        smatch sm;
        blk->code.push_back(iter->second);
        if(regex_match(iter->second, sm, rbranch)) {
            // blk->code.push_back(iter->second);
            enter_use(blk, string(sm[2]));
            blk->branch_label = "#" + string(sm[3]);
            blk = new blk_node;
            blk_list.push_back(blk);
        } else if(regex_match(iter->second, sm, rjump)) {
            // blk->code.push_back(iter->second);
            blk->jump_label = "#" + string(sm[1]);
            blk = new blk_node;
            blk_list.push_back(blk);
        } else if(regex_match(iter->second, sm, rreturn)) {
            blk->ret_blk = true;
            blk = new blk_node;
            blk_list.push_back(blk);
        } else if(regex_match(iter->second, sm, rassign2)) {
            string left(sm[1]);
            string right(sm[2]);
            if(regex_match(left, sm, rarr)) {
                enter_use(blk, string(sm[2]));
                enter_use(blk, right);
                enter_def(blk, string(sm[1]));
            } else if(regex_match(right, sm, rarr)) {
                enter_use(blk, string(sm[1]));
                enter_use(blk, string(sm[2]));
                enter_def(blk, left);
            } else {
                enter_use(blk, right);
                enter_def(blk, left);
            }
        } else if(regex_match(iter->second, sm, rassign3)) {
            enter_use(blk, string(sm[2]));
            enter_use(blk, string(sm[4]));
            enter_def(blk, string(sm[1]));
        } else if (regex_match(iter->second, sm, rretval)) {
            enter_use(blk, string(sm[1]));
        } else if(regex_match(iter->second, sm, rpush)) {
            enter_use(blk, string(sm[1]));
        } else if(regex_match(iter->second, sm, rprintv)) {
            enter_use(blk, string(sm[2]));
        } else if(regex_match(iter->second, sm, rscan)) {
            enter_def(blk, string(sm[2]));
        }
    }
}

void link_blk_list() {
    assert(blk_list[blk_list.size() - 1]->ret_blk);
    for(int i = 0; i < blk_list.size(); i++) {
        if(!blk_list[i]->jump_label.empty()) {
            for(int j = 0; j < blk_list.size(); j++) {
                if(blk_list[i]->jump_label == blk_list[j]->label) {
                    blk_list[i]->successor.insert(blk_list[j]);
                    blk_list[j]->precursor.insert(blk_list[i]);
                    cout << i << "->" << j << endl;
                    break;
                }
            }
        } else if(!blk_list[i]->branch_label.empty()) {
            for(int j = 0; j < blk_list.size(); j++) {
                if(blk_list[i]->branch_label == blk_list[j]->label) {
                    blk_list[i]->successor.insert(blk_list[j]);
                    blk_list[j]->precursor.insert(blk_list[i]);
                    cout << i << "->" << j << endl;
                    break;
                }
            }
            blk_list[i]->successor.insert(blk_list[i + 1]);
            blk_list[i + 1]->precursor.insert(blk_list[i]);
            cout << i << "->" << i + 1 << endl;
        } else if(blk_list[i]->ret_blk) {
            cout << i << "->exit" << endl;
        } else {
            blk_list[i]->successor.insert(blk_list[i + 1]);
            blk_list[i + 1]->precursor.insert(blk_list[i]);
            cout << i << "->" << i + 1 << endl;
        }
    }
    cout << "block count: " << blk_list.size() << endl;
}

void coloring(vector<pair<string, string>>::iterator begin, vector<pair<string,string>>::iterator end) {
    // get local symbol table ltab
    int h = begin->second.find(' ') + 1;
    int t = begin->second.find('(');
    string func_name = begin->second.substr(h, t - h);
    for(int i = 0; i < global_tab.size(); i++) {
        if(global_tab[i].kind == func && !strcmp(global_tab[i].ident, func_name.c_str())) {
            ltab = local_tab[global_tab[i].value];
            break;
        }
    }

    local_const_pool.clear();
    local_var_pool.clear();
    for(int i = 0; i < ltab.size(); i++)
        if(ltab[i].kind == var || ltab[i].kind == para)
            local_var_pool[string(ltab[i].ident)] = i;
        else if(ltab[i].kind == cons)
            local_const_pool[string(ltab[i].ident)] = ltab[i].value;

    // throw away function declare
    ++begin;

    // divide into blocks and link them
    build_blk_list(begin, end);



    // erase empty block
    for(vector<blk_link>::iterator iter = blk_list.begin(); iter != blk_list.end(); iter++) {
        if(!(*iter)->code.size()) {
            delete *iter;
            iter = blk_list.erase(iter) - 1;
        }
    }

    // link nodes
    link_blk_list();

    // delete unreachable node
    /*
    for(vector<blk_link>::iterator iter = blk_list.begin(); iter != blk_list.end(); iter++)
        if((*iter)->precursor.empty()) {
            delete *iter;
            iter = blk_list.erase(iter);
        }
    */

    // update in and out set
    while(1) {
        bool updated = false;
        for(vector<blk_link>::reverse_iterator riter = blk_list.rbegin(); riter != blk_list.rend(); riter++) {
            update_out(*riter);
            updated = update_in(*riter);
        }
        if(!updated)break;
    }


    int i = 0;
    for(vector<blk_link>::iterator iter = blk_list.begin(); iter != blk_list.end(); iter++) {
            /*
            if(!(*iter)->label.empty())
                cout << (*iter)->label << endl;
            for(int i = 0; i < (*iter)->code.size(); i++) {
                cout << (*iter)->code[i] << endl;
            }
            */
            cout << "block " << i++ <<endl;
            cout << "use:" << endl;
            for(set<string>::iterator i = (*iter)->use.begin(); i != (*iter)->use.end(); i++)
                cout << '\t' << *i;
            cout << endl << "def:" << endl;
            for(set<string>::iterator i = (*iter)->def.begin(); i != (*iter)->def.end(); i++)
                cout << '\t' << *i;
            cout <<endl << "out:" << endl;
            for(set<string>::iterator i = (*iter)->out.begin(); i != (*iter)->out.end(); i++)
                cout << '\t' << *i;
            cout << endl << "in:" << endl;
            for(set<string>::iterator i = (*iter)->in.begin(); i != (*iter)->in.end(); i++)
                cout << '\t' << *i;
            cout << endl;
            cout  << "=========================" << endl;

    }

    system("pause");
}

void coloring_translate(vector<pair<string, string>> mcode) {
    for(int i = 0; i < global_tab.size(); i++)
        if(global_tab[i].kind == cons)
        global_const_pool[string(global_tab[i].ident)] = global_tab[i].value;

    vector<pair<string, string>>::iterator p = mcode.begin();
    for(vector<pair<string, string>>::iterator iter = mcode.begin(); iter != mcode.end(); iter++) {
        if(regex_match(iter->second, rfunc)) {
            if(p != iter)coloring(p, iter);
            p = iter;
        }
    }
    coloring(p, mcode.end());
}
