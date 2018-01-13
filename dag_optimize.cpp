#include "dag_optimize.h"

vector<int> entrance;
vector<quadruple> mcode;
set<string> reserved_var;

vector<dag_link> node_list;
map<string, dag_link> node_map;
map<dag_link, vector<string>> reverse_node_map;

vector<pair<string, string>> optimize_codes;



void enter_mcode() {
    string s;
    int line_count = 0;
    EMBEDTAB embedtab = embeddingLabel();
    for(int i = 0; i <embedtab.size(); i++) {
        string label = embedtab[i].first;
        string code = embedtab[i].second;

        smatch sm;
        quadruple quad;
        quad.code = code;
        quad.label = label;

        if(!label.empty())
            entrance.push_back(line_count);

        if (regex_match(code, sm, rassign2_without_left_arr)) {
            quad.value = string(sm[1]);
            string s(sm[2]);
            if (regex_match(s, sm, rarr)) {
                quad.left = string(sm[1]);
                quad.right = string(sm[2]);
                quad.op = _arr_get;
            } else {
                quad.op = _null;
                quad.left = s;
            }
        } else if (regex_match(code, sm, rassign3)) {
            quad.value = string(sm[1]);
            quad.left = string(sm[2]);
            quad.right = string(sm[4]);
            string op(sm[3]);
            if (op == "+")quad.op = _add;
            else if (op == "-")quad.op = _sub;
            else if (op == "*")quad.op = _mul;
            else if (op == "/")quad.op = _div;
        } else {
            entrance.push_back(line_count);
            entrance.push_back(line_count + 1);
            if (regex_match(code, sm, rassign2_with_left_arr)) {
                quad.value = string(sm[1]);
                quad.left = string(sm[2]);
                quad.right = string(sm[3]);
            } else if (regex_match(code, sm, rbranch)) {
                quad.value = string(sm[2]);
            } else if (regex_match(code, sm, rretval)) {
                quad.value = string(sm[1]);
            } else if (regex_match(code, sm, rpush)) {
                quad.value = string(sm[1]);
            } else if (regex_match(code, sm, rprintv)) {
                quad.value = string(sm[2]);
            } else if (regex_match(code, sm, rscan)) {
                quad.value = string(sm[2]);
            }
        }
        mcode.push_back(quad);
        line_count++;
    }
    entrance.push_back(line_count);
}

void enter_var() {
    map<string, set<int>> varmap;
    for (int block_no = 0; block_no < entrance.size() - 1; block_no++) {
        for (int i = entrance[block_no]; i < entrance[block_no + 1]; i++) {
            string s[] = { mcode[i].value, mcode[i].left, mcode[i].right };
            for (int i = 0; i < 3; i++) {
                if (s[i].empty())
                    continue;
                if (s[i][0] == '-' || s[i][0] >= '0' && s[i][0] <= '9')
                    continue;
                if (s[i][0] != '#')
                    reserved_var.insert(s[i]);
                else {
                    varmap[s[i]].insert(block_no);
                    if (varmap[s[i]].size() > 1)
                        reserved_var.insert(s[i]);
                }
            }
        }
    }
}

void enter_code(string z, string x) {
    if(!optimize_codes.rbegin()->second.empty())
        optimize_codes.push_back(pair<string, string>());
    optimize_codes.rbegin()->second = z + " = " + x;
}

void enter_code(string z, string x, optype op, string y) {
    if(!optimize_codes.rbegin()->second.empty())
        optimize_codes.push_back(pair<string, string>());
    if(op == _arr_get)
        optimize_codes.rbegin()->second = z + " = " + x + "[" + y + "]";
    else {
        string op_str = op == _add ? "+":
                        op == _sub ? "-":
                        op == _mul ? "*": "/";
        optimize_codes.rbegin()->second =z + " = " + x + " " + op_str + " " + y;
    }
}

void enter_code(string code) {
    if(!optimize_codes.rbegin()->second.empty())
        optimize_codes.push_back(pair<string, string>());
    optimize_codes.rbegin()->second = code;
}

void enter_label(string label) {
    optimize_codes.push_back(pair<string, string>(label, string()));
}

string calculate(dag_link link) {
    if (link->leaf)
        return reverse_node_map[link][0];
    if (link->op == _null) {
        assert(link->left->leaf == true);
        assert(reverse_node_map[link->left].size() == 1);
        return reverse_node_map[link->left][0];
    }
    for (int i = 0; i < reverse_node_map[link].size(); i++) {
        if (reserved_var.find(reverse_node_map[link][i]) != reserved_var.end())
            return reverse_node_map[link][i];
    }
    string l_str = calculate(link->left);
    string r_str = calculate(link->right);
    enter_code(reverse_node_map[link][0], l_str, link->op, r_str);
    reserved_var.insert(reverse_node_map[link][0]);
    return reverse_node_map[link][0];
}



void DAG(vector<quadruple>::iterator begin, vector<quadruple>::iterator end) {

    //for (vector<quadruple>::iterator iter = begin; iter != end; iter++)
    //	cout << iter->code << endl;

    //cout << "==================" << endl;

    for (vector<quadruple>::iterator iter = begin; iter != end; iter++) {
        dag_link left;
        dag_link right;

        // find or create left node
        if (node_map.find(iter->left) == node_map.end()) {
            left = new dag_node;
            left->leaf = true;
            left->leaf_var_name = iter->left;
            left->left = left->right = NULL;
            left->op = _null;
            node_list.push_back(left);
            node_map[iter->left] = left;
        } else {
            left = node_map[iter->left];
        }

        // find or create right node if op not _null
        if (iter->op == _null)
            right = NULL;
        else {
            if (node_map.find(iter->right) == node_map.end()) {
                right = new dag_node;
                right->leaf = true;
                right->leaf_var_name = iter->right;
                right->left = right->right = NULL;
                right->op = _null;
                node_list.push_back(right);
                node_map[iter->right] = right;
            } else {
                right = node_map[iter->right];
            }
        }

        // value = left
        if (iter->op == _null) {
            if (left->leaf) {
                int index = -1;
                for (int i = 0; i < left->parent.size(); i++)
                    if (left->parent[i]->op == _null)
                        index = i;
                if (index >= 0)
                    node_map[iter->value] = left->parent[index];
                else {
                    dag_link value = new dag_node;
                    value->leaf = false;
                    value->left = left;
                    value->op = _null;
                    node_list.push_back(value);
                    left->parent.push_back(value);
                    node_map[iter->value] = value;
                }
            } else {
                node_map[iter->value] = left;
            }
        }
        // value = left op right
        else {
            bool flag = false;
            for (int i = 0; i < node_list.size(); i++) {
                if (node_list[i]->left == left && node_list[i]->right == right && node_list[i]->op == iter->op) {
                    flag = true;
                    node_map[iter->value] = node_list[i];
                    break;
                }

            }
            if (!flag) {
                dag_link value = new dag_node;
                value->leaf = false;
                value->left = left;
                value->right = right;
                value->op = iter->op;
                left->parent.push_back(value);
                right->parent.push_back(value);
                node_list.push_back(value);
                node_map[iter->value] = value;
            }
        }
    }


    vector<dag_link> Q;
    while (1) {
        dag_link p = NULL;
        // find node out of Q that parents all in Q
        for (int i = 0; i < node_list.size(); i++) {
            if (!node_list[i]->inQ && !node_list[i]->leaf) {
                bool allInQ = true;
                for (int j = 0; j < node_list[i]->parent.size(); j++) {
                    if (!node_list[i]->parent[j]->inQ) allInQ = false;
                }
                if (allInQ) {
                    p = node_list[i];
                    Q.push_back(p);
                    p->inQ = true;
                    break;
                }
            }
        }

        if (!p)break;

        while (1) {
            p = p->left;
            if (p->leaf)break;
            bool allInQ = true;
            for (int i = 0; i < p->parent.size(); i++) {
                if (!p->parent[i]->inQ)
                    allInQ = false;
            }
            if (!allInQ)break;
            Q.push_back(p);
            p->inQ = true;
        }
    }

    for (int i = 0; i < node_list.size(); i++)
        assert(node_list[i]->inQ || node_list[i]->leaf);

    for (map<string, dag_link>::iterator iter = node_map.begin(); iter != node_map.end(); iter++) {
        reverse_node_map[iter->second].push_back(iter->first);
    }
    static int tmp_var_index = 0;
    for (int i = 0; i < node_list.size(); i++) {
        if (reverse_node_map.find(node_list[i]) == reverse_node_map.end()) {
            char n[5];
            sprintf(n, "%d", tmp_var_index++);
            string s = "#t_" + string(n);
            reverse_node_map[node_list[i]].push_back(s);

            if(node_list[i]->leaf) {
                // if this node is leaf and origin leaf var move to other node,
                // then set a temp var to be it's value before it leaves leaf node
                enter_code(s, node_list[i]->leaf_var_name);
                reserved_var.insert(s);
            }
        }
    }

    node_list;
    node_map;
    reverse_node_map;

    for (int i = Q.size() - 1; i >= 0; i--) {
        dag_link node = Q[i];
        if (node->op == _null) {
            dag_link l = node->left;
            dag_link v = node;

            assert(reverse_node_map[l].size() == 1);
            assert(l->leaf);
            string l_str = calculate(l);
            for (int i = 0; i < reverse_node_map[v].size(); i++) {
                if (reserved_var.find(reverse_node_map[v][i]) != reserved_var.end()) {
                    enter_code(reverse_node_map[v][i], l_str);
                    // cout << reverse_node_map[v][i] << " = " << l_str << endl;
                }
            }
        } else {
            dag_link v = node;
            vector<string> reservedVarInV;
            for (int i = 0; i < reverse_node_map[v].size(); i++) {
                if (reserved_var.find(reverse_node_map[v][i]) != reserved_var.end()) {
                    reservedVarInV.push_back(reverse_node_map[v][i]);
                }
            }
            if (!reservedVarInV.empty()) {
                string l_str = calculate(v->left);
                string r_str = calculate(v->right);

                enter_code(reservedVarInV[0], l_str, v->op, r_str);
                // cout << reservedVarInV[0] << " = " << l_str << " " << v->op << " " << r_str << endl;
                for (int i = 1; i < reservedVarInV.size(); i++)
                    enter_code(reservedVarInV[i], reservedVarInV[0]);
                //cout << reservedVarInV[i] << " = " << reservedVarInV[0] << endl;
            }
        }
    }

    //cout << endl;
    for (int i = 0; i < node_list.size(); i++)
        delete node_list[i];

    node_list.clear();
    node_map.clear();
    reverse_node_map.clear();


}


void dag_optimize() {
    enter_mcode();
    enter_var();

    optimize_codes.clear();
    optimize_codes.push_back(pair<string, string>());

    for (int i = 0; i < entrance.size() - 1; i++) {
        if (entrance[i + 1] - entrance[i] > 1) {
            if(!mcode[entrance[i]].label.empty())
                enter_label(mcode[entrance[i]].label);
            DAG(mcode.begin() + entrance[i], mcode.begin() + entrance[i + 1]);
            //break;
        } else if(entrance[i + 1] - entrance[i] == 1) {
            if(!mcode[entrance[i]].label.empty())
                enter_label(mcode[entrance[i]].label);
            enter_code(mcode[entrance[i]].code);
        }
    }
}

void printMcode_optimized(string path) {
    std::ofstream fout(path);
    for(int i = 0; i < optimize_codes.size(); i++) {
        char s[5000];
        sprintf(s, "%10s    %s\n", optimize_codes[i].first.c_str(), optimize_codes[i].second.c_str());
        fout << s;
    }

}
