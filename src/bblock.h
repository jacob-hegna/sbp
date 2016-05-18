#ifndef __BBLOCK_H_
#define __BBLOCK_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "asm/ins.h"
#include "asm/jmp.h"

class BBlock {
public:
    BBlock() {

    }
    BBlock(uint64_t addr) {
    	init(addr);
    }
    ~BBlock() {
         
    }

    void init(uint64_t addr) {
        start = addr;
    }

    void parse();

    // returns the address the block is predicted to branch to
    uint64_t next() {
    	return start;
    }

    // returns the address of the next fall-through block
    uint64_t fall() {
        Jmp *j = (Jmp*)(&ins.at(0));
        return j->get_from() + ins.at(0).get_size();
    }

    void push_back(Ins i) {
        ins.push_back(i);
    }

    std::vector<Ins> get_ins() {
        return ins;
    }
private:
	uint64_t start;
    std::vector<Ins> ins;
};



#endif