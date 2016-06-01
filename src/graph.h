#ifndef __GRAPH_H_
#define __GRAPH_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "bblock.h"
#include "types.h"

class Graph {
public:
    Graph();
    Graph(vector_shared<BBlock> super_set);

    void init(vector_shared<BBlock> super_set);

    void insert(std::shared_ptr<BBlock> parent, std::shared_ptr<BBlock> child, bool jmp);
    std::shared_ptr<BBlock> search(uint64_t tag);

private:
    std::shared_ptr<BBlock> root;

    void init(vector_shared<BBlock> super_set, std::shared_ptr<BBlock> leaf);
    std::shared_ptr<BBlock> search(uint64_t tag, std::shared_ptr<BBlock> leaf);
};

#endif