#include "bblock.h"

#include <functional>
#include <random>
#include <algorithm>

#include "graph.h"

/*
 * 0xFFFFFFFFFFFFFFFF is the hex code I'll use to show the heuristic failed,
 * (or didn't match/apply). It's not perfect, but it's unlikely to have a
 * collision with a real value in the near future
 */

#define FAIL_H 0xFFFFFFFFFFFFFFFF

HeuristicProfile BBlock::profile;

void BBlock::create_profile(vector_shared<BBlock> &super_set, 
                            std::vector<uint64_t> &exec_path) {
    if(exec_path.size() < 100) return; // skip profiling small test files

    uint call_s_correct     = 0;
    uint call_s_coverage    = 0;
    float call_s_accuracy   = 0;
    float test_ratio        = 0.02;

    std::shared_ptr<BBlock> prev_block = nullptr;

    BBlock::profile.call_s_flip = true;

    for(uint i = 0; i < exec_path.size() * test_ratio; ++i) {
        uint64_t addr = exec_path.at(i);
        std::shared_ptr<BBlock> block = search_bblocks(super_set, addr, true);
        if(block == nullptr) continue;
        if(prev_block == nullptr) {
            prev_block = block;
            continue;
        }
        if(!block->static_jmp()) {
            prev_block = block;
            continue;
        }

        uint64_t predict = prev_block->call_s_h();
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
            ++call_s_correct;
        }

        ++call_s_coverage;
        prev_block = block;
    }
    call_s_accuracy = (float)call_s_correct/(float)call_s_coverage;
    if(call_s_accuracy < .5) BBlock::profile.call_s_flip ^= true;
}

uint64_t BBlock::combined_h() {
    uint64_t addr = FAIL_H;

    auto heuristics = {
        &BBlock::loop_h,
        &BBlock::opcode_h,
        &BBlock::call_s_h,
        &BBlock::return_s_h,
        &BBlock::rand_h
    };

    // if don't know at compile-time where the block branches to, we can't
    // make a prediction
    if(get_jmp() == FAIL_H) {
        return FAIL_H;
    }

    if(ins.size() >= 2) {
        if(ins.end()[-2]->get_ins_type() == InsType::CALL) {
            return FAIL_H;
        }
    }

    for(auto heuristic : heuristics) {
        if((addr = (this->*heuristic)()) != FAIL_H) break;
    }

    return addr;
}

/* 
 * detects if the basic block results in a loop, if so, predicts it will always
 * loop back
 */
uint64_t BBlock::loop_h() {
    if(graph == nullptr) {
        return FAIL_H;
    }

    if(jmp.use_count() == 0) {
        return FAIL_H;
    }

    std::shared_ptr<BBlock> jmp_shared = jmp.lock();

    if(jmp_shared->get_loc() > this->get_loc()) {
        return FAIL_H;
    }

    if(graph->dominator_check(shared_from_this(), jmp_shared)) {
        return get_fall();
    }

    return FAIL_H;
}

/*
 * determines the type of opcode which exits the basic block and makes a
 * prediction based off of that branch condition
 */
uint64_t BBlock::opcode_h() {
    std::shared_ptr<Jmp> exit = this->get_last();

    // this function (actually a pointer to a lambda) detects whether the given
    // conditional branch (jmp) is in the given vector of branches (jmps)
    std::function<bool(std::vector<JmpType>, JmpType)> jmp_matches =
    [](std::vector<JmpType> jmps, JmpType jmp) {
        return (std::find(jmps.begin(), jmps.end(), jmp) != jmps.end());
    };

    // NOTE: might want to reduce some of the greater/less than opcode filtering
    // because MIPS had greater/less than zero, but x86 only has greater/less
    // than a given register

    // many functions return 0 or greater to indicate success, so we predict
    // these branches will be taken
    std::vector<JmpType> jmp_zero_or_greater = {
        JmpType::JLE

        //JmpType::JA,
        //JmpType::JGE,
        //JmpType::JG,
    };
    if(jmp_matches(jmp_zero_or_greater, exit->get_jmp_type())) {
        return this->get_jmp();
    }

    // if the function returns negative or less than some target, we predict the
    // branch will fall-through as we should expect many of those to be error
    // codes
    std::vector<JmpType> jmp_negative = {
        JmpType::JNZ,
        JmpType::JZ,
        JmpType::JNL,
        JmpType::JB,
        JmpType::JNB

        //JmpType::JLE,
        //JmpType::JNAE,
        //JmpType::JL,
        //JmpType::JNGE
    };
    if(jmp_matches(jmp_negative, exit->get_jmp_type())) {
        return this->get_fall();
    }

    return FAIL_H;
}

/*
 * checks each successor branch for a call instruction, chooses the one with
 * the call. if each has a call, return FAIL_H
 */
uint64_t BBlock::call_s_h() {
    std::shared_ptr<BBlock> fall_shared = fall.lock();
    std::shared_ptr<BBlock> jmp_shared  = jmp.lock();

    if(fall_shared == nullptr || jmp_shared == nullptr) return FAIL_H;

    bool next_fall_call = false;
    for(std::shared_ptr<Ins> i : fall_shared->get_ins()) {
        if(i->get_ins_type() == InsType::CALL) {
            next_fall_call = true;
            break;
        }
    }

    bool next_jmp_call = false;
    for(std::shared_ptr<Ins> i : jmp_shared->get_ins()) {
        if(i->get_ins_type() == InsType::CALL) {
            next_jmp_call = true;
            break;
        }
    }

    if(next_fall_call && next_jmp_call) { // guards against the dual case
        return FAIL_H;
    } else if(next_fall_call) {
        if(!BBlock::profile.call_s_flip) {
            return fall_shared->get_loc();
        } else {
            return jmp_shared->get_loc();
        }
    } else if(next_jmp_call) {
        if(!BBlock::profile.call_s_flip) {
            return jmp_shared->get_loc();
        } else {
            return fall_shared->get_loc();
        }
    }
    return FAIL_H;
}

/*
 * checks each successor branch for a return instruction, chooses the one
 * WITHOUT the return. if each has a return, return FAIL_H
 */
uint64_t BBlock::return_s_h() {
    std::shared_ptr<BBlock> fall_shared = fall.lock();
    std::shared_ptr<BBlock> jmp_shared  = jmp.lock();

    if(fall_shared == nullptr || jmp_shared == nullptr) return FAIL_H;

    bool next_fall_ret = false;
    for(std::shared_ptr<Ins> i : fall_shared->get_ins()) {
        if(i->get_ins_type() == InsType::RET) {
            next_fall_ret = true;
            break;
        }
    }

    bool next_jmp_ret = false;
    for(std::shared_ptr<Ins> i : jmp_shared->get_ins()) {
        if(i->get_ins_type() == InsType::RET) {
            next_jmp_ret = true;
            break;
        }
    }

    if(next_fall_ret && next_jmp_ret) { // guards against the dual case
        return FAIL_H;
    } else if(next_fall_ret) {          // note the returns are the converse
        return jmp_shared->get_loc();          // of what would normally be expected
    } else if(next_jmp_ret) {
        return fall_shared->get_loc();
    }
    return FAIL_H;

}

/*
 * if no previous heuristics create a useful prediction, default to random
 */
std::random_device BBlock::rd;
std::mt19937 BBlock::rng(rd());
std::uniform_int_distribution<int> BBlock::dist(0, 1);
uint64_t BBlock::rand_h() {    
    return (dist(rng) ? get_jmp() : get_fall());
}
