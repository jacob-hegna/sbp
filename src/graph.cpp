#include "graph.h"

Graph::Graph() {
    root = nullptr;
}

Graph::Graph(vector_shared<BBlock> super_set) {
    init(super_set);
}

void Graph::init(vector_shared<BBlock> super_set) {
    root = super_set.at(0);
    init(super_set, root);
}

void Graph::init(vector_shared<BBlock> super_set, std::shared_ptr<BBlock> leaf) {
    
    // if the block has a dynamic branch, we can only 
    if(leaf->static_jmp() == false) {
        auto it = std::next(std::find(super_set.begin(), super_set.end(), leaf));
        this->insert(leaf, *it, true);
        this->init(super_set, *it);
    }

    for(std::shared_ptr<BBlock> block : super_set) {
        if(block->get_loc() == leaf->get_fall()) {
            this->insert(leaf, block, false);
            this->init(super_set, block);
        }
        if(block->get_loc() == leaf->get_jmp()) {
            this->insert(leaf, block, true);
            if(block->get_loc() != leaf->get_loc()) this->init(super_set, block);
        }
    }
}

void Graph::insert(std::shared_ptr<BBlock> parent, std::shared_ptr<BBlock> child,
                bool is_jmp) {
    if(is_jmp) {
        parent->jmp = child;
    } else {
        parent->fall = child;
    }
}

std::shared_ptr<BBlock> Graph::search(uint64_t tag) {
    return search(tag, root);
}

std::shared_ptr<BBlock> Graph::search(uint64_t tag, std::shared_ptr<BBlock> leaf) {
    if(leaf != nullptr) {
        if(leaf->get_tag() == tag) {
            return leaf;
        } else {
            std::shared_ptr<BBlock> ret = nullptr;
            if((ret = search(tag, leaf->fall)) != nullptr) {
                return ret;
            } else if((ret = search(tag, leaf->jmp)) != nullptr) {
                return ret;
            }
        }
    }
    return nullptr;
}