#include "ins.h"

Ins::Ins() {

}
Ins::Ins(uint64_t loc, uint8_t size) {
    init(loc, size);
}
Ins::~Ins() {

}

void Ins::init(uint64_t loc, uint8_t size) {
    this->loc  = loc;
    this->size = size;
}

uint8_t Ins::get_size() {
    return size;
}
uint64_t Ins::get_loc() {
    return loc;
}