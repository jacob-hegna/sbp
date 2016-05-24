#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <memory>
// #include <static_branch_prediction>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"

uint64_t predict_branch(uint64_t start) {
    BBlock block(start);

    std::shared_ptr<Ins> ins_a(new Ins(start, 2));
    std::shared_ptr<Ins> ins_b(new Ins(start + ins_a->get_size(), 2));
    std::shared_ptr<Jmp> jmp (new Jmp(JmpType::JO, 0xFF,
                        start + ins_a->get_size() + ins_b->get_size(), 4));

    // manually creating block instead of parsing
    block.push_back(ins_a);
    block.push_back(ins_b);
    block.push_back(jmp);

    return block.next();
}

uint64_t s_to_uint64(std::string s) {
    std::stringstream sstr(s);
    uint64_t ret;
    sstr >> std::hex >> ret;
    return ret;
}

std::map<uint64_t, BBlock> parse_file(std::string path) {
    std::map<uint64_t, BBlock> super_set;


    std::ifstream file(path);
    std::vector<std::string> lines;

    for(std::string line; getline(file, line);) {
        lines.push_back(line);
    }

    bool open_tag = false;
    for(std::string line : lines) {
        if(line.substr(0, 8) == "dispatch") {
            uint64_t block_tag = s_to_uint64(line.substr(22));
            open_tag = true;
        } else if(open_tag) {
            if(line.length() < 50) {
                if(line.length() == 0) open_tag = false;
            } else {
                std::string ins_str = line.substr(43, 7);
            }
        }

    }

    file.close();

    return super_set;
}

int main(int argc, char *argv[]) {
    /*
    std::string path;

    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    
    path = std::string(argv[1]);

    parse_file(path);*/

    
    uint64_t start = 0x01; // initial block address
    uint64_t end   = predict_branch(start);
    std::cout << "0x" << std::hex << start << " --> 0x"
              << std::hex << end << std::endl;
    

    return 0;
}
