#include "bblock.h"

#include <functional>
#include <random>
#include <algorithm>

/*
 * 0xFFFFFFFFFFFFFFFF is the hex code I'll use to show the heuristic failed,
 * (or didn't match/apply). It's not perfect, but it's unlikely to have a
 * collision with a real value in the near future
 */

#define FAIL_H 0xFFFFFFFFFFFFFFFF

uint64_t BBlock::combined_h() {
    uint64_t addr = 0x0;

    // if don't know at compile-time where the block branches to, we can't
    // make a prediction
    if(get_jmp() == FAIL_H) {
        return FAIL_H;
    }

    // this auto becomes std::function<uint64_t(void)> at compile-time, but
    // gcc won't allow uint64_t in the template parameters for whatever reason,
    // so for portability, we let the compiler determine the exact type at 
    // compile-time
    auto heuristics = {
        &BBlock::loop_h,
        &BBlock::opcode_h,
        //&BBlock::call_s_h,
        //&BBlock::return_s_h,
        //&BBlock::rand_h
    };

    // iterate through each heuristic, if any of them don't return the
    // FAIL_H error code, break out and use that address
    // Otherwise, it will default to the rand_h heuristic
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
    Jmp *exit = (Jmp*)(get_ins().back().get());
    if(exit->is_loop()) {
        return this->get_jmp();
    }

    return FAIL_H;
}

/*
 * determines the type of opcode which exits the basic block and makes a
 * prediction based off of that branch condition
 */
uint64_t BBlock::opcode_h() {
    Jmp *exit = (Jmp*)(get_ins().back().get());

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
        JmpType::JZ,
        JmpType::JNB,
        JmpType::JA,
        JmpType::JGE,
        JmpType::JNL,
        JmpType::JG 
    };
    if(jmp_matches(jmp_zero_or_greater, exit->get_jmp_type())) {
        return this->get_jmp();
    }

    // if the function returns negative or less than some target, we predict the
    // branch will fall-through as we should expect many of those to be error
    // codes
    std::vector<JmpType> jmp_negative = {
        JmpType::JNZ,
        JmpType::JB,
        JmpType::JNAE,
        JmpType::JL,
        JmpType::JNGE
    };
    if(jmp_matches(jmp_negative, exit->get_jmp_type())) {
        return this->get_fall();
    }

    return FAIL_H;
}

/*
 * checks each successor branch for a call instruction, chooses the one WITHOUT
 * the call. if each has a call, return FAIL_H
 */
uint64_t BBlock::call_s_h() {
    BBlock next_fall(0x0, this->get_fall());
    BBlock next_jmp(0x0, this->get_jmp());

    // <test code>
    std::shared_ptr<Ins> ins_fall(new Ins(this->get_fall(), 2, InsType::INS));
    std::shared_ptr<Ins> ins_jmp(new Ins(this->get_jmp(), 2, InsType::INS));

    next_fall.push_back(ins_fall);
    next_jmp.push_back(ins_jmp);
    // </test code>

    bool next_fall_call = false;
    for(std::shared_ptr<Ins> i : next_fall.get_ins()) {
        if(i->get_ins_type() == InsType::CALL) {
            next_fall_call = true;
            break;
        }
    }

    bool next_jmp_call = false;
    for(std::shared_ptr<Ins> i : next_jmp.get_ins()) {
        if(i->get_ins_type() == InsType::CALL) {
            next_jmp_call = true;
            break;
        }
    }

    if(next_fall_call && next_jmp_call) { // guards against the dual case
        return FAIL_H;
    } else if(next_fall_call) {           // note the returns are the converse
        return next_jmp.get_loc();        // of what would normally be expected
    } else if(next_jmp_call) {
        return next_fall.get_loc();
    }
    return FAIL_H;
}

/*
 * checks each successor branch for a return instruction, chooses the one
 * WITHOUT the return. if each has a return, return FAIL_H
 */
uint64_t BBlock::return_s_h() {
    BBlock next_fall(0x0, this->get_fall());
    BBlock next_jmp(0x0, this->get_jmp());

    // <test code>
    std::shared_ptr<Ins> ins_fall(new Ins(this->get_fall(), 2, InsType::INS));
    std::shared_ptr<Ins> ins_jmp(new Ins(this->get_jmp(), 2, InsType::INS));

    next_fall.push_back(ins_fall);
    next_jmp.push_back(ins_jmp);
    // </test code>

    bool next_fall_ret = false;
    for(std::shared_ptr<Ins> i : next_fall.get_ins()) {
        if(i->get_ins_type() == InsType::RET) {
            next_fall_ret = true;
            break;
        }
    }

    bool next_jmp_ret = false;
    for(std::shared_ptr<Ins> i : next_jmp.get_ins()) {
        if(i->get_ins_type() == InsType::RET) {
            next_jmp_ret = true;
            break;
        }
    }

    if(next_fall_ret && next_jmp_ret) { // guards against the dual case
        return FAIL_H;
    } else if(next_fall_ret) {          // note the returns are the converse
        return next_jmp.get_loc();      // of what would normally be expected
    } else if(next_jmp_ret) {
        return next_fall.get_loc();
    }
    return FAIL_H;

}

/*
 * if no previous heuristics create a useful prediction, default to random
 */
uint64_t BBlock::rand_h() {    
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    return (dist(rng) ? get_jmp() : get_fall());
}