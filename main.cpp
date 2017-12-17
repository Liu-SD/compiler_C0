#include "grammaticalAnalysis.h"
#include "translate.h"
#include "dag_optimize.h"
#include "coloring.h"


int main(int argc, char *argv[]) {
    /*
    if(argc < 2) {
        std::cout << "input file path" << std::endl;
        return -1;
    }
    set_file_stream(argv[1]);
    */

    //set_file_stream("tmp.txt");
    //set_file_stream("input(1).txt");
    set_file_stream("15061111_test.txt");
    //set_file_stream("15061129_test.txt");

    nextSym();
    NFA_program();
    // show_tables();
    showerrormessages();
    if (!errorlist.empty())
        return 0;
    std::cout << "no error(s) in program\ngenerate mcode in ./mocde.c0\ngenerate tcode in ./tcode.asm" << std::endl;
    std::cout << "generate dag optimized mcode in ./dag_optimized_mcode.c0\ngenerate dag optimized tcode.asm in ./ dag_optimized_tacode.asm" << std::endl;
    printMcode();
    translate(embeddingLabel());
    printTcode("tcode.asm");

    dag_optimize();
    update_symbol_table(reserved_var);
    //show_tables();
    printMcode_optimized();
    translate(optimize_codes);
    printTcode("tcode_dag_optimized.asm");

    // mcode is in optimize_codes and symbol table updated now
    // do register optimize

    // coloring_translate(optimize_codes);
}
