#include "mcode.h"


std::map<std::string, int> label_tab;
std::vector<std::string> code_tab;

int mcode_lc() {
    return code_tab.size();
}

std::string int2str(int i) {
    char s[10];
    sprintf(s, "%d", i);
    return std::string(s);
}

std::string newLabel() {
    static int label_no = 0;
    char s[4];
    sprintf(s, "%d", label_no);
    ++label_no;
    std::string label = "#Label_" + std::string(s) + "_";
    label_tab[label] = -1;
    return label;
}

int setLabelVal(std::string label, int val) {
    if(label_tab.find(label) != label_tab.end()) {
        label_tab[label] = val;
        return 1;
    }
    return 0;
}

std::string newTmpVar() {
    static int tmpVar_no = 0;
    char s[7];
    sprintf(s, "%d", tmpVar_no);
    ++tmpVar_no;
    std::string varname = "#t" + std::string(s);
    enter(varname.c_str(), var, t_tmp, 0, 0, 1);
    return varname;
}

void emit(std::string z, std::string x, std::string op, std::string y) {
    code_tab.push_back(z + " = " + x + " " + op + " " + y);
}

void emit(std::string x, std::string y, std::string z) {
    code_tab.push_back(x + " " + y + " " + z);
}

void emit(std::string x, std::string y) {
    code_tab.push_back(x + " " + y);
}

void emit(std::string x) {
    code_tab.push_back(x);
}

void showMcode() {
    for(int i = 0; i < mcode_lc(); i++) {
        std::cout << i << "\t" << code_tab[i] << std::endl;
    }
    std::cout << std::endl;

    for(std::map<std::string, int>::iterator iter = label_tab.begin(); iter != label_tab.end(); iter++) {
        std::cout << iter->first << "\t" << iter->second << std::endl;
    }
}


EMBEDTAB embeddingLabel() {
    EMBEDTAB embed;
    for (std::vector<std::string>::iterator iter = code_tab.begin(); iter != code_tab.end(); ++iter) {
        std::pair<std::string, std::string> p (std::string(), *iter);
        embed.push_back(p);
    }
    // embed.push_back(std::pair<std::string, std::string> (std::string(), "BREAK"));
    for (std::map<std::string, int>::iterator iter= label_tab.begin(); iter != label_tab.end(); ++iter) {
        if(embed[iter->second].first.empty()) {
            embed[iter->second].first = iter->first;
        } else {
            std::string origin = embed[iter->second].first;
            std::string replaced = iter->first;
            for (EMBEDTAB::iterator iter = embed.begin(); iter != embed.end(); ++iter) {
                int pos = iter->second.find(replaced);
                if(pos != std::string::npos) {
                    iter->second = iter->second.replace(pos, replaced.size(), origin);
                }
            }
        }
    }
    return embed;
}

void printMcode(std::string path) {
    std::ofstream fout(path);
    EMBEDTAB code = embeddingLabel();
    // std::cout << std::endl;
    for(int i = 0; i < code.size(); i++) {
        char s[100];
        sprintf(s, "%10s    %s\n", code[i].first.c_str(), code[i].second.c_str());
        fout << s;
    }
}
