#ifndef __CFG_WORKER_H_
#define __CFG_WORKER_H_

#include <thread>
#include <memory>
#include <mutex>

#include "graph.h"

class CFGWorker {
public:
    CFGWorker() : thread() {

    }
    ~CFGWorker() {
        if(thread.joinable()) thread.join();
    }

    void start() {
        thread = std::thread(&CFGWorker::thread_main, this);
    }

    static void set_graphs(std::queue<Graph> graphs) {
        CFGWorker::graphs = graphs;
    }

private:
    std::thread thread;

    static std::queue<Graph> graphs;
    static std::mutex        graphs_mutex;

    static std::unique_ptr<Graph> get_graph() {
        std::lock_guard<std::mutex> guard(graphs_mutex);

        std::unique_ptr<Graph> ret = nullptr;
        if(graphs.size() > 0) {
            ret = std::make_unique<Graph>(graphs.front());
            graphs.pop();
        }
        return ret;
    }

    void thread_main() {
        std::unique_ptr<Graph> graph;
        while((graph = get_graph()) != nullptr) {
            std::cout << "a" << std::endl;
        }
    }
};
std::queue<Graph> CFGWorker::graphs;
std::mutex        CFGWorker::graphs_mutex;

#endif