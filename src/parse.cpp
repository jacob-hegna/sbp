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

std::shared_ptr<Ins> parse_ins(std::string line) {
    std::shared_ptr<Ins> ins_ret(new Ins());

    std::string ins_str = line.substr(43, 7);

    // strip whitespace
    ins_str.erase(remove_if(ins_str.begin(), ins_str.end(), isspace),
        ins_str.end());

    // check if the instruction is a jmp and parse it separately
    if(ins_str.at(0) == 'j') {
        ins_ret = parse_jmp(ins_str, line.substr(50));
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

vector_shared<BBlock> parse_file(std::string path) {
    vector_shared<BBlock> super_set;

    std::ifstream file(path);
    std::vector<std::string> lines;

    for(std::string line; getline(file, line);) {
        lines.push_back(line);
    }

    bool open_tag = false;
    uint64_t block_tag;
    vector_shared<Ins> ins;

    for(std::string line : lines) {
        // newline after every block signifies the end of the block
        // TODO: instead of newline checking, end the block definition
        // once a certain instruction is reached (JNZ, RET, etc)
        if(line.length() == 0 && open_tag) {
            open_tag = false;
            std::shared_ptr<BBlock> block(new BBlock(block_tag, ins.at(0)->get_loc()));

            // check if the basic block branches statically or not
            if(ins.back()->get_ins_type() != InsType::JMP) {
                std::shared_ptr<Jmp> last_jmp(new Jmp());
                last_jmp->set_static(false);
                last_jmp->set_loc(ins.back()->get_loc());
                last_jmp->set_size(ins.back()->get_size());
                ins.push_back(last_jmp);
            }


            block->set_ins(ins);
            ins.clear();

            super_set.push_back(block);

            continue;
        }

        // the only lines without a space in the first column are "Block #:" and
        // the block tags
        if(line.at(0) != ' ') {
            if(line.substr(0,5) != "Block") {
                open_tag = true;
                block_tag = s_to_uint64(line);
            }
        }

        // if the line begines with a space, it details an instruction (usually)
        if(line.at(0) == ' ' && line.length() > 44) {
            std::shared_ptr<Ins> current_ins = parse_ins(line);
            ins.push_back(current_ins);
        }
    }

    file.close();

    return super_set;
}
