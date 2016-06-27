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

    BlockFile block_file = parse_file(path);

    std::queue<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);
    std::vector<uint64_t> exec_path = get_exec_path(path, block_file.blocks);
    BBlock::create_profile(block_file.blocks, exec_path);

    CFGWorker::set_graphs(graphs);

    const uint worker_amt = 5;
    std::array<CFGWorker, worker_amt> workers;

    for(auto& worker : workers) {
        worker.start();
    }

    return 0;
}
