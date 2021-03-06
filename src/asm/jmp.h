#ifndef __JMP_H_
#define __JMP_H_

#include "ins.h"

#include <array>
#include <algorithm>
#include <string>

enum struct JmpType {
    JMP,   // Unconditional jmp
    JO,    // Jump if overflow
    JNO,   // Jump if not overflow
    JS,    // Jump if sign
    JNS,   // Jump if not sign
    JE,    // Jump if equal
    JZ,    // Jump if zero
    JNE,   // Jump if not equal
    JNZ,   // Jump if not zero
    JB,    // Jump if below
    JNAE,  // Jump if not above or equal
    JC,    // Jump if carry
    JNB,   // Jump if not below
    JAE,   // Jump if above or equal
    JNC,   // Jump if not carry
    JBE,   // Jump if below or equal
    JNA,   // Jump if not above
    JA,    // Jump if above
    JNBE,  // Jump if not below or equal
    JL,    // Jump if less
    JNGE,  // Jump if not greater or equal
    JGE,   // Jump if greater or equal
    JNL,   // Jump if not less
    JLE,   // Jump if less or equal
    JNG,   // Jump if not greater
    JG,    // Jump if greater
    JNLE,  // Jump if not less or equal
    JP,    // Jump if parity
    JPE,   // Jump if parity even
    JNP,   // Jump if not parity
    JPO,   // Jump if parity odd
    JCXZ,  // Jump if %CX register is 0
    JECXZ  // Jump if $ECX register is 0
};



class Jmp : public Ins {
public:
    Jmp();
    Jmp(JmpType jmp_type, uint64_t to, uint64_t from, uint8_t size);

    void init(JmpType jmp_type, uint64_t to);

    static JmpType str_to_jmp(std::string jmp_str);
    static std::string jmp_to_str(JmpType jmp_type);
    static std::array<std::string, 33> get_jmp_strings();

    void set_static(bool static_jmp);
    void set_to(uint64_t to);

    uint64_t get_to();
    JmpType get_jmp_type();
    bool get_static();

private:
    uint64_t to;
    JmpType jmp_type;

    bool static_jmp;

    static std::array<std::string, 33> jmp_strings;
};

#endif
