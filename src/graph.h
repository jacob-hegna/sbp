#ifndef __GRAPH_H_
#define __GRAPH_H_

#include <stdint.h>
#include <vector>

#include "bblock.h"

class Graph {
public:
    Graph();
    ~Graph();

    void insert(uint64_t addr);
    BBlock* search(uint64_t addr);
    void delete_tree();

private:
    BBlock *root;

    void insert(uint64_t addr, BBlock *parent, bool jmp);
    BBlock* search(uint64_t addr, BBlock *leaf);
    void delete_tree(BBlock *leaf);
};

#endif