#include "bblock.h"

#include <functional>
#include <random>
#include <list>
#include <algorithm>

void BBlock::parse() {
    int col_start = 44;
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

    for(auto heuristic : heuristics) {
        if((addr = (this->*heuristic)()) != FAIL_H) break;
    }

    return addr;
}

uint64_t BBlock::loop_h() {
    Jmp *exit = (Jmp*)(ins.back());
    if(exit->is_loop()) {
        return exit->get_to();
    }

    return FAIL_H;
}


uint64_t BBlock::opcode_h() {
    Jmp *exit = (Jmp*)(ins.back());
    std::array<JmpType, 6> zero_or_greater = {
        JmpType::JZ,
        JmpType::JNB,
        JmpType::JA,
        JmpType::JGE,
        JmpType::JNL,
        JmpType::JG 
    };

    if(std::find(zero_or_greater.begin(), zero_or_greater.end(),
            exit->get_type()) != zero_or_greater.end()) {
        return exit->get_to();
    }

    return FAIL_H;
}

uint64_t BBlock::rand_h() {
    Jmp *exit = (Jmp*)(ins.back());
    
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, 1);

    if(dist(rng)) {
        return exit->get_to();
    } else {
        return exit->get_loc();
    }
}