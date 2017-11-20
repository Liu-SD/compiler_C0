#include <iostream>
#include "main.h"

using namespace std;

int main() {
    set_file_stream("15061111_test.c0");
    while(1) {
        nextSym();
        cout << SYMBOL_STRING[int(sym)] << "\t" << token << endl;
    }
    return 0;
}
