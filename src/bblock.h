#ifndef __BBLOCK_H_
#define __BBLOCK_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

#include "asm/ins.h"
#include "asm/jmp.h"

class Graph;

class BBlock {
    friend class Graph;
public:
    BBlock();
    BBlock(uint64_t addr);
    ~BBlock();

    void init(uint64_t addr);

    // returns the address the block is predicted to branch to
    uint64_t next();

    // returns the address of the next fall-through block
    uint64_t get_fall();

    // returns the address of the next condition jmp-to block
    uint64_t get_jmp();

    void push_back(std::shared_ptr<Ins>);

    uint64_t get_loc();
    std::vector<std::shared_ptr<Ins>> get_ins();
private:
	uint64_t start;
    std::vector<std::shared_ptr<Ins>> ins;

    // for the graph
    BBlock *fall, *jmp;
    BBlock *parent;

    // the below heuristics are defined in heuristics.cpp

    // the combined heuristics
    uint64_t combined_h();

    // heuristics on the given block
    uint64_t opcode_h();
    uint64_t loop_h();

    // heuristic on successor block(s)
    uint64_t call_s_h();
    uint64_t return_s_h();

    // default random heuristic
    uint64_t rand_h();
};

#endif