#include "jmp.h"

std::array<std::string, 33> Jmp::jmp_strings = {
    "JMP",
    "JO",
    "JNO",
    "JS",
    "JNS",
    "JE",
    "JZ",
    "JNE",
    "JNZ",
    "JB",
    "JNAE",
    "JC",
    "JNB",
    "JAE",
    "JNC",
    "JBE",
    "JNA",
    "JA",
    "JNBE",
    "JL",
    "JNGE",
    "JGE",
    "JNL",
    "JLE",
    "JNG",
    "JG",
    "JNLE",
    "JP",
    "JPE",
    "JNP",
    "JPO",
    "JCXZ",
    "JECXZ"
};

/*
 * converts a string to a JmpType
 */
JmpType Jmp::str_to_jmp(std::string jmp_str) {
    JmpType ret = JmpType::JMP;

    std::transform(jmp_str.begin(), jmp_str.end(), jmp_str.begin(), toupper);

    for(int i = 0; i < 34; ++i) {
        ret = static_cast<JmpType>(i);
        if(jmp_str == jmp_strings[i]) break;
    }

    return ret;
}

std::string Jmp::jmp_to_str(JmpType jmp_type) {
    std::string ret;
    for(int i = 0; i < 34; ++i) {
        ret = jmp_strings[i];
        if(jmp_type == static_cast<JmpType>(i)) break;
    }
    return ret;
}

Jmp::Jmp() {
    this->jmp_type   = JmpType::JMP;
    this->ins_type   = InsType::JMP;
    this->to         = 0x0;
    this->static_jmp = true;
}

Jmp::Jmp(JmpType jmp_type, uint64_t to, uint64_t from, uint8_t size)
        : Ins(from, size) {
    init(jmp_type, to);
}

void Jmp::init(JmpType jmp_type, uint64_t to) {
    this->jmp_type   = jmp_type;
    this->ins_type   = ins_type;
    this->to         = to;
    this->static_jmp = true;
}

void Jmp::set_static(bool static_jmp) {
    this->static_jmp = static_jmp;
    if(static_jmp == false) this->to = 0xFFFFFFFFFFFFFFFF;
}

void Jmp::set_to(uint64_t to) {
    this->to = to;
}

uint64_t Jmp::get_to() {
    return (static_jmp) ? this->to : 0xFFFFFFFFFFFFFFFF;
}
JmpType Jmp::get_jmp_type() {
    return jmp_type;
}
bool Jmp::get_static() {
    return static_jmp;
}
std::array<std::string, 33> Jmp::get_jmp_strings() {
    return jmp_strings;
}