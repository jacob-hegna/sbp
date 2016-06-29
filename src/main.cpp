#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <queue>

#include "bblock.h"
#include "parse.h"
#include "graph.h"
#include "cfg_worker.h"

int main(int argc, char *argv[]) {
    std::string usage = "Usage: " + std::string(argv[0]) + " -fc <filename> | -h";
    if(argc < 2) {
        std::cout << usage << std::endl;
        return -1;
    }

    std::string path(argv[1]);

    std::cout << "parsing file" << std::endl;
    BlockFile block_file = parse_file(path);

    std::cout << "constructing graphs" << std::endl;
    std::queue<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);

    std::cout << "determining execution path" << std::endl;
    std::vector<uint64_t> exec_path = get_exec_path(path, block_file.blocks);

    std::cout << "profiling" << std::endl;
    BBlock::create_profile(block_file.blocks, exec_path);

    std::cout << "determining tendency" << std::endl;
    CFGWorker::set_graphs(graphs);
    CFGWorker::find_tendency(exec_path, block_file.blocks);

    const uint worker_amt = 5;
    std::array<CFGWorker, worker_amt> workers;

    std::cout << "checking heuristics for accuracy" << std::endl;
    for(auto& worker : workers) {
        worker.start();
    }
    for(auto& worker : workers) {
        worker.join();
    }

    for(int i = 0; i < 6; ++i) {
        std::cout << i << " " << CFGWorker::get_accuracy(i) * 100
                  << " " << CFGWorker::get_coverage(i) << std::endl;
    }

    return 0;
}
