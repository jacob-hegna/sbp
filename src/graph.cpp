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
    for(std::shared_ptr<BBlock> block : super_set) {
        if(block->get_loc() == leaf->get_fall()) {
            this->insert(root, block, false);
            this->init(super_set, block);
        }
        if(block->get_loc() == leaf->get_jmp()) {
            this->insert(root, block, true);
            this->init(super_set, block);
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