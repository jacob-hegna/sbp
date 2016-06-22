#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <sstream>
#include <memory>
#include <fstream>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"
#include "smart_vector.h"

struct BlockFile {
    vector_shared<BBlock> blocks;
    std::vector<uint64_t> calls;
};

struct HeuristicAccuracy {
    float accuracy;
    uint coverage;
};

HeuristicAccuracy check_predictions(std::string path, vector_shared<BBlock> blocks,
    uint64_t (BBlock::*indiv_heuristic)() = {});

BlockFile parse_file(std::string path);

#endif