#ifndef __GRAPH_H_
#define __GRAPH_H_

#include <stdint.h>
#include <vector>
#include <memory>

#include "bblock.h"
#include "vector_shared.h"

std::vector<Graph> make_graphs(vector_shared<BBlock> super_set,
                               std::vector<uint64_t> calls);

class Graph {
public:
    Graph();
    Graph(vector_shared<BBlock> super_set, uint64_t addr);

    static bool good_call(vector_shared<BBlock> super_set, uint64_t addr,
        const std::vector<uint64_t> finished_calls = std::vector<uint64_t>());

    void init(vector_shared<BBlock> super_set, uint64_t addr);

    void insert(std::shared_ptr<BBlock> parent, std::shared_ptr<BBlock> child, bool jmp);
    std::shared_ptr<BBlock> search(uint64_t addr);

    std::shared_ptr<BBlock> get_root();

private:
    std::shared_ptr<BBlock> root;
    vector_shared<BBlock> super_set;

    void init(std::shared_ptr<BBlock> leaf);
    std::shared_ptr<BBlock> search(uint64_t addr, std::shared_ptr<BBlock> leaf);
};

#endif