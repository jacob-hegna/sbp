#include <iostream>
#include <string>
#include <vector>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"


uint64_t predict_branch(uint64_t start) {
    BBlock block(start);

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
    uint64_t start = 0x0;
    uint64_t end   = predict_branch(start);
  	std::cout << "The block located at 0x" << std::hex << start
  			  << " will likely branch to 0x" << std::hex << end << std::endl;

  	return -1;
}
