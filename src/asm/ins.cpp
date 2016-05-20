#include "ins.h"

Ins::Ins() {

}
Ins::Ins(uint64_t loc, uint8_t size, InsType type) {
    init(loc, size, type);
}
Ins::~Ins() {

}

void Ins::init(uint64_t loc, uint8_t size, InsType type) {
    this->loc  = loc;
    this->size = size;
    this->type = type;
}
void Ins::set_size(uint8_t size) {
    this->size = size;
}
void Ins::set_loc(uint64_t loc) {
    this->loc = loc;
}
void Ins::set_type(InsType type) {
    this->type = type;
}

uint8_t Ins::get_size() {
    return size;
}
uint64_t Ins::get_loc() {
    return loc;
}
InsType Ins::get_type() {
    return type;
}