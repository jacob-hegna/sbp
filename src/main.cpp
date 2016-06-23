#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include "bblock.h"
#include "parse.h"
#include "graph.h"
#include "checker_thread.h"

int main(int argc, char *argv[]) {
    std::string path;
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    path = std::string(argv[1]);

    BlockFile block_file = parse_file(path);

    std::vector<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);

    std::vector<uint64_t> exec_path = get_exec_path(path, block_file.blocks);

    CheckerThread::set_exec_path(exec_path);
    CheckerThread::set_bblocks(block_file.blocks);

    CheckerThread loop_h("loop_h", &BBlock::loop_h);
    loop_h.start();

    CheckerThread opcode_h("opcode_h", &BBlock::opcode_h);
    opcode_h.start();

    CheckerThread call_s_h("call_s_h", &BBlock::call_s_h);
    call_s_h.start();

    CheckerThread return_s_h("return_s_h", &BBlock::return_s_h);
    return_s_h.start();

    CheckerThread rand_h("rand_h", &BBlock::rand_h);
    rand_h.start();

    CheckerThread combined_h("combined_h", &BBlock::combined_h);
    combined_h.start(false);

    return 0;
}
