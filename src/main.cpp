#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"
#include "parse.h"
#include "graph.h"

uint64_t predict_branch(uint64_t start) {
    BBlock block(0x0, start);

    Ins ins_a(start, 2);
    Ins ins_b(start + ins_a.get_size(), 2);
    Jmp jmp (JmpType::JO, 0xFF, start + ins_a.get_size() + ins_b.get_size(), 4);

    // manually creating block instead of parsing
    block.push_back(std::make_shared<Ins>(ins_a));
    block.push_back(std::make_shared<Ins>(ins_b));
    block.push_back(std::make_shared<Jmp>(jmp));

    return block.next();
}

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
    //Graph graph(super_set);

    for(std::shared_ptr<BBlock> block : super_set) {
        std::cout << block->print_info() << block->print_ins() << std::endl;
    }

    return 0;
}
