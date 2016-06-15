#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <sstream>
#include <memory>
#include <fstream>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"
#include "vector_shared.h"

struct BlockFile {
    vector_shared<BBlock> blocks;
    std::vector<uint64_t> calls;
};

float check_predictions(std::string path, vector_shared<BBlock> blocks, uint &total);

BlockFile parse_file(std::string path);

#endif