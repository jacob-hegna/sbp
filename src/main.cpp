#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include "bblock.h"
#include "parse.h"
#include "graph.h"

void prediction_thread(std::string path, vector_shared<BBlock> super_set,
            std::string heuristic_str, uint64_t (BBlock::*heuristic_fn)()) {

    HeuristicAccuracy acc = check_predictions(path, super_set, heuristic_fn);
    std::stringstream ss;
    ss << heuristic_str << ": " << std::endl
       << "\taccuracy: " << acc.accuracy * 100
       << "\tcoverage: " << acc.coverage
       << std::endl;
    std::cout << ss.str();
}


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

    std::array<std::string, 5> heuristic_str = {
        "BBlock::loop_h",
        "BBlock::opcode_h",
        "BBlock::call_s_h",
        "BBlock::return_s_h",
        "BBlock::rand_h"
    };

    std::array<uint64_t(BBlock::*)() , 5> heuristic_fn = {
        &BBlock::loop_h,
        &BBlock::opcode_h,
        &BBlock::call_s_h,
        &BBlock::return_s_h,
        &BBlock::rand_h
    };

    std::vector<std::thread> heuristic_threads;
/*
    for(int i = 0; i < 2; ++i) {
        prediction_thread(
            path,
            block_file.blocks,
            heuristic_str[i],
            heuristic_fn[i]
        );
    }   
*/

    for(int i = 0; i < 2; ++i) {
        heuristic_threads.push_back(std::thread(
            prediction_thread,
            path,
            block_file.blocks,
            heuristic_str[i],
            heuristic_fn[i]
        ));
    }
    
    for(int i = 0; i < 2; ++i) {
        heuristic_threads[i].join();
    }

    return 0;
}
