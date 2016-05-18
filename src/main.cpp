#include <iostream>
#include <string>
#include <vector>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"

uint64_t predict_branch(uint64_t start) {
    BBlock block(start);

    Ins ins(start);
    Jmp jmp(JmpType::J0, 0xFFFAAAAFFFFAAAA, start);

    block.push_back(&ins);
    block.push_back(&jmp);

    return block.next();
}

int main(int argc, char *argv[]) {
    std::string path;

    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    
    path = std::string(argv[1]);
    uint64_t start = 0x01;
    uint64_t end   = predict_branch(start);
  	std::cout << "0x" << std::hex << start << " --> 0x"
              << std::hex << end << std::endl;

  	return 0;
}
