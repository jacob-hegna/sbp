#include "bblock.h"


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

uint64_t BBlock::jmp() {
    Jmp *j = (Jmp*)(ins.back());
    return j->get_to();
}

void BBlock::push_back(Ins *i) {
    if(ins.size() > 0) {
        i->set_loc(ins.back()->get_loc() + ins.back()->get_size());
    }
    ins.push_back(i);
}

uint64_t BBlock::get_loc() {
    return start;
}

std::vector<Ins*> BBlock::get_ins() {
    return ins;
}