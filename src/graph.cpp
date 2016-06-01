#include "graph.h"

Graph::Graph() {
    root = nullptr;
}

Graph::~Graph() {
    delete_tree();
}

void Graph::insert(uint64_t addr) {
    BBlock *new_block = new BBlock(0x0, addr);

    root = new_block;
}

void Graph::insert(uint64_t addr, BBlock *parent, bool jmp) {
    BBlock *new_block = new BBlock(0x0, addr);

    if(jmp) {
        parent->jmp = new_block;
    } else {
        parent->fall = new_block;
    }
}

BBlock* Graph::search(uint64_t addr) {
    return search(addr, root);
}

BBlock* Graph::search(uint64_t addr, BBlock *leaf) {
    if(leaf != nullptr) {
        if(leaf->get_loc() == addr) {
            return leaf;
        } else {
            BBlock *ret = nullptr;
            if((ret = search(addr, leaf->fall)) != nullptr) {
                return ret;
            } else if((ret = search(addr, leaf->jmp)) != nullptr) {
                return ret;
            }
        }
    }
    return nullptr;
}

void Graph::delete_tree() {
    delete_tree(root);
}

void Graph::delete_tree(BBlock *leaf) {
    if(leaf != nullptr) {
        delete_tree(leaf->fall);
        delete_tree(leaf->jmp);
        delete leaf;
    }
}