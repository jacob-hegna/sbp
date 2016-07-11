#include "cfg_worker.h"

#include "../semaphore/semaphore.h"

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

void CFGWorker::spawn_workers() {
    std::array<CFGWorker, worker_amt> workers;

    for(auto& worker : workers) worker.start();
    for(auto& worker : workers) worker.join();
}

void CFGWorker::set_graphs(std::queue<Graph> graphs) {
    CFGWorker::graphs = graphs;
}



struct BBlockPair {
    bool poison_pill;
    std::shared_ptr<BBlock> first;
    std::shared_ptr<BBlock> second;
};

std::queue<BBlockPair> block_queue;
std::mutex mutex;
Semaphore semaphore;

void tendency_producer(std::vector<uint64_t> exec_path,
                       vector_shared<BBlock> super_set) {
    BBlockPair pair;
    pair.poison_pill = false;

    for(uint64_t tag : exec_path) {
        pair.first = search_bblocks(super_set, tag, true);

        if(pair.first == nullptr) {
            continue;
        }
        if(pair.second == nullptr) {
            pair.second = pair.first;
            continue;
        }

        mutex.lock();
        block_queue.push(pair);
        mutex.unlock();

        semaphore.signal();

        pair.second = pair.first;
    }

    pair.poison_pill = true;

    mutex.lock();
    block_queue.push(pair);
    mutex.unlock();

    semaphore.signal();
}

void tendency_consumer() {
    int pills = 0;
    while(true) {
        semaphore.wait();

        mutex.lock();
        BBlockPair pair = block_queue.front();
        block_queue.pop();
        mutex.unlock();

        if(pair.poison_pill) {
            pills++;
            if(pills == 2) {
                break;
            } else {
                continue;
            }
        }

        if(!pair.second->static_jmp()) {
            continue;
        }

        if(pair.second->get_jmp() == pair.first->get_loc()) {
            pair.second->jmp_count++;
        }

        if(pair.second->get_fall() == pair.first->get_loc()) {
            pair.second->fall_count++;
        }
    }
}


void CFGWorker::find_tendency(std::vector<uint64_t> exec_path,
                              vector_shared<BBlock> super_set) {

    size_t divison = exec_path.size() / 4;

    std::vector<uint64_t> first = std::vector<uint64_t>(
        exec_path.begin(),
        exec_path.begin() + divison
    );
    std::vector<uint64_t> second = std::vector<uint64_t>(
        exec_path.begin() + divison,
        exec_path.begin() + divison + divison
    );
    std::vector<uint64_t> third = std::vector<uint64_t>(
        exec_path.begin() + divison + divison,
        exec_path.begin() + divison + divison + divison
    );
    std::vector<uint64_t> fourth = std::vector<uint64_t>(
        exec_path.begin() + divison + divison + divison,
        exec_path.end()
    );

    std::thread producer_1(&tendency_producer, first, super_set);
    std::thread producer_2(&tendency_producer, second, super_set);
    std::thread producer_3(&tendency_producer, third, super_set);
    std::thread producer_4(&tendency_producer, fourth, super_set);

    std::thread consumer_1(&tendency_consumer);
    std::thread consumer_2(&tendency_consumer);

    producer_1.join();
    producer_2.join();
    producer_3.join();
    producer_4.join();

    consumer_1.join();
    consumer_2.join();
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

    static std::array<uint64_t (BBlock::*)(), 6> heuristics = {{
        &BBlock::combined_h,
        &BBlock::opcode_h,
        &BBlock::opcode_h,
        &BBlock::call_s_h,
        &BBlock::return_s_h,
        &BBlock::rand_h
    }};

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
        std::cout << graphs.size() << std::endl;
        check_accuracy(graph->get_root());
    }
    std::cout << "finished" << std::endl;
}