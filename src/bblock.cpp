#include "bblock.h"


BBlock::BBlock() {
    fall   = nullptr;
    jmp    = nullptr;
    parent = nullptr;
}
BBlock::BBlock(uint64_t addr) : BBlock() {
    init(addr);
}
BBlock::~BBlock() {

}

void BBlock::init(uint64_t addr) {
    start = addr;
}

uint64_t BBlock::next() {
    return combined_h();
}

uint64_t BBlock::get_fall() {
    Jmp *j = (Jmp*)(ins.back().get());
    return j->get_loc() + ins.back()->get_size();
}

uint64_t BBlock::get_jmp() {
    Jmp *j = (Jmp*)(ins.back().get());
    return j->get_to();
}

void BBlock::push_back(std::shared_ptr<Ins> i) {
    if(ins.size() > 0) {
        i->set_loc(ins.back()->get_loc() + ins.back()->get_size());
    }
    ins.push_back(i);
}

uint64_t BBlock::get_loc() {
    return start;
}

std::vector<std::shared_ptr<Ins>> BBlock::get_ins() {
    return ins;
}