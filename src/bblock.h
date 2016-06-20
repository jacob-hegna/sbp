#ifndef __BBLOCK_H_
#define __BBLOCK_H_

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "smart_vector.h"

class Graph;

class BBlock {
    friend class Graph;
public:
    BBlock();
    BBlock(uint64_t block_tag, uint64_t block_addr);

    void init(uint64_t block_tag, uint64_t block_addr);

    // returns human-readable information about the basic block
    std::string print_info();
    std::string print_ins();

    // returns the address the block is predicted to branch to
    uint64_t predict();

    // returns the last instruction in the block, presumably a Jmp
    std::shared_ptr<Jmp> get_last();

    // returns true if the block has a static branch
    bool static_jmp();

    // returns the address of the next fall-through block
    uint64_t get_fall();

    // returns the address of the next condition jmp-to block
    uint64_t get_jmp();

    void push_back(std::shared_ptr<Ins>);
    void set_ins(vector_shared<Ins>);

    uint64_t get_loc();
    uint64_t get_tag();
    vector_shared<Ins> get_ins();

    vector_shared<BBlock> get_parents();
private:
	uint64_t block_addr;
    uint64_t block_tag;
    uint64_t prediction;

    vector_shared<Ins> ins;

    // for the graph
    std::shared_ptr<BBlock> fall, jmp;
    vector_shared<BBlock> parents;

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