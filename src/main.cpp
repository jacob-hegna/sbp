#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <functional>

#include "bblock/bblock.h"
#include "parse/parse.h"
#include "graph/graph.h"
#include "graph/cfg_worker.h"

#include "timer_debug.h"

int main(int argc, char *argv[]) {
    std::string usage = "Usage: " + std::string(argv[0]) + " -fc <filename> | -h";
    if(argc < 2) {
        std::cout << usage << std::endl;
        return -1;
    }

    std::string path(argv[1]);

    BlockFile block_file = time_debug(parse_file,
        "parsing file").call(path);

    std::queue<Graph> graphs = time_debug(make_graphs,
        "constructing graphs").call(block_file.blocks, block_file.calls);

    std::vector<uint64_t> exec_path = time_debug(get_exec_path,
        "determining execution path").call(path, block_file.blocks);

    time_debug(BBlock::create_profile,
        "profiling").call(block_file.blocks, exec_path);

    CFGWorker::set_graphs(graphs);
    time_debug(CFGWorker::find_tendency,
        "determining tendency").call(exec_path, block_file.blocks);

    time_debug(CFGWorker::spawn_workers,
        "checking heuristics for accuracy").call();

    for(int i = 0; i < 6; ++i) {
        std::cout << i << " " << CFGWorker::get_accuracy(i) * 100
                  << " " << CFGWorker::get_coverage(i) << std::endl;
    }

    return 0;
}
