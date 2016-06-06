#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"
#include "parse.h"
#include "graph.h"

/*
 * main is full of test code rn and will not match the main in the final version
 * of the project
 */
int main(int argc, char *argv[]) {
    std::string path;
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    path = std::string(argv[1]);

    vector_shared<BBlock> super_set = parse_file(path);

    return 0;
}
