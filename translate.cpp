#include "translate.h"

#include <regex>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

int linecount = 0;

#define OFFSET_RET_VALUE_ 0
#define OFFSET_RET_ADDR_ 1
#define OFFSET_PREV_FP_ 2
#define DISPLAY_SIZE_ 3

#define WORD_WIDTH_ 4
#define DATA_BASE_ADDR_ 0x10040000

EMBEDTAB embedcode;

int block;
int blockSize;

std::regex rfunc("(void|int|char) (\\S+)\\(\\)");
std::regex rreturn("RETURN");
std::regex rassign2("(\\S+) = (\\S+)");
std::regex rassign3("(\\S+) = (\\S+) (\\+|-|\\*|/) (\\S+)");
std::regex rbranch("(GEZ|GZ|NEZ|EZ|LZ|LEZ) (\\S+) #(Label_\\d+_)");
std::regex rretval("RETV (\\S+)");
std::regex rjump("JMP #(Label_\\d+_)");
std::regex rpush("PUSH (\\S+) (\\d+)");
std::regex rcal("CAL (\\S+)");
std::regex rprintv("PRINTF(N|C) (\\S+)");
std::regex rprints("PRINTFS <(.*)>");
std::regex rscan("SCANF(N|C) (\\S+)");

std::regex rarr("(\\S+)\\[(\\S+)\\]");


std::vector<std::string> text_segment;
std::vector<std::string> data_segment;


void getGlobalOffset(std::string ident, int &offset, bool &constkind) {
    for(int i = 0; i < global_tab.size(); ++i) {
        if(!strcmp(ident.c_str(), global_tab[i].ident)) {
            offset = global_tab[i].value;
            if(global_tab[i].kind == cons)
                constkind = true;
            else
                constkind = false;
            return;
        }
    }
    std::cout << "getGlobalOffset error" << ident << std::endl;
}

bool getLocalOffset(std::string ident, int &offset, bool &constkind) {
    for(int i = 0; i < local_tab[block].size(); ++i) {
        if(!strcmp(ident.c_str(), local_tab[block][i].ident)) {
            offset = local_tab[block][i].value;
            if(local_tab[block][i].kind == cons)
                constkind = true;
            else
                constkind = false;
            return true;
        }
    }
    return false;
}


void setBlock(std::string ident) {
    for(int i = 0; i < global_tab.size(); ++i) {
        if(!strcmp(ident.c_str(), global_tab[i].ident)) {
            block = global_tab[i].value;
            if(local_tab[block].empty())
                blockSize = DISPLAY_SIZE_;
            else {
                std::vector<TAB_ELEMENT>::iterator iter = local_tab[block].end() - 1;
                if(iter->kind == para || iter->kind == var)
                    if(iter->length)
                        blockSize = iter->length + iter->value;
                    else
                        blockSize = iter->value + 1;
                else
                    blockSize = DISPLAY_SIZE_;
            }
            return;
        }
    }
    std::cout << "setBlock error" << std::endl;
}

bool num(std::string s) {
    int i = 0;
    if(s[0] == '-') {
        ++i;
    }
    for(; i < s.size(); ++i) {
        if(s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}

void write(std::string s) {
    text_segment.push_back(s);
    //std::cout << s << std::endl;
}

std::string writeString(std::string str) {
    static int strLabelNo = 0;
    char s[20];
    sprintf(s, "str%d", strLabelNo++);
    std::string strLabel(s);
    data_segment.push_back(strLabel + ": .asciiz " + "\"" + str + "\"");
    return strLabel;
}

void showTcode () {
    std::cout << ".data" << std::endl;
    for (int i = 0; i < data_segment.size(); i++)
        std::cout << data_segment[i] << std::endl;
    std::cout << std::endl << ".text" << std::endl;
    for (int i = 0; i < text_segment.size(); i++)
        std::cout << text_segment[i] << std::endl;

    std::ofstream fout("tcode.asm");
    fout << ".data" << std::endl;
    for (int i = 0; i < data_segment.size(); i++)
        fout << data_segment[i] << std::endl;
    fout << std::endl << ".text" << std::endl;
    for (int i = 0; i < text_segment.size(); i++)
        fout << text_segment[i] << std::endl;
}

void loadVal(std::string reg, std::string ident) {
    std::smatch sm;
    if(std::regex_match(ident, sm, rarr)) {
        std::string arr(sm[1]);
        std::string index(sm[2]);
        loadVal(reg, index);
        int offset;
        bool constkind;
        if(getLocalOffset(arr, offset, constkind)) {
            write("la $t0 " + int2str(offset * WORD_WIDTH_) + "($fp)");
        } else {
            getGlobalOffset(arr, offset, constkind);
            write("la $t0 " + int2str(offset * WORD_WIDTH_) + "($gp)");
        }
        write("mul " + reg + " " + reg + " 4");
        write("add " + reg + " " + reg + " $t0");
        write("lw " + reg + " (" + reg + ")");
    }


    else if(num(ident)) {
        write("li " + reg + " " + ident);
    }

    else if (ident == "RET") {
        write("lw " + reg + " " + int2str((OFFSET_RET_VALUE_ + blockSize) * WORD_WIDTH_) + "($fp)");
    }

    else {
        int offset = 0;
        bool constkind;
        if(getLocalOffset(ident, offset, constkind))
            if(constkind)
                write("li " + reg + " " + int2str(offset));
            else
                write("lw " + reg + " " + int2str(offset * WORD_WIDTH_) + "($fp)");
        else {
            getGlobalOffset(ident, offset, constkind);
            if(constkind)
                write("li " + reg + " " + int2str(offset));
            else
                write("lw " + reg + " " + int2str(offset * WORD_WIDTH_) + "($gp)");
        }
    }
}

void storeVal(std::string reg, std::string ident) {
    int offset = 0;
    std::smatch sm;
    bool constkind;
    if(std::regex_match(ident, sm, rarr)) {
        std::string arr(sm[1]);
        std::string index(sm[2]);
        loadVal("$t0", index);
        int offset;
        if(getLocalOffset(arr, offset, constkind))
            write("la $t1 " + int2str(offset * WORD_WIDTH_) + "($fp)");
        else {
            getGlobalOffset(arr, offset, constkind);
            write("la $t1 " + int2str(offset * WORD_WIDTH_) + "($gp)");
        }
        write("mul $t0 $t0 4");
        write("add $t0 $t0 $t1");
        write("sw " + reg + " ($t0)");
    } else if(getLocalOffset(ident, offset, constkind))
        write("sw " + reg + " " + int2str(offset * WORD_WIDTH_) + "($fp)");
    else {
        getGlobalOffset(ident, offset, constkind);
        write("sw " + reg + " " + int2str(offset * WORD_WIDTH_) + "($gp)");
    }
}



void translate() {
    embedcode = embeddingLabel();

    // set $fp to global var top
    bool flag = false;
    for(std::vector<TAB_ELEMENT>::reverse_iterator riter = global_tab.rbegin(); riter != global_tab.rend(); ++riter) {
        if(riter->kind == var) {
            int off = riter->length ? riter->value + riter->length : riter->value + 1;
            write("li $fp " + int2str(off * WORD_WIDTH_ + DATA_BASE_ADDR_));
            flag = true;
            break;
        }
    }
    if(!flag) write("li $fp " + int2str(DATA_BASE_ADDR_));
    // set $gp to DATA_BASE_ADDR_
    write("li $gp " + int2str(DATA_BASE_ADDR_));
    // jal main
    write("jal main");
    // dead loop
    write("end: beq $0 $0 end");

    for(EMBEDTAB::iterator iter = embedcode.begin(); iter != embedcode.end(); ++iter) {

        //write("");
        //write(iter->second);
        std::smatch sm;
        if(!iter->first.empty()) {
            std::string s = iter->first.substr(1, iter->first.size() - 1);
            write(s + ":");
        }


        if(std::regex_match(iter->second, sm, rfunc)) {


            setBlock(std::string(sm[2]));
            write(std::string(sm[2]) + ":");
            write("sw $ra " + int2str(OFFSET_RET_ADDR_ * WORD_WIDTH_) + "($fp)");


        }

        else if(std::regex_match(iter->second, sm, rreturn)) {


            write("lw $ra " + int2str(OFFSET_RET_ADDR_ * WORD_WIDTH_) + "($fp)");
            write("lw $fp " + int2str(OFFSET_PREV_FP_ * WORD_WIDTH_) + "($fp)");
            write("jr $ra");


        }

        else if(std::regex_match(iter->second, sm, rassign2)) {


            std::string left_var(sm[1]);
            std::string right_var(sm[2]);

            loadVal("$s0", right_var);

            storeVal("$s0", left_var);


        }

        else if(std::regex_match(iter->second, sm, rassign3)) {
            std::string left_var(sm[1]);
            std::string right_var_1(sm[2]);
            std::string right_var_2(sm[4]);
            std::string right_op(sm[3]);

            loadVal("$s1", right_var_1);
            loadVal("$s2", right_var_2);

            if(right_op == "+")
                write("add $s0 $s1 $s2");
            else if(right_op == "-")
                write("sub $s0 $s1 $s2");
            else if(right_op == "*")
                write("mul $s0 $s1 $s2");
            else if(right_op == "/")
                write("div $s0 $s1 $s2");

            storeVal("$s0", left_var);
        }

        else if(std::regex_match(iter->second, sm, rbranch)) {
            std::string b(sm[1]);
            std::string v(sm[2]);
            std::string l(sm[3]);

            loadVal("$s0", v);
            if(b == "GEZ")
                write("bgez $s0 " + l);
            else if(b == "GZ")
                write("bgtz $s0 " + l);
            else if(b == "NEZ")
                write("bne $s0 $0 " + l);
            else if(b == "EZ")
                write("beq $s0 $0 " + l);
            else if(b == "LZ")
                write("bltz $s0 " + l);
            else if(b == "LEZ")
                write("blez $s0 " + l);

        }

        else if(std::regex_match(iter->second, sm, rretval)) {


            std::string ret_var(sm[1]);
            if(num(ret_var)) {
                write("li $t0 " + ret_var);
            } else {
                int offset = 0;
                bool constkind;
                if(getLocalOffset(ret_var, offset, constkind)) {
                    if(constkind)
                        write("li $t0 " + int2str(offset));
                    else
                        write("lw $t0 " + int2str(offset * WORD_WIDTH_) + "($fp)");
                } else {
                    getGlobalOffset(ret_var, offset, constkind);
                    if(constkind)
                        write("li $t0 " + int2str(offset));
                    else
                        write("lw $t0" + int2str(offset * WORD_WIDTH_) + "($gp)");
                }
            }
            write("sw $t0 " + int2str(OFFSET_RET_VALUE_ * WORD_WIDTH_) + "($fp)");


        }

        else if(std::regex_match(iter->second, sm, rjump)) {
            write("j " + std::string(sm[1]));
        }

        else if(std::regex_match(iter->second, sm, rpush)) {
            std::string para_var(sm[1]);
            std::string para_off(sm[2]);
            loadVal("$s0", sm[1]);
            int para_off_v = 0;
            for (int i = 0; i < para_off.size(); ++i)
                para_off_v = para_off_v * 10 + para_off[i] - '0';
            write("sw $s0 " + int2str((blockSize + DISPLAY_SIZE_ + para_off_v) * WORD_WIDTH_) + "($fp)");
        }

        else if(std::regex_match(iter->second, sm, rcal)) {

            std::string cal_func(sm[1]);
            write("sw $fp " + int2str((blockSize + OFFSET_PREV_FP_) * WORD_WIDTH_) + "($fp)");
            write("addi $fp $fp " + int2str(blockSize * WORD_WIDTH_));
            write("jal " + cal_func);


        }

        else if(std::regex_match(iter->second, sm, rprintv)) {
            std::string v(sm[2]);
            std::string t(sm[1]);
            loadVal("$a0", v);
            if(t == "C")
                write("li $v0 11");

            else
                write("li $v0 1");


            write("syscall");

        }

        else if(std::regex_match(iter->second, sm, rprints)) {
            std::string str(sm[1]);
            std::string label = writeString(str);
            write("la $a0 " + label);
            write("li $v0 4");
            write("syscall");
        }

        else if(std::regex_match(iter->second, sm, rscan)) {
            std::string v(sm[2]);
            std::string t(sm[1]);
            if(t == "C")
                write("li $v0 12");
            else
                write("li $v0 5");
            write("syscall");
            storeVal("$v0", v);

        }

        else std::cout << iter->second << std::endl;
    }
    showTcode();
}
