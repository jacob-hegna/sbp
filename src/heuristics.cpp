#include "heuristics.h"

#include "asm/jmp.h"

/*
 * 0xFFFFFFFFFFFFFFFF is the hex code I'll use to show the heuristic failed,
 * (or didn't match/apply). It's not perfect, but it's unlikely to have a
 * collision with a real value in the near future
 */

uint64_t combined_h(BBlock block) {
    uint64_t addr = 0x0;
    if((addr = loop_h(block)) != 0xFFFFFFFFFFFFFFFF) {
        return addr;
    } else if((addr = opcode_h(block)) != 0xFFFFFFFFFFFFFFFF) {
        return addr;
    }

    return 0xFFFFFFFFFFFFFFFF;
}

uint64_t loop_h(BBlock block) {
    Jmp *exit = (Jmp*)(&block.get_ins().at(0));
    if(exit->is_loop()) {
        return exit->get_to();
    }
    return 0xFFFFFFFFFFFFFFFF;
}

uint64_t opcode_h(BBlock block) {
    Jmp *exit = (Jmp*)(&block.get_ins().at(0));
    if(exit->get_type() == JmpType::JZ) {
        return exit->get_to();
    }

    return 0xFFFFFFFFFFFFFFFF;
}