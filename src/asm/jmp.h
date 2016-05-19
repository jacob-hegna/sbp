#ifndef __JMP_H_
#define __JMP_H_

#include "ins.h"

enum class JmpType {
    JO,   // Jump if overflow
    JNO,  // Jump if not overflow
    JS,   // Jump if sign
    JNS,  // Jump if not sign
    JE,   // Jump if equal
    JZ,   // Jump if zero
    JNE,  // Jump if not equal
    JNZ,  // Jump if not zero
    JB,   // Jump if below
    JNAE, // Jump if not above or equal
    JC,   // Jump if carry
    JNB,  // Jump if not below
    JAE,  // Jump if above or equal
    JNC,  // Jump if not carry
    JBE,  // Jump if below or equal
    JNA,  // Jump if not above
    JA,   // Jump if above
    JNBE, // Jump if not below or equal
    JL,   // Jump if less
    JNGE, // Jump if not greater or equal
    JGE,  // Jump if greater or equal
    JNL,  // Jump if not less
    JLE,  // Jump if less or equal
    JNG,  // Jump if not greater
    JG,   // Jump if greater
    JNLE, // Jump if not less or equal
    JP,   // Jump if parity
    JPE,  // Jump if parity even
    JNP,  // Jump if not parity
    JPO,  // Jump if parity odd
    JCXZ, // Jump if %CX register is 0
    JECXZ // Jump if $ECX register is 0
};

class Jmp : public Ins {
public:
    Jmp();
    Jmp(JmpType type, uint64_t to,uint64_t from, uint8_t size);
    ~Jmp();

    void init(JmpType type, uint64_t to);
    bool is_loop();

    uint64_t get_to();
    JmpType get_type();

private:
    uint64_t to;
    JmpType type;
};

#endif