#include "symbolTable.h"


// using namespace std;

std::vector<TAB_ELEMENT> global_tab;
std::vector<std::vector<TAB_ELEMENT>> local_tab;
int display = -1;
int offset = 0;

const int FUNC_OFFSET = 3;// area for return value, return address, previous $fp

//std::vector<std::string> mcode;

std::ostream &operator << (std::ostream &out, TAB_ELEMENT ele) {
    const char *k = ele.kind == var ? " var" :
                    ele.kind == func ? "func" :
                    ele.kind == cons ? "cons" : "para";
    const char *t = ele.type == t_int ? " int" :
                    ele.type == t_char ? "char" :
                    ele.type == t_void ? "void" : "temp";
    char s[60];
    sprintf(s, "|%10s|\t%s|\t%s|\t%3d|\t%3d|", ele.ident, k, t, ele.length, ele.value);
    out << s;
    return out;
}


TAB_ELEMENT* enter(const char *ident, SYMBOL_KIND kind, SYMBOL_TYPE type, int length, int value, int lev) {
    TAB_ELEMENT t;
    strcpy(t.ident, ident);
    t.kind = kind;
    t.type = type;
    t.value = value;
    t.length = length;

    // if function, new display area
    if(kind == func) {
        t.value = ++display;
        local_tab.push_back(std::vector<TAB_ELEMENT>());
        offset = FUNC_OFFSET;
    }

    // if parameter or variable, raise offset
    if(kind == para || kind == var) {
        t.value = offset;
        if(length) offset += length;
        else ++offset;
    }

    if(lev) {
        local_tab[display].push_back(t);
        return &*(local_tab[display].end() - 1);
    } else {
        global_tab.push_back(t);
        return &*(global_tab.end() - 1);
    }

}

int lookup(char *ident, int local_flag, TAB_ELEMENT *element) {
    std::vector<TAB_ELEMENT>::iterator head, tail;
    if(local_flag) {
        head = local_tab[display].begin();
        tail = local_tab[display].end();
    } else {
        head = global_tab.begin();
        tail = global_tab.end();
    }
    for(std::vector<TAB_ELEMENT>::iterator iter = head; iter != tail; iter++) {
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

void show_tables() {
    char tab_head[60];
    sprintf(tab_head, "|%10s|\t%4s|\t%4s|\t%3s|\t%3s|", "ident", "kind", "type", "len", "val");
    std::cout << std::endl << "global_table: " << std::endl;
    std::cout << " ------------------------------------------ " << std::endl;
    std::cout << tab_head << std::endl;
    std::cout << " ------------------------------------------ " << std::endl;
    for(int i = 0; i < global_tab.size(); i++)
        std::cout << global_tab[i] << std::endl;
    std::cout << " ------------------------------------------ " << std::endl << std::endl << "local_table: " << std::endl;
    std::cout << " ------------------------------------------ " << std::endl;
    std::cout << tab_head << std::endl;
    for (int i = 0; i < local_tab.size(); i++) {
        std::cout << " --------------------" << i << "--------------------- " << std::endl;
        for (int j = 0; j < local_tab[i].size(); j++)
            std::cout << local_tab[i][j] << std::endl;
    }
    std::cout << " ------------------------------------------ " << std::endl;
}

void update_symbol_table(std::set<std::string> reserved_var) {
    for(int i = 0; i < local_tab.size(); i++) {
        for(std::vector<TAB_ELEMENT>::iterator iter = local_tab[i].begin(); iter != local_tab[i].end();) {
            if(reserved_var.find(std::string(iter->ident)) == reserved_var.end()) {
                iter = local_tab[i].erase(iter);
            } else iter++;
        }
        int offset = FUNC_OFFSET;
        for(int j = 0; j < local_tab[i].size(); j++) {
            if(local_tab[i][j].kind == var || local_tab[i][j].kind == para) {
                local_tab[i][j].value = offset;
                offset += local_tab[i][j].length == 0 ? 1 : local_tab[i][j].length;
            }
        }
    }

    for(std::set<std::string>::iterator iter = reserved_var.begin(); iter != reserved_var.end(); iter++) {
        int offset = 0;
        for(std::vector<TAB_ELEMENT>::reverse_iterator riter = global_tab.rbegin(); riter != global_tab.rend(); riter++) {
            if(riter->kind == var) {
                int last_off = riter->value;
                offset = riter->length ? last_off + riter->length : last_off + 1;
                break;
            }
        }
        if((*iter)[0] == '#' && (*iter)[2] == '_') {
            TAB_ELEMENT gtmp;
            strcpy(gtmp.ident, iter->c_str());

            gtmp.kind = var;
            gtmp.type = t_tmp;
            gtmp.value = offset++;
            gtmp.length = 0;
            global_tab.push_back(gtmp);
        }
    }
}
