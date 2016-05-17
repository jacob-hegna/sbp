#ifndef __JMP_H_
#define __JMP_H_

#include "ins.h"

class Jmp : public Ins {
public:
    Jmp(){

    }

    ~Jmp() {

    }

    bool is_loop() {
        return (from - to > 0);
    }

private:
    unsigned int to, from;
};

#endif