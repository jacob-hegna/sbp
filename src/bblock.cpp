#include "bblock.h"

#include <functional>
#include <random>
#include <algorithm>


BBlock::BBlock() {

}
BBlock::BBlock(uint64_t addr) {
    init(addr);
}
BBlock::~BBlock() {
     
}

void BBlock::init(uint64_t addr) {
    start = addr;
}

void BBlock::parse() {
    int col_start = 44;
}

uint64_t BBlock::next() {
    return combined_h();
}

uint64_t BBlock::fall() {
    Jmp *j = (Jmp*)(ins.back());
    return j->get_loc() + ins.back()->get_size();
}

void BBlock::push_back(Ins *i) {
    ins.push_back(i);
}

std::vector<Ins*> BBlock::get_ins() {
    return ins;
}


/* heuristics */

/*
 * 0xFFFFFFFFFFFFFFFF is the hex code I'll use to show the heuristic failed,
 * (or didn't match/apply). It's not perfect, but it's unlikely to have a
 * collision with a real value in the near future
 */

#define FAIL_H 0xFFFFFFFFFFFFFFFF

uint64_t BBlock::combined_h() {
    uint64_t addr = 0x0;

    // order of combining heuristics
    auto heuristics = {
        &BBlock::loop_h,
        &BBlock::opcode_h,
        &BBlock::rand_h
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
    Jmp *exit = (Jmp*)(ins.back());
    if(exit->is_loop()) {
        return exit->get_to();
    }

    return FAIL_H;
}

/*
 * determines the type of opcode which exits the basic block and makes a
 * prediction based off of that branch condition
 */
uint64_t BBlock::opcode_h() {
    Jmp *exit = (Jmp*)(ins.back());

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
    if(jmp_matches(jmp_zero_or_greater, exit->get_type())) {
        return exit->get_to();
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
    if(jmp_matches(jmp_negative, exit->get_type())) {
        return fall();
    }

    return FAIL_H;
}

uint64_t BBlock::call_s_h() {

}

/*
 * if no previous heuristics create a useful prediction, default to random
 */
uint64_t BBlock::rand_h() {
    Jmp *exit = (Jmp*)(ins.back());
    
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    return (dist(rng) ? exit->get_to() : fall());
}