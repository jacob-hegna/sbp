#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <sstream>
#include <memory>
#include <fstream>

#include "asm/ins.h"
#include "asm/jmp.h"

#include "bblock.h"

std::shared_ptr<Jmp> parse_jmp(std::string jmp_str);

std::shared_ptr<Ins> parse_ins(std::string ins_str);

std::vector<BBlock> parse_file(std::string path);

#endif