#include "coloring.h"

#include <cstdlib>
#include <cassert>

vector<blk_link> blk_list;

vector<TAB_ELEMENT> ltab;
map<string, int> local_var_pool;
map<string, int> global_const_pool;
map<string, int> local_const_pool;
map<string, int> var_reg;


const int reg_start = 10;
const int reg_end = 25;
const int reg_count = reg_end - reg_start + 1;

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
    if(var[0] == '#' && var[2] == '_') {
        blk->def.insert(var);
        return;
    }
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

void update_active() { // active = in U out = use U out
    for(int i = 0; i < blk_list.size(); i++) {
        for(set<string>::iterator iter = blk_list[i]->use.begin(); iter != blk_list[i]->use.end(); iter++)
            blk_list[i]->active.insert(*iter);
        for(set<string>::iterator iter = blk_list[i]->out.begin(); iter != blk_list[i]->out.end(); iter++)
            blk_list[i]->active.insert(*iter);
    }
}

void build_blk_list(vector<pair<string, string>>::iterator begin, vector<pair<string,string>>::iterator end) {

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
                    //cout << i << "->" << j << endl;
                    break;
                }
            }
        } else if(!blk_list[i]->branch_label.empty()) {
            for(int j = 0; j < blk_list.size(); j++) {
                if(blk_list[i]->branch_label == blk_list[j]->label) {
                    blk_list[i]->successor.insert(blk_list[j]);
                    blk_list[j]->precursor.insert(blk_list[i]);
                    //cout << i << "->" << j << endl;
                    break;
                }
            }
            blk_list[i]->successor.insert(blk_list[i + 1]);
            blk_list[i + 1]->precursor.insert(blk_list[i]);
            //cout << i << "->" << i + 1 << endl;
        } else if(blk_list[i]->ret_blk) {
            //cout << i << "->exit" << endl;
        } else {
            blk_list[i]->successor.insert(blk_list[i + 1]);
            blk_list[i + 1]->precursor.insert(blk_list[i]);
            //cout << i << "->" << i + 1 << endl;
        }
    }
    //cout << "block count: " << blk_list.size() << endl;
}

void build_conflict_map() {
    int var_count = 0;
    map<string, int> var_index;
    map<int, string> index_var;
    for(int i = 0; i < blk_list.size(); i++)
        for(set<string>::iterator iter = blk_list[i]->active.begin(); iter != blk_list[i]->active.end(); iter++)
            if(var_index.find(*iter) == var_index.end()) {
                index_var[var_count] = *iter;
                var_index[*iter] = var_count++;
                // cout << var_count - 1 << ":\t" << *iter << endl;
            }

    int **matrix = new int*[var_count];
    for(int i = 0; i < var_count; i++) {
        matrix[i] = new int[var_count + 1];
        memset(matrix[i], 0, (var_count + 1) * sizeof(int));
    }


    for(int i = 0; i < blk_list.size(); i++)
        for(set<string>::iterator xiter = blk_list[i]->active.begin(); xiter != blk_list[i]->active.end(); xiter++)
            for(set<string>::iterator yiter = blk_list[i]->active.begin(); yiter != blk_list[i]->active.end(); yiter++)
                matrix[var_index[*xiter]][var_index[*yiter]] = 1;

    for(int i = 0; i < var_count; i++) {
        int sum = 0;
        for(int j = 0; j < var_count; j++)
            sum += matrix[i][j];
        matrix[i][var_count] = sum - 1;
    }



    int *picked = new int[var_count];
    memset(picked, 0, var_count * sizeof(int));
    int picked_no = 0;
    int *record = new int[var_count];
    memset(record, 0, var_count * sizeof(int));
    while(picked_no < var_count) {
        // remove and record;
        for(int i = 0; i < var_count; i++) {
            if(picked[i]) continue;
            if(matrix[i][var_count] < reg_count) {
                picked[i] = ++picked_no;
                record[i] = 1;
                for(int j = 0; j < var_count; j++)
                    if(i != j && matrix[i][j])
                        matrix[j][var_count]--;
                i = -1;
            }
        }
        // remove but don't record;
        int max_node_count = -1;
        int max_node_index = -1;
        for(int i = 0; i < var_count; i++) {
            if(picked[i])continue;
            if(max_node_count < matrix[i][var_count]) {
                max_node_count = matrix[i][var_count];
                max_node_index = i;
            }
        }
        if(max_node_index == -1)
            continue;
        picked[max_node_index] = ++picked_no;
        record[max_node_index] = 0;
        for(int j = 0; j < var_count; j++)
            if(max_node_index != j && matrix[max_node_index][j])
                matrix[j][var_count]--;
    }
    /*
    for(int i = 0; i < var_count; i++) {
        for(int j = 0; j <= var_count; j++)
            cout << matrix[i][j] << ' ' ;
        cout << endl;
    }
    */
    int *range = new int[var_count];
    for(int i = 0; i < var_count; i++)
        range[picked[i] - 1] = i;

    int *regs = new int[var_count];
    memset(regs, 0, reg_count * sizeof(int));
    for(int i = var_count - 1; i >= 0; i--) {
        int v = range[i];
        if(!record[v])continue;
        for(int j = reg_start; j <= reg_end; j++) {
            bool flg = false;
            for(int k = 0; k < var_count; k++) {
                if(matrix[v][k] && regs[k] == j)
                    flg = true;
            }
            if(!flg) {
                regs[v] = j;
                var_reg[index_var[v]] = j;
                break;
            }
        }
    }
    /*
    for(int i = 0; i < var_count; i++)
        cout << regs[i] << ' ';
    cout << endl;
    */
    for(map<string, int>::iterator iter = var_reg.begin(); iter != var_reg.end(); iter++)
        cout << iter->first << ": " << iter->second << endl;

    for(int i = 0; i < var_count; i++)
        delete[] matrix[i];
    delete[] matrix;
    delete[] picked;
    delete[] record;
    delete[] range;
    delete[] regs;
}

void distribute_reg_to_blks() {
    // for each block:
    //     { active } -> register distributed in map<string, int>var_reg
    //     { def - out } -> { all register - above }
    for(int i = 0; i < blk_list.size(); i++) {
        blk_link blk = blk_list[i];

        set<string> def_sub_out;
        for(set<string>::iterator iter = blk->def.begin(); iter != blk->def.end(); iter++)
            if(blk->out.find(*iter) == blk->out.end())
                def_sub_out.insert(*iter);

        int used[reg_end + 1];
        memset(used, 0, (reg_end + 1) * sizeof(int));
        for(set<string>::iterator iter = blk->active.begin(); iter != blk->active.end(); iter++) {
            if(var_reg.find(*iter) == var_reg.end())continue;
            int x = var_reg[*iter];
            blk->register_distribute[*iter] = x;
            used[x] = 1;
        }
        for(set<string>::iterator iter = def_sub_out.begin(); iter != def_sub_out.end(); iter++) {
            for(int j = reg_start; j <= reg_end; j++)
                if(!used[j]) {
                    blk->register_distribute[*iter] = j;
                    used[j] = 1;
                    break;
                }
        }
    }
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
        if(ltab[i].kind == var && !ltab[i].length || ltab[i].kind == para)
            local_var_pool[string(ltab[i].ident)] = i;
        else if(ltab[i].kind == cons)
            local_const_pool[string(ltab[i].ident)] = ltab[i].value;

    // throw away function declare
    ++begin;

    // divide into blocks and link them
    blk_list.clear();
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
    /* still have bug
    for(vector<blk_link>::iterator i = blk_list.begin(); i != blk_list.end(); i++)
        if((*i)->precursor.empty()) {
            for(set<blk_link>::iterator j = (*i)->precursor.begin(); j != (*i)->precursor.end(); j++)
                (*j)->precursor.erase(*i);
            blk_list.erase(i);
            delete *i;
            i = blk_list.begin() - 1;
        }
    */

    // update in and out set
    while(1) {
        bool updated = false;
        for(vector<blk_link>::reverse_iterator riter = blk_list.rbegin(); riter != blk_list.rend(); riter++) {
            update_out(*riter);
            if(update_in(*riter))updated = true;
        }
        if(!updated)break;
    }
    update_active();

    /*
    int i = 0;
    for(vector<blk_link>::iterator iter = blk_list.begin(); iter != blk_list.end(); iter++) {

        //if(!(*iter)->label.empty())
        //    cout << (*iter)->label << endl;
        //for(int i = 0; i < (*iter)->code.size(); i++) {
        //    cout << (*iter)->code[i] << endl;
        //}

        cout << "block " << i++ <<endl;
        cout << "use:" << endl;
        for(set<string>::iterator i = (*iter)->use.begin(); i != (*iter)->use.end(); i++)
            cout << ' ' << *i;
        cout << endl << "def:" << endl;
        for(set<string>::iterator i = (*iter)->def.begin(); i != (*iter)->def.end(); i++)
            cout << ' ' << *i;
        cout <<endl << "out:" << endl;
        for(set<string>::iterator i = (*iter)->out.begin(); i != (*iter)->out.end(); i++)
            cout << ' ' << *i;
        cout << endl << "in:" << endl;
        for(set<string>::iterator i = (*iter)->in.begin(); i != (*iter)->in.end(); i++)
            cout << ' ' << *i;
        cout << endl;
        cout  << "=========================" << endl;
    }
    */

    var_reg.clear();
    build_conflict_map();

    distribute_reg_to_blks();

    for(int i = 0; i < blk_list.size(); i++) {
        cout << "block " << i << endl;
        for(map<string, int>::iterator iter = blk_list[i]->register_distribute.begin(); iter != blk_list[i]->register_distribute.end(); iter++)
            cout << iter->first << ": " << iter->second <<endl;
        cout << endl;
    }

    to_tcode(func_name);

    // system("pause");
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
