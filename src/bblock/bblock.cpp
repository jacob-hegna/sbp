#include "bblock.h"
/*
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<int> dist(0, 1);
*/

std::shared_ptr<BBlock> search_bblocks(vector_shared<BBlock> &blocks,
                                      uint64_t addr, bool tag) {
    std::shared_ptr<BBlock> block = nullptr;
    for(std::shared_ptr<BBlock> b : blocks) {
        uint64_t temp_addr; // will store either the iterator b's tag or addr
        if(tag) {
            temp_addr = b->get_tag();
        } else {
            temp_addr = b->get_loc();
        }
        if(addr == temp_addr) {
            block = b;
            break;
        }
    }

    return block;
}

BBlock::BBlock() {
    fall_count = 0;
    jmp_count  = 0;
    graph = nullptr;
}
BBlock::BBlock(uint64_t block_tag, uint64_t block_addr) : BBlock() {
    init(block_tag, block_addr);
}

void BBlock::init(uint64_t block_tag, uint64_t block_addr) {
    this->block_tag  = block_tag;
    this->block_addr = block_addr;
}

std::string BBlock::print_info() {
    std::stringstream ss;
    ss << "tag:\t" << std::hex << this->get_tag()  << std::endl
    << "addr\t"    << std::hex << this->get_loc()  << std::endl
    << "jmp:\t"    << std::hex << this->get_jmp()  << std::endl
    << "fall:\t"   << std::hex << this->get_fall() << std::endl;

    return ss.str();
}

std::string BBlock::print_ins() {
    std::stringstream ss;
    for(std::shared_ptr<Ins> i : ins) {
        InsType type = i->get_ins_type();
        if(type == InsType::INS) {
            ss << std::hex << i->get_loc() << std::flush << "\tINS" << std::endl;
        } else if(type == InsType::JMP) {
            std::shared_ptr<Jmp> j = std::static_pointer_cast<Jmp>(i);
            std::string jmp_type = Jmp::jmp_to_str(j->get_jmp_type());
            ss << std::hex << i->get_loc() << std::flush << "\t" << jmp_type
               << std::endl;
        } else if(type == InsType::CALL) {
            ss << std::hex << i->get_loc() << std::flush << "\tCALL" << std::endl;
        } else if(type == InsType::RET) {
            ss << std::hex << i->get_loc() << std::flush << "\tRET" << std::endl;
        }
    }
    return ss.str();
}

uint64_t BBlock::predict() {
    return this->combined_h();
}

std::shared_ptr<Jmp> BBlock::get_last() {
    return std::static_pointer_cast<Jmp>(ins.back());
}

bool BBlock::static_jmp() {
    return get_last()->get_static();
}

uint64_t BBlock::get_fall() {
    if(get_last() != nullptr) {
        return this->get_last()->get_loc()
            + this->get_last()->get_size();
    } else {
        return 0xFFFFFFFFFFFFFFFF;
    }
}

uint64_t BBlock::get_jmp() {
    if(get_last() != nullptr) {
        return this->get_last()->get_to();
    } else {
        return 0xFFFFFFFFFFFFFFFF;
    }
}

void BBlock::push_back(std::shared_ptr<Ins> i) {
    ins.push_back(i);
}

void BBlock::set_ins(vector_shared<Ins> ins) {
    this->ins = ins;
}

uint64_t BBlock::get_loc() {
    return block_addr;
}

uint64_t BBlock::get_tag() {
    return block_tag;
}

vector_shared<Ins> BBlock::get_ins() {
    return ins;
}

vector_weak<BBlock> BBlock::get_parents() {
    return parents;
}

std::unordered_map<uint64_t, uint64_t> BBlock::addr_to_tag_map;

void BBlock::set_addr_to_tag_map(std::unordered_map<uint64_t, uint64_t> map) {
    BBlock::addr_to_tag_map = map;
}
uint64_t BBlock::addr_to_tag(uint64_t addr) {
    auto search = BBlock::addr_to_tag_map.find(addr);
    if(search == BBlock::addr_to_tag_map.end()) {
        return 0xFFFFFFFFFFFFFFFF;
    } else {
        return (*search).second;
    }
}

std::shared_ptr<BBlock> BBlock::find(BlockSet &super_set,
                      uint64_t search, bool tag) {
    if(!tag) {
        search = BBlock::addr_to_tag(search);
    }
    auto result = super_set.find(search);
    if(result == super_set.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}