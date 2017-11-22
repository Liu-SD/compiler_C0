#include <iostream>
#include "grammaticalAnalysis.h"

using namespace std;

int main() {
    set_file_stream("15061111_test.c0");
    nextSym();
    constDeclare(0);
    constDeclare(0);
    for(int i = 0; i < global_tab.size(); i++) {
        cout <<global_tab[i].ident << "  " << global_tab[i].value << endl;
    }
    return 0;
}
