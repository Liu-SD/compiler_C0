#include "grammaticalAnalysis.h"
#include "translate.h"


void show_lexical() {
    int i = 1;
    while(sym != eof) {
        nextSym();
        std::string s;
        switch (sym) {
        case ident:
        case ifsy:
        case elsesy:
        case whilesy:
        case switchsy:
        case casesy:
        case defaultsy:
        case scanfsy:
        case printfsy:
        case voidsy:
        case mainsy:
        case intsy:
        case charsy:
        case returnsy:
        case constsy:
            s = token;
            break;
        case intcon:
        case charcon:
            break;
        case stringcon:
            s = stringbuff;
            break;
        case pluscon:
            s = "+";
            break;
        case minuscon:
            s = "-";
            break;
        case timescon:
            s = "*";
            break;
        case divcon:
            s = "/";
            break;
        case becomes:
            s = "=";
            break;
        case eql:
            s = "==";
            break;
        case neq:
            s = "!=";
            break;
        case gtr:
            s = ">";
            break;
        case grq:
            s = ">=";
            break;
        case lss:
            s = "<";
            break;
        case leq:
            s = "<=";
            break;
        case lsmall:
            s = "(";
            break;
        case rsmall:
            s = ")";
            break;
        case lmedium:
            s = "[";
            break;
        case rmedium:
            s = "]";
            break;
        case lbig:
            s = "{";
            break;
        case rbig:
            s = "}";
            break;
        case comma:
            s = ",";
            break;
        case semicolon:
            s = ";";
            break;
        case colon:
            s = ":";
            break;
        case eof:
            s = "end of file flag";
            break;
        default:
            s = "error!!!";
            break;
        }

        if(sym == intcon) std::cout << i++ << " " << SYMBOL_STRING[sym] << "\t" << num << std::endl;
        else if(sym == charcon) std::cout << i++ << " " << SYMBOL_STRING[sym] << "\t" << char(num) << std::endl;
        else std::cout << i++ << " " << SYMBOL_STRING[sym] << "\t" << s << std::endl;
    }
}

int main(int argc,char *argv[]) {
    /*
    if(argc < 2) {
        std::cout << "input file path" << std::endl;
        return -1;
    }
    set_file_stream(argv[1]);
    */

    //set_file_stream("tmp.txt");
    set_file_stream("input(1).txt");
    nextSym();
    NFA_program();
    //show_tables();
    showerrormessages();
    if(!errorlist.empty())
        return 0;
    printMcode();


    translate();
    printTcode();


    //show_lexical();
}
