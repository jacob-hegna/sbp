#include "smart_vector.h"

#include "bblock.h"

std::shared_ptr<BBlock> search_bblocks(vector_shared<BBlock> blocks,
    uint64_t addr, bool tag) {

    std::shared_ptr<BBlock> block = nullptr;
    for(std::shared_ptr<BBlock> b : blocks) {
        uint64_t temp_addr; // will store either the iterator b's tag or addr
        if(tag) {
            temp_addr = b->get_tag();
        } else {
            temp_addr = b->get_loc();
        }
        if(addr == temp_addr) {
            block = b;
            break;
        }
    }

    return block;
}