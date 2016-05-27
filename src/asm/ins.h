#ifndef __INS_H_
#define __INS_H_

#include <stdint.h>

enum class InsType {
    INS, // unhandled case
    JMP,
    CALL,
    RET
};

class Ins {
public:
    Ins();
    Ins(uint64_t loc, uint8_t size, InsType ins_type = InsType::INS);

    void init(uint64_t loc, uint8_t size, InsType ins_type = InsType::INS);

    void set_size(uint8_t size);
    void set_loc(uint64_t loc);
    void set_type(InsType type);

    uint8_t get_size();
    uint64_t get_loc();
    InsType get_ins_type();
protected:
    uint8_t size;
    uint64_t loc;

    InsType ins_type;
};

#endif