#include "parse.h"

/*
 * converts a string to a uint64_t
 * local file scoped
 */
uint64_t s_to_uint64(std::string s) {
    std::stringstream sstr(s);
    uint64_t ret;
    sstr >> std::hex >> ret;
    return ret;
}

std::vector<uint64_t> get_exec_path(std::string path,
                                    vector_shared<BBlock> super_set) {
    std::vector<uint64_t> exec_path;
    std::ifstream file(path);

    std::string line;
    while(getline(file, line)) {
        // skip empty lines
        if(line.length() == 0) continue;

        // skip block number lines
        if(line.substr(0, 5) == "Block") continue;

        // catch all the artificially created blocks
        if(line.substr(2, 2) == "0x") {
            uint64_t addr = s_to_uint64(line.substr(2, 18));

            std::shared_ptr<BBlock> block;
            if((block = search_bblocks(super_set, addr)) != nullptr &&
                block->get_tag() != exec_path.back()) {
                exec_path.push_back(addr);
            }
        } else if(line.at(0) == ' ') {
            continue; // skip any other line that starts with whitespace
        } else {
            exec_path.push_back(s_to_uint64(line));
        }
    }
    return exec_path;
}

std::shared_ptr<Jmp> parse_jmp(std::string jmp_str, std::string args) {
    auto jmp_ret = std::make_shared<Jmp>();

    // we can only predict the branch if it jmps to a register
    if(args.at(0) == '$') {
        jmp_ret = std::make_shared<Jmp>(Jmp());
        uint64_t addr = s_to_uint64(args.substr(1, 18));
        jmp_ret->init(Jmp::str_to_jmp(jmp_str), addr);
    } else {
        jmp_ret->set_static(false);
    }

    return jmp_ret;
}

std::shared_ptr<Ins> parse_ins(std::string line, std::vector<uint64_t> &calls) {
    auto ins_ret = std::make_shared<Ins>();

    std::string ins_str = line.substr(43, 7);

    // strip whitespace
    ins_str.erase(remove_if(ins_str.begin(), ins_str.end(), isspace),
        ins_str.end());

    // check if the instruction is a jmp and parse it separately
    if(ins_str.at(0) == 'j') {
        ins_ret = parse_jmp(ins_str, line.substr(50));
    } else if(ins_str == "call") {
        ins_ret->set_type(InsType::CALL);
        uint64_t addr = 0xFFFFFFFFFFFFFFFF;
        if(line.at(50) == '$') {
            addr = s_to_uint64(line.substr(51, 68));
        }
        ins_ret->set_data(addr);

        if(std::find(calls.begin(), calls.end(), addr) == calls.end())
            calls.push_back(addr);
    } else if(ins_str == "ret") {
        ins_ret->set_type(InsType::RET);
    }


    // set the size of the instruction
    std::string bytes = line.substr(22, 20);
    int size = 0;
    for(char &c : bytes) {
        if(c != ' ') ++size;
    }
    size /= 2;
    ins_ret->set_size(size);

    // set the location of the instruction
    std::string loc = line.substr(2, 18);
    ins_ret->set_loc(s_to_uint64(loc));

    return ins_ret;
}

BlockFile parse_file(std::string path) {
    BlockFile block_file;
    
    std::ifstream file(path);
    std::vector<std::string> lines;

    for(std::string line; getline(file, line);) {
        lines.push_back(line);
    }
    file.close();

    uint64_t block_tag = 0xFFFFFFFFFFFFFFFF;
    vector_shared<Ins> ins;
    InsType last_ins_type = InsType::INS;

    for(std::string line : lines) {
        if(line.length() == 0) continue;

        // the only lines without a space in the first column are "Block #:" and
        // the block tags
        if(line.at(0) != ' ') {
            if(line.substr(0,5) != "Block") {
                block_tag = s_to_uint64(line);
            }
        }

        // if the line begines with a space, it details an instruction (usually)
        if(line.at(0) == ' ' && line.length() > 44) {
            std::shared_ptr<Ins> current_ins = parse_ins(line, block_file.calls);
            ins.push_back(current_ins);
            last_ins_type = current_ins->get_ins_type();
        }

        if(last_ins_type != InsType::INS ) {
            if(last_ins_type == InsType::JMP) {
                if(std::static_pointer_cast<Jmp>(ins.back())->get_jmp_type()
                    == JmpType::JMP) {
                    if(line.length() > 51 && line.substr(43, 8) == "jmp    <") {
                        // TODO: refactor this
                    } else {
                        last_ins_type = InsType::INS;
                        continue;
                    }
                }
            }

            if(block_tag == 0xFFFFFFFFFFFFFFFF) {
                block_tag = ins.at(0)->get_loc();
            }

            auto block = std::make_shared<BBlock>(block_tag, ins.at(0)->get_loc());

            // check if the basic block branches statically or not
            if(ins.back()->get_ins_type() != InsType::JMP) {
                auto last_jmp = std::make_shared<Jmp>();
                if(ins.back()->get_ins_type() == InsType::CALL) {
                    if(ins.back()->get_data() != 0xFFFFFFFFFFFFFFFF) {
                        last_jmp->set_static(true);
                        last_jmp->set_to(ins.back()->get_data());
                    } else {
                        last_jmp->set_static(false);
                    }
                } else {
                    last_jmp->set_static(false);
                }
                last_jmp->set_loc(ins.back()->get_loc());
                last_jmp->set_size(ins.back()->get_size());
                ins.push_back(last_jmp);
            }

            block->set_ins(ins);
            ins.clear();

            block_file.blocks.push_back(block);

            block_tag = 0xFFFFFFFFFFFFFFFF;
            last_ins_type = InsType::INS;
        }
    }

    return block_file;
}