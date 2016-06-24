#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "bblock.h"
#include "parse.h"
#include "graph.h"
#include "checker_thread.h"

void indiv_test(std::string path) {
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
}

void combined_test(std::string path) {
    BlockFile block_file = parse_file(path);
    std::vector<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);
    std::vector<uint64_t> exec_path = get_exec_path(path, block_file.blocks);

    CheckerThread::set_exec_path(exec_path);
    CheckerThread::set_bblocks(block_file.blocks);

    CheckerThread combined_h("combined_h", &BBlock::combined_h);
    combined_h.start(false);
}

int main(int argc, char *argv[]) {
    std::string usage = "Usage: " + std::string(argv[0]) + " -fc <filename> | -h";
    if(argc < 2) {
        std::cout << usage << std::endl;
        return -1;
    }

    opterr = 0;
    char c;
    while((c = getopt(argc, argv, "i:c:h")) != -1) {
        switch(c) {
            case 'i':
                indiv_test(optarg);
                break;
            case 'c':
                combined_test(optarg);
                break;
            case 'h':
            default:
                std::cout << usage << std::endl
                          << "  -i tests each heuristic individually" << std::endl
                          << "  -c tests the combined heuristics" << std::endl
                          << "  -h displays this message" << std::endl;
                break;
        }
    }


    return 0;
}
