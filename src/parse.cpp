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

std::shared_ptr<Jmp> parse_jmp(std::string jmp_str, std::string args) {
    std::shared_ptr<Jmp> jmp_ret(new Jmp());

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

std::shared_ptr<Ins> parse_ins(std::string ins_str, std::string args) {
    std::shared_ptr<Ins> ins_ret(new Ins());

    // strip whitespace
    ins_str.erase(remove_if(ins_str.begin(), ins_str.end(), isspace),
        ins_str.end());

    if(ins_str.at(0) == 'j') {
        ins_ret = parse_jmp(ins_str, args);
    }

    return ins_ret;
}

std::map<uint64_t, BBlock> parse_file(std::string path) {
    std::map<uint64_t, BBlock> super_set;

    std::ifstream file(path);
    std::vector<std::string> lines;

    for(std::string line; getline(file, line);) {
        lines.push_back(line);
    }

    bool open_tag = false;
    uint64_t block_tag;
    std::vector<std::shared_ptr<Ins>> ins;
    for(std::string line : lines) {
        if(line.substr(0, 8) == "dispatch") {
            block_tag = s_to_uint64(line.substr(21));
            open_tag = true;
        } else if(open_tag) {
            if(line.length() < 50) {
                if(line.length() == 0) {
                    open_tag = false;
                    BBlock block(block_tag);
                    if(ins.back()->get_ins_type() != InsType::JMP) {
                        std::shared_ptr<Jmp> last_jmp(new Jmp());
                        last_jmp->set_static(false);
                        ins.push_back(last_jmp);
                    }
                    block.set_ins(ins);
                    ins.clear();

                    super_set[block_tag] = block;                    
                }
            } else {
                std::string ins_str = line.substr(43, 7);
                std::shared_ptr<Ins> current_ins = parse_ins(ins_str, line.substr(50));
                if(current_ins != nullptr)
                    current_ins->set_loc(s_to_uint64(line.substr(2,19)));
                ins.push_back(current_ins);
            }
        }

    }

    file.close();

    return super_set;
}
