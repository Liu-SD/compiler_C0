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

    std::cout << "file path:" << std::endl;
    char fpath[100];
    std::cin >> fpath;
    set_file_stream(fpath);


    //set_file_stream("tmp.txt");
    //set_file_stream("input(1).txt");
    //set_file_stream("15061111_test.txt");
    //set_file_stream("15061129_test.txt");
    //set_file_stream("coloring_test.txt");
    //set_file_stream("new_test.txt");


    // 编译获得中间代码
    NFA_program();
    // 打印符号表
    // show_tables();
    // 打印错误信息
    showerrormessages();
    // 编译出现错误则停止之后的行为
    if (!errorlist.empty())
        return 0;
    std::cout << "no error in program\ngenerate mcode in ./mocde.c0\ngenerate tcode in ./tcode.asm" << std::endl;
    std::cout << "generate dag optimized mcode in ./dag_optimized_mcode.c0\ngenerate dag optimized tcode.asm in ./ dag_optimized_tacode.asm" << std::endl;
    // 打印中间代码到code.c0
    printMcode("mcode.c0");
    // 将中间代码翻译成目标代码
    translate(embeddingLabel());
    // 打印目标代码
    printTcode("tcode.asm");
    // DAG图优化，得到优化后中间代码
    dag_optimize();
    // 更新符号表——DAG优化后会删去部分中间变量，增加部分全局变量
    update_symbol_table(reserved_var);
    // 打印新的符号表
    // show_tables();
    // 打印优化后中间代码到mcode_dag_optimized.c0
    printMcode_optimized("mcode_dag_optimized.c0");
    // 按简单翻译策略从DAG优化中间代码翻译成目标代码
    translate(optimize_codes);
    // 打印DAG优化后目标代码
    printTcode("tcode_dag_optimized.asm");
    // 按着色算法优化的翻译策略从DAG优化中间代码翻译成目标代码
    coloring_translate(optimize_codes);
    // 打印DAG和着色算法共同优化的目标代码！
    printTcode_coloring("tcode_coloring_optimized.asm");
}
