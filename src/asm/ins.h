#ifndef __INS_H_
#define __INS_H_

#include <stdint.h>

class Ins {
public:
    Ins() {

    }

    ~Ins() {

    }

    uint8_t get_size() {
        return size;
    }
private:
    uint8_t size;
};

#endif