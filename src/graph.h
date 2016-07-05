#ifndef __GRAPH_H_
#define __GRAPH_H_

#include <stdint.h>
#include <vector>
#include <memory>
#include <queue>

#include "bblock.h"
#include "smart_vector.h"

std::queue<Graph> make_graphs(vector_shared<BBlock> super_set,
                               std::vector<uint64_t> calls);

class Graph {
public:
    Graph();
    Graph(vector_shared<BBlock> super_set, uint64_t addr);

    void init(vector_shared<BBlock> super_set, uint64_t addr);

    bool isolated(std::shared_ptr<BBlock> leaf = nullptr,
        vector_shared<BBlock> finished_blocks = vector_shared<BBlock>());

    bool dominator_check(std::shared_ptr<BBlock> block, std::shared_ptr<BBlock> dominator,
    vector_shared<BBlock> finished_blocks = vector_shared<BBlock>());

    std::shared_ptr<BBlock> search(uint64_t addr);

    std::string print_info(std::shared_ptr<BBlock> leaf = nullptr);
    void print_dot_file(std::string path);

    std::shared_ptr<BBlock> get_root();

private:
    std::shared_ptr<BBlock> root;
    vector_shared<BBlock> super_set;

    void init(std::shared_ptr<BBlock> leaf);
    std::shared_ptr<BBlock> search(uint64_t addr, std::shared_ptr<BBlock> leaf,
        bool recursion = false);

    std::string print_dot_file(std::shared_ptr<BBlock> leaf,
        vector_shared<BBlock> &completed);
};

#endif