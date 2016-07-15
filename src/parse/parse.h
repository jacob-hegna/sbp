#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include <unordered_map>

#include "../asm/ins.h"
#include "../asm/jmp.h"

#include "../bblock/bblock.h"
#include "../smart_vector.h"

struct BlockFile {
    BlockSet blocks;
    std::unordered_map<uint64_t, uint64_t> addr_to_tag_map;
    std::vector<uint64_t> calls;
};

std::vector<uint64_t> get_exec_path(std::string path,
    BlockSet super_set);

BlockFile parse_file(std::string path);

#endif