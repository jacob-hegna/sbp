#include "ins.h"

Ins::Ins() {

}
Ins::Ins(uint64_t loc, uint8_t size, InsType ins_type) {
    init(loc, size, ins_type);
}

void Ins::init(uint64_t loc, uint8_t size, InsType ins_type) {
    this->loc      = loc;
    this->size     = size;
    this->ins_type = ins_type;
}
void Ins::set_size(uint8_t size) {
    this->size = size;
}
void Ins::set_loc(uint64_t loc) {
    this->loc = loc;
}
void Ins::set_type(InsType ins_type) {
    this->ins_type = ins_type;
}

uint8_t Ins::get_size() {
    return size;
}
uint64_t Ins::get_loc() {
    return loc;
}
InsType Ins::get_ins_type() {
    return ins_type;
}