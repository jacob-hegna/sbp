#ifndef __PREDICTION_CHECK_H_
#define __PREDICTION_CHECK_H_

#include <thread>
#include <vector>
#include <string>

#include "bblock.h"
#include "smart_vector.h"

struct HeuristicAccuracy {
    float accuracy;
    uint coverage;
};


class CheckerThread {
public:
    CheckerThread() : thread() {

    }
    CheckerThread(std::string heuristic_str, uint64_t (BBlock::*heuristic_fn)())
                  : CheckerThread() {
        this->path = path;
        this->heuristic_str = heuristic_str;
        this->heuristic_fn = heuristic_fn;
    }
    ~CheckerThread() {
        if(thread.joinable()) thread.join();
    }

    void start() {
        thread = std::thread(&CheckerThread::thread_main, this);
    }

    static void set_exec_path(std::vector<uint64_t> exec_path) {
        CheckerThread::exec_path = exec_path;
    }
    static void set_bblocks(vector_shared<BBlock> super_set) {
        CheckerThread::super_set = super_set;
    }

private:
    std::thread thread;
    std::string path;
    std::string heuristic_str;
    uint64_t (BBlock::*heuristic_fn)();

    static std::vector<uint64_t> exec_path;
    static vector_shared<BBlock> super_set;

    HeuristicAccuracy check_predictions(uint64_t (BBlock::*indiv_heuristic)() = {});

    void thread_main() {
        HeuristicAccuracy acc = check_predictions(heuristic_fn);
        std::stringstream ss;
        ss << heuristic_str << ": " << std::endl
           << "\taccuracy: " << acc.accuracy * 100
           << "\tcoverage: " << acc.coverage
           << std::endl;
        std::cout << ss.str();
    }
};

#endif