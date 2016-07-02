#include "cfg_worker.h"

std::queue<Graph>  CFGWorker::graphs;
std::mutex         CFGWorker::graphs_mutex;

std::mutex         CFGWorker::heuristic_mutex;
std::array<int, 6> CFGWorker::heuristic_accuracy;
std::array<int, 6> CFGWorker::heuristic_coverage;

void CFGWorker::start() {
    thread = std::thread(&CFGWorker::thread_main, this);
}
void CFGWorker::join() {
    if(thread.joinable()) thread.join();
}

void CFGWorker::set_graphs(std::queue<Graph> graphs) {
    CFGWorker::graphs = graphs;
}

void CFGWorker::find_tendency(std::vector<uint64_t> exec_path,
                              vector_shared<BBlock> super_set) {
    std::shared_ptr<BBlock> last_block = nullptr;

    for(uint64_t tag : exec_path) {
        std::shared_ptr<BBlock> block = search_bblocks(super_set, tag, true);

        if(block == nullptr) {
            continue;
        }

        if(last_block == nullptr) {
            last_block = block;
            continue;
        }

        if(!last_block->static_jmp()) {
            last_block = block;
            continue;
        }

        if(last_block->get_jmp() == block->get_loc()) {
            last_block->jmp_count++;
        }

        if(last_block->get_fall() == block->get_loc()) {
            last_block->fall_count++;
        }

        last_block = block;
    }
}

void CFGWorker::set_accuracy(int heuristic, int correct, int total) {
    std::lock_guard<std::mutex> guard(heuristic_mutex);
    if(heuristic >= 0 && heuristic < 6) {
        heuristic_accuracy[heuristic] += correct;
        heuristic_coverage[heuristic] += total;
    }
}

float CFGWorker::get_accuracy(int heuristic) {
    std::lock_guard<std::mutex> guard(heuristic_mutex);
    if(heuristic >= 0 && heuristic < 6) {
        return (float)heuristic_accuracy[heuristic]
             / (float)heuristic_coverage[heuristic];
    }
    return -1;
}

int CFGWorker::get_coverage(int heuristic) {
    std::lock_guard<std::mutex> guard(heuristic_mutex);
    if(heuristic >= 0 && heuristic < 6) {
        return heuristic_coverage[heuristic];
    }
    return -1;
}

std::unique_ptr<Graph> CFGWorker::get_graph() {
    std::lock_guard<std::mutex> guard(graphs_mutex);

    std::unique_ptr<Graph> ret = nullptr;
    if(graphs.size() > 0) {
        // should replace the below code with std::make_unique once ittc
        // computers are upgraded to fedora 21
        ret = std::unique_ptr<Graph>(new Graph);
        *ret = graphs.front();
        graphs.pop();
    }
    return ret;
}

void CFGWorker::check_accuracy(std::shared_ptr<BBlock> leaf,
                               vector_shared<BBlock> finished) {
    if(leaf == nullptr) return;
    if(std::find(finished.begin(), finished.end(), leaf) != finished.end()) {
        return;
    }

    std::array<uint64_t (BBlock::*)(), 6> heuristics = {
        &BBlock::combined_h,
        &BBlock::loop_h,
        &BBlock::opcode_h,
        &BBlock::call_s_h,
        &BBlock::return_s_h,
        &BBlock::rand_h
    };

    for(uint i = 0; i < heuristics.size(); ++i) {
        leaf->prediction[i] = (leaf.get()->*heuristics[i])();
        if(leaf->prediction[i] == leaf->get_jmp()) {
            CFGWorker::set_accuracy(i, leaf->jmp_count,
                                    leaf->jmp_count + leaf->fall_count);
        } else if(leaf->prediction[i] == leaf->get_fall()) {
            CFGWorker::set_accuracy(i, leaf->fall_count,
                                    leaf->jmp_count + leaf->fall_count);
        }
    }

    finished.push_back(leaf);

    if(leaf->jmp.use_count() > 0) {
        std::shared_ptr<BBlock> jmp_shared(leaf->jmp);
        check_accuracy(jmp_shared, finished);
    }

    if(leaf->fall.use_count() > 0) {
        std::shared_ptr<BBlock> fall_shared(leaf->fall);
        check_accuracy(fall_shared, finished);
    }
}

void CFGWorker::thread_main() {
    std::unique_ptr<Graph> graph;
    while((graph = get_graph()) != nullptr) {
        check_accuracy(graph->get_root());
    }
}