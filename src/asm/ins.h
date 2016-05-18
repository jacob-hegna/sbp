#ifndef __INS_H_
#define __INS_H_

#include <stdint.h>

class Ins {
public:
    Ins() {

    }
    Ins(uint64_t loc) {
        init(loc);
    }
    ~Ins() {

    }

    void init(uint64_t loc) {
        this->loc = loc;
    }

    uint8_t get_size() {
        return size;
    }
    uint64_t get_loc() {

    }
protected:
    uint8_t size;
    uint64_t loc;
};

#endif