#include "coloring.h"

#include <cassert>

#define OFFSET_RET_VALUE_ 0
#define OFFSET_RET_ADDR_ 1
#define OFFSET_PREV_FP_ 2
#define DISPLAY_SIZE_ 3
#define REGISTER_SIZE_ reg_count

#define WORD_WIDTH_ 4
#define DATA_BASE_ADDR_ 0x10040000

vector<string> tcode_data;
vector<string> tcode_text;

map<string, int> reg_dis;

int blksz; // block size, not include register size

void ett(string s) {
    tcode_text.push_back(s);
}

string etd(string s) {
    static int str_cnt = 0;
    string ss = "str" + int2str(str_cnt) + ": .asciiz \"" + s + "\"";
    tcode_data.push_back(ss);
    return "str" + int2str(str_cnt++);
}

string load(int reg, string v) {
    if(v == "RET") {
        ett("lw $" + int2str(reg) + " " + int2str(4 * (blksz + REGISTER_SIZE_ + OFFSET_RET_VALUE_)) + "($fp)");
        return "$" + int2str(reg);
    }
    if(reg_dis.find(v) != reg_dis.end())
        return "$" + int2str(reg_dis[v]);
    for(int i = 0; i < ltab.size(); i++) {
        if(v == string(ltab[i].ident)) {
            ett("lw $" + int2str(reg) + " " + int2str(4 * ltab[i].value) + "($fp)");
            return "$" + int2str(reg);
        }
    }
    for(int i = 0; i < global_tab.size(); i++) {
        if(v == string(global_tab[i].ident)) {
            ett("lw $" + int2str(reg) + " " + int2str(4 * global_tab[i].value) + "($gp)");
            return "$" + int2str(reg);
        }
    }
    assert(0);
}

void loadArr(int reg, string v) {
    for(int i = 0; i < ltab.size(); i++) {
        if(v == string(ltab[i].ident)) {
            ett("la $" + int2str(reg) + " " + int2str(4 * ltab[i].value) + "($fp)");
            return;
        }
    }
    for(int i = 0; i < global_tab.size(); i++) {
        if(v == string(global_tab[i].ident)) {
            ett("la $" + int2str(reg) + " " + int2str(4 * global_tab[i].value) + "($gp)");
            return;
        }
    }
    assert(0);
}

void setblksz() {
    for(vector<TAB_ELEMENT>::reverse_iterator riter = ltab.rbegin(); riter != ltab.rend(); riter++) {
        if(riter->kind == var || riter->kind == para) {
            blksz = riter->length ? riter->value + riter->length : riter->value + 1;
            return;
        }
    }
    blksz = DISPLAY_SIZE_;
}

bool inRegister(string v) {
    return reg_dis.find(v) != reg_dis.end();
}

string regName(string v) {
    assert(reg_dis.find(v) != reg_dis.end());
    return "$" + int2str(reg_dis[v]);
}

bool isConst(string v) {
    return v[0] == '-' || v[0] >= '0' && v[0] <= '9';
}

string varOff(string v) {
    for(int i = 0; i < ltab.size(); i++)
        if(v == string(ltab[i].ident))
            return int2str(ltab[i].value * 4) + "($fp)";
    for(int i = 0; i < global_tab.size(); i++)
        if(v == string(global_tab[i].ident))
            return int2str(global_tab[i].value * 4) + "($gp)";
    assert(0);
}

int str2int(string v) {
    int n = 0;
    sscanf(v.c_str(), "%d", &n);
    return n;
}



void to_tcode(string func_name) {
    static bool setHead = false;
    if(!setHead) {
        ett("li $gp " + int2str(DATA_BASE_ADDR_));
        int off = 0;
        for(int i = global_tab.size() - 1; i >= 0; i--) {
            if(global_tab[i].kind == var) {
                off = global_tab[i].length ? global_tab[i].value + 1 : global_tab[i].value + global_tab[i].length;
                break;
            }
        }
        ett("li $fp " + int2str(DATA_BASE_ADDR_ + 4 * off));
        ett("jal main");
        ett("li $v0 10");
        ett("syscall");
        setHead = true;
    }

    setblksz();
    ett(func_name + ":");
    ett("sw $ra " + int2str(OFFSET_RET_ADDR_ * WORD_WIDTH_) + "($fp)");

    reg_dis = blk_list[0]->register_distribute;
    for(set<string>::iterator iter = blk_list[0]->in.begin(); iter != blk_list[0]->in.end(); iter++)
        if(inRegister(*iter))
            ett("lw " + regName(*iter) + " " + varOff(*iter));

    for(int i = 0; i < blk_list.size(); i++) {
        //ett("#==========");
        reg_dis = blk_list[i]->register_distribute;
        if(!blk_list[i]->label.empty())
            ett(blk_list[i]->label.substr(1, 9527) + ":");
        for(int j = 0; j < blk_list[i]->code.size(); j++) {
            string cd = blk_list[i]->code[j];
            //ett("");
            //ett("#" + cd);
            smatch sm;
            if(regex_match(cd, sm, rreturn)) {
                ett("lw $ra " + int2str(OFFSET_RET_ADDR_ * WORD_WIDTH_) + "($fp)");
                ett("lw $fp " + int2str(OFFSET_PREV_FP_ * WORD_WIDTH_) + "($fp)");
                ett("jr $ra");
            } else if(regex_match(cd, sm, rassign2_without_left_arr)) {
                string a(sm[1]);
                string right(sm[2]);
                if(regex_match(right, sm, rarr)) { // a = x[b]
                    string x(sm[1]);
                    string b(sm[2]);

                    loadArr(8, x);
                    if(isConst(b)) {
                        ett("addi $8 $8 " + int2str(str2int(b) * 4));
                    } else {
                        string b_reg = load(9, b);
                        ett("mul $9 " + b_reg + " 4");
                        ett("add $8 $8 $9");
                    }
                    if(inRegister(a)) {
                        ett("lw " + regName(a) + " 0($8)");
                    } else {
                        ett("lw $8 0($8)");
                        ett("sw $8 " + varOff(a));
                    }

                } else { // a = b or a = RET
                    string b(right);
                    if(isConst(b)) {
                        if(inRegister(a)) {
                            ett("li " + regName(a) + " " + b);
                        } else {
                            ett("li $8 " + b);
                            ett("sw $8 " + varOff(a));
                        }
                    } else {
                        string b_reg = load(8, b);
                        if(inRegister(a)) {
                            ett("move " + regName(a) + " " + b_reg);
                        } else {
                            ett("sw " + b_reg + " " + varOff(a));
                        }
                    }
                }
            } else if(regex_match(cd, sm, rassign2_with_left_arr)) { // x[a] = b
                string x(sm[1]);
                string a(sm[2]);
                string b(sm[3]);
                loadArr(8, x);
                if(isConst(a)) {
                    ett("addi $8 $8 " + int2str(str2int(a) * 4));
                } else {
                    string reg_a = load(9, a);
                    ett("mul $9 " + reg_a + " 4");
                    ett("add $8 $8 $9");
                }

                if(isConst(b)) {
                    ett("li $9 " + b);
                    ett("sw $9 0($8)");
                } else {
                    string reg_b = load(9, b);
                    ett("sw " + reg_b + " 0($8)");
                }
            } else if(regex_match(cd, sm, rassign3)) { // a = b op c
                string a(sm[1]);
                string b(sm[2]);
                string op(sm[3]);
                string c(sm[4]);
                if(isConst(b) && isConst(c)) {
                    int val = op == "+" ? (str2int(b) + str2int(c)) :
                              op == "-" ? (str2int(b) - str2int(c)) :
                              op == "*" ? (str2int(b) * str2int(c)) :
                              (str2int(b) / str2int(c));
                    if(inRegister(a)) {
                        ett("li " + regName(a) + " " + int2str(val));
                    } else {
                        ett("li $8 " + int2str(val));
                        ett("sw $8 " + varOff(a));
                    }
                } else if(isConst(b)) {
                    string reg_c = load(8, c);
                    if(op == "+") {
                        if(inRegister(a))
                            ett("addi " + regName(a) + " " + reg_c + " " + b);
                        else {
                            ett("addi $8 " + reg_c + " " + b);
                            ett("sw $8 " + varOff(a));
                        }
                    } else if(op == "-") {
                        if(inRegister(a)) {
                            ett("li " + regName(a) + " " + b);
                            ett("sub " + regName(a) + " " + regName(a) + " " + reg_c);
                        } else {
                            ett("li $9 " + b);
                            ett("sub $8 $9 " + reg_c);
                            ett("sw $8 " + varOff(a));
                        }
                    } else if(op == "*") {
                        if(inRegister(a))
                            ett("mul " + regName(a) + " " + reg_c + " " + b);
                        else {
                            ett("mul $8 " + reg_c + " " + b);
                            ett("sw $8 " + varOff(a));
                        }
                    } else if(op == "/") {
                        if(inRegister(a)) {
                            ett("li " + regName(a) + " " + b);
                            ett("div " + regName(a) + " " + regName(a) + " " + reg_c);
                        } else {
                            ett("li $9 " + b);
                            ett("div $8 $9 " + reg_c);
                            ett("sw $8 " + varOff(a));
                        }
                    }
                } else if(isConst(c)) {
                    string reg_b = load(8, b);
                    if(inRegister(a)) {
                        if(op == "+")ett("addi " + regName(a) + " " + reg_b + " " + c);
                        else if(op == "-")ett("subi " + regName(a) + " " + reg_b + " " + c);
                        else if(op == "*")ett("mul " + regName(a) + " " + reg_b + " " + c);
                        else ett("div " + regName(a) + " " + reg_b + " " + c);
                    } else {
                        if(op == "+")ett("addi $8 " + reg_b + " " + c);
                        else if(op == "-")ett("subi $8 " + reg_b + " " + c);
                        else if(op == "*")ett("mul $8 " + reg_b + " " + c);
                        else ett("div $8 " + reg_b + " " +c);
                        ett("sw $8 " + varOff(a));
                    }
                } else {
                    string reg_b = load(8, b);
                    string reg_c = load(9, c);
                    if(inRegister(a)) {
                        if(op == "+")ett("add " + regName(a) + " " + reg_b + " " + reg_c);
                        else if(op == "-")ett("sub " + regName(a) + " " + reg_b + " " + reg_c);
                        else if(op == "*")ett("mul " + regName(a) + " " + reg_b + " " + reg_c);
                        else ett("div " + regName(a) + " " + reg_b + " " + reg_c);
                    } else {
                        if(op == "+")ett("add $8 " + reg_b + " " + reg_c);
                        else if(op == "-")ett("sub $8 " + reg_b + " " + reg_c);
                        else if(op == "*")ett("mul $8 " + reg_b + " " + reg_c);
                        else ett("div $8 " + reg_b + " " +reg_c);
                        ett("sw $8 " + varOff(a));
                    }
                }
            } else if(regex_match(cd, sm, rbranch)) { // b v l
                string b(sm[1]);
                string l(sm[3]);
                string v(sm[2]);

                string reg_v = load(8, v);
                if(b == "GEZ")
                    ett("bgez " + reg_v + " " + l);
                else if(b == "GZ")
                    ett("bgtz " + reg_v + " " + l);
                else if(b == "NEZ")
                    ett("bne " + reg_v + " $0 " + l);
                else if(b == "EZ")
                    ett("beq " + reg_v + " $0 " + l);
                else if(b == "LZ")
                    ett("bltz " + reg_v + " " + l);
                else if(b == "LEZ")
                    ett("blez " + reg_v + " " + l);

            } else if(regex_match(cd, sm, rretval)) { // ret r
                string r(sm[1]);
                string reg_r = load(8, r);
                ett("sw " + reg_r + " " + int2str(OFFSET_RET_VALUE_ * 4) + "($fp)");

            } else if(regex_match(cd, sm, rjump)) {
                ett("j " + string(sm[1]));
            } else if(regex_match(cd, sm, rpush)) { // push v pos
                string v(sm[1]);
                int pos = str2int(string(sm[2]));
                string reg_v = load(8, v);
                ett("sw " + reg_v + " " + int2str((blksz + REGISTER_SIZE_ + DISPLAY_SIZE_ + pos) * 4) + "($fp)");
            } else if(regex_match(cd, sm, rcal)) {
                string fn(sm[1]);

                for(set<string>::iterator iter = blk_list[i]->out.begin(); iter != blk_list[i]->out.end(); iter++)
                    if(inRegister(*iter))ett("sw " + regName(*iter) + " " + int2str((blksz + reg_dis[*iter] - reg_start) * 4) + "($fp)");

                ett("sw $fp " + int2str((blksz + REGISTER_SIZE_ + OFFSET_PREV_FP_) * 4) + "($fp)");
                ett("addi $fp $fp " + int2str((blksz + REGISTER_SIZE_) * 4));
                ett("jal " + fn);

                for(set<string>::iterator iter = blk_list[i]->out.begin(); iter != blk_list[i]->out.end(); iter++)
                    if(inRegister(*iter))ett("lw " + regName(*iter) + " " + int2str((blksz + reg_dis[*iter] - reg_start) * 4) + "($fp)");


            } else if(regex_match(cd, sm, rprintv)) { // printf v
                string flg(sm[1]);
                string v(sm[2]);
                string reg_v = load(8, v);
                ett("move $a0 " + reg_v);
                if(flg == "N") ett("li $v0 1");
                else ett("li $v0 11");
                ett("syscall");
            } else if(regex_match(cd, sm, rprints)) {
                string s(sm[1]);
                string label = etd(s);
                ett("la $a0 " + label);
                ett("li $v0 4");
                ett("syscall");
            } else if(regex_match(cd, sm, rscan)) { // scanf v
                string flg(sm[1]);
                string v(sm[2]);
                if(flg == "N") ett("li $v0 5");
                else ett("li $v0 12");
                ett("syscall");
                if(inRegister(v))
                    ett("move " + regName(v) + " $v0");
                else
                    ett("sw $v0 " + varOff(v));
            }
        }
    }
}


void printTcode_coloring(string path) {
    std::ofstream fout(path);
    fout << ".data" << endl;
    for(int i = 0; i < tcode_data.size(); i++)
        fout << tcode_data[i] << endl;
    fout << endl << ".text" << endl;
    for(int i = 0; i < tcode_text.size(); i++)
        fout << tcode_text[i] << endl;
}
