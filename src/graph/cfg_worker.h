#ifndef __CFG_WORKER_H_
#define __CFG_WORKER_H_

#include <thread>
#include <memory>
#include <mutex>

#include "graph.h"
#include "../smart_vector.h"
#include "../semaphore/semaphore.h"

class CFGWorker {
public:
    CFGWorker() : thread() {

    }
    ~CFGWorker() {
        if(thread.joinable()) thread.join();
    }

    void start();
    void join();

    static void spawn_workers();

    static void set_graphs(std::queue<Graph> graphs);
    static void find_tendency(std::vector<uint64_t> exec_path,
                              BlockSet super_set);

    static void set_accuracy(int heuristic, int correct, int total);
    static float get_accuracy(int heuristic);
    static int get_coverage(int heuristic);

private:
    std::thread thread;

    static vector_shared<BBlock> accuracy_finished;
    static std::mutex            accuracy_mutex;

    static const uint worker_amt = 3;

    static std::queue<Graph>  graphs;
    static std::mutex         graphs_mutex;

    static std::mutex         heuristic_mutex;
    static std::array<int, 6> heuristic_accuracy;
    static std::array<int, 6> heuristic_coverage;

    static std::unique_ptr<Graph> get_graph();

    void check_accuracy(std::shared_ptr<BBlock> leaf);
    void thread_main();
};


class TendencyWorker {
public:
    TendencyWorker();
    ~TendencyWorker();

    void spawn(std::vector<uint64_t> exec_path, BlockSet super_set);
    void join();
private:
    std::thread producer_thread;
    std::thread consumer_thread;

    struct BBlockPair {
        bool poison_pill;
        std::shared_ptr<BBlock> first;
        std::shared_ptr<BBlock> second;
    };

    std::queue<BBlockPair> block_queue;
    std::mutex mutex;
    Semaphore semaphore;

    void producer(std::vector<uint64_t> exec_path, BlockSet super_set);
    void consumer();
};
#endif