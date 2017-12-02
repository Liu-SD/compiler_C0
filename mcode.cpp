#include "mcode.h"

#include "stdio.h"
#include <iostream>

std::map<std::string, int> label_tab;
std::vector<std::string> code_tab;

int mcode_lc() {return code_tab.size();}

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
    std::string label = "#Label_" + std::string(s);
    label_tab[label] = -1;
    return label;
}

int setLabelVal(std::string label, int val) {
    if(label_tab.find(label) != label_tab.end()){
        label_tab[label] = val;
        return 1;
    }
    return 0;
}

std::string newTmpVar() {
    static int tmpVar_no = 0;
    char s[4];
    sprintf(s, "%d", tmpVar_no);
    ++tmpVar_no;
    return "#t" + std::string(s);
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
