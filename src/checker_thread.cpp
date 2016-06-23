#include "checker_thread.h"

std::vector<uint64_t> CheckerThread::exec_path;
vector_shared<BBlock> CheckerThread::super_set;

HeuristicAccuracy CheckerThread::check_predictions(uint64_t (BBlock::*indiv_heuristic)()) {
    HeuristicAccuracy accuracy;
    accuracy.accuracy = 0;
    accuracy.coverage = 0;

    uint correct = 0;
    std::shared_ptr<BBlock> prev_block = nullptr;

    for(uint64_t addr : CheckerThread::exec_path) {
        std::shared_ptr<BBlock> block = search_bblocks(CheckerThread::super_set,
                                                       addr, true);
        if(block == nullptr) continue;

        // if prev_block hasn't been assigned, assign it and move to the next
        // bblock
        if(prev_block == nullptr) {
            prev_block = block;
            continue;
        }

        // if the block does not not have a static branch, we won't attempt to
        // predict the branch
        if(!block->static_jmp()) {
            prev_block = block;
            continue;
        }

        // if our prediction failed (was FAIL_H) we skip this block
        // when a prediction fails, it is often due to a dynamic branch
        // or having incomplete information about the surrounding blocks
        uint64_t predict = prev_block->predict(indiv_heuristic);
        if(predict == 0xFFFFFFFFFFFFFFFF) {
            prev_block = block;
            continue;
        }

        if(prev_block->get_fall() != block->get_loc()
            && prev_block->get_jmp() != block->get_loc()) {
            prev_block = block;
            continue;
        }


        if(predict == block->get_loc()) {
            ++correct;
        }

        ++accuracy.coverage;
        prev_block = block;
    }

    accuracy.accuracy = (float)correct/(float)accuracy.coverage;
    return accuracy;
}
