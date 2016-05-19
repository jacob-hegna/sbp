#include "jmp.h"

Jmp::Jmp() {

}

Jmp::Jmp(JmpType type, uint64_t to, uint64_t from, uint8_t size) : Ins(from, size) {
    init(type, to);
}

Jmp::~Jmp() {

}

void Jmp::init(JmpType type, uint64_t to) {
    this->type = type;
    this->to   = to;
}

bool Jmp::is_loop() {
    return loc > to;
}

uint64_t Jmp::get_to() {
    return to;
}
JmpType Jmp::get_type() {
    return type;
}
