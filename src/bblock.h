#ifndef __BBLOCK_H_
#define __BBLOCK_H_

#include <stdint.h>
#include <vector>

#include "asm/ins.h"

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

    void push_back(Ins i) {
        ins.push_back(i);
    }
private:
	uint64_t start;
    std::vector<Ins> ins;
};



#endif