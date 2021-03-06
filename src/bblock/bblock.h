#ifndef __BBLOCK_H_
#define __BBLOCK_H_

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <sys/types.h>

#include "../asm/ins.h"
#include "../asm/jmp.h"

#include "../smart_vector.h"

struct HeuristicAccuracy {
    float accuracy;
    uint coverage;
};

struct HeuristicProfile {
    bool call_s_flip;
};

class Graph;
class BBlock;

typedef std::unordered_map<uint64_t, std::shared_ptr<BBlock>> BlockSet;

/*
 * useful utility function which uses vector_shared
 * if tag is false, we search based on the given address
 * if tag is true, we assume the "address" is actually a block tag
 */
std::shared_ptr<BBlock> search_bblocks(vector_shared<BBlock> &blocks,
    uint64_t addr, bool tag = false);

class BBlock : public std::enable_shared_from_this<BBlock> {
    friend class Graph;
    friend class CFGWorker;
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

    vector_weak<BBlock> get_parents();

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

    static void create_profile(BlockSet &super_set,
        std::vector<uint64_t> &exec_path);

    static void set_addr_to_tag_map(std::unordered_map<uint64_t, uint64_t> map);
    static uint64_t addr_to_tag(uint64_t addr);
    static std::shared_ptr<BBlock> find(BlockSet &super_set,
        uint64_t search, bool tag = true);

    uint fall_count;
    uint jmp_count;
    std::array<uint64_t, 6> prediction;

private:
	uint64_t block_addr;
    uint64_t block_tag;

    static HeuristicProfile profile;

    static std::unordered_map<uint64_t, uint64_t> addr_to_tag_map;

    vector_shared<Ins> ins;

    // normally would use a smart pointer, but this makes more sense in this
    // context - the graph won't be declared on the heap, so we'll need to make
    // sure that it's in scope when we use this object
    Graph *graph;

    // for the graph
    std::weak_ptr<BBlock> fall, jmp;
    vector_weak<BBlock> parents;

    // random number generation for rand_h
    static std::random_device rd;
    static std::mt19937 rng;
    static std::uniform_int_distribution<int> dist;
};

#endif
