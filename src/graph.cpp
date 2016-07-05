#include "graph.h"

#include <fstream>

std::queue<Graph> make_graphs(vector_shared<BBlock> super_set,
                               std::vector<uint64_t> calls) {
    std::queue<Graph> graphs;

    for(uint64_t call : calls) {
        Graph graph(super_set, call);
        if(graph.get_root() != nullptr) {
            graphs.push(graph);
        }
    }

    return graphs;
}

Graph::Graph() {
    root = nullptr;
}

Graph::Graph(vector_shared<BBlock> super_set, uint64_t addr) {
    root = nullptr;
    init(super_set, addr);
}

/*
 * root initialization function
 * calls the recursive init()
 * public
 */
void Graph::init(vector_shared<BBlock> super_set, uint64_t leaf) {
    this->super_set = super_set;

    for(std::shared_ptr<BBlock> block : super_set) {
        if(block->get_loc() == leaf) root = block;
    }

    init(root);
}

/*
 * recursive initialization function
 * finds the stems of the tree
 * private
 */
void Graph::init(std::shared_ptr<BBlock> leaf) {
    if(leaf == nullptr) return;

    leaf->graph = this;

    for(std::shared_ptr<BBlock> block : super_set) {
        auto parents_shared = vector_weak_lock<BBlock>(block->parents);

        if(block->get_loc() == leaf->get_fall()) {
            if(search(block->get_loc()) == nullptr) {
                leaf->fall = block;
                block->parents.push_back(leaf);
                init(block);
            }
            bool child = (std::find(parents_shared.begin(), parents_shared.end(),
                                    leaf) != parents_shared.end());
            if(!child) {
                block->parents.push_back(leaf);
                leaf->fall = block;
            }
        }

        if(block->get_loc() == leaf->get_jmp()) {
            if(leaf->get_ins().size() >= 2) {
                if(leaf->get_ins().end()[-2]->get_ins_type() == InsType::CALL) {
                    continue;
                }
            }
            if(search(block->get_loc()) == nullptr) {
                leaf->jmp = block;
                block->parents.push_back(leaf);
                init(block);
            }
            bool child = (std::find(parents_shared.begin(), parents_shared.end(),
                                    leaf) != parents_shared.end());
            if(!child) {
                block->parents.push_back(leaf);
                leaf->jmp = block;
            }
        }
    }
}

bool Graph::isolated(std::shared_ptr<BBlock> leaf,
                            vector_shared<BBlock> finished_blocks) {
    if(leaf == nullptr) leaf = root;

    if(std::find(finished_blocks.begin(), finished_blocks.end(), leaf)
        != finished_blocks.end()) {
        return true;
    }
    finished_blocks.push_back(leaf);

    bool correct = dominator_check(leaf, root);

    if(!correct) {
        std::cout << std::hex << leaf->get_loc() << std::endl;
        return false;
    }

    if(leaf->jmp.use_count()  != 0)  correct = correct && isolated(leaf->jmp.lock(),  finished_blocks);
    if(leaf->fall.use_count() != 0) correct = correct && isolated(leaf->fall.lock(), finished_blocks);

    return correct;
}

bool Graph::dominator_check(std::shared_ptr<BBlock> block, std::shared_ptr<BBlock> dominator,
                            vector_shared<BBlock> finished_blocks) {
    static std::shared_ptr<BBlock> original;
    // first iteration of the recursion
    if(finished_blocks.size() == 0) {
        original = block;
    }

    if(std::find(finished_blocks.begin(), finished_blocks.end(), block)
        != finished_blocks.end()) {
        return true;
    }
    finished_blocks.push_back(block);

    if(block == dominator) return true;
    if(block->parents.size() == 0) return false;
    if(block->parents.size() == 1) {
        if(block->parents.at(0).lock() == dominator)  return true;
        if(block->parents.at(0).lock() == this->root) return false;
    }
    auto parents_lock = vector_weak_lock<BBlock>(block->parents);
    for(std::shared_ptr<BBlock> parent : parents_lock) {
        //if(block != original && parent == original) return false;
        if(dominator_check(parent, dominator, finished_blocks) == false) {
            return false;
        }
    }
    return true;
}


std::shared_ptr<BBlock> Graph::search(uint64_t addr) {
    return search(addr, root);
}

std::shared_ptr<BBlock> Graph::search(uint64_t addr, std::shared_ptr<BBlock> leaf,
                                      bool recursion) {
    static vector_shared<BBlock> searched_nodes;

    if(!recursion) {
        searched_nodes = vector_shared<BBlock>();
    }

    if(leaf == nullptr) return nullptr;

    if(std::find(searched_nodes.begin(), searched_nodes.end(), leaf)
         != searched_nodes.end()) {
        return nullptr;
    } else {
        searched_nodes.push_back(leaf);
    }

    if(leaf->get_loc() == addr) {
        return leaf;
    } else {
        std::shared_ptr<BBlock> ret = nullptr;
        if((ret = search(addr, leaf->fall.lock(), true)) != nullptr) {
            return ret;
        } else if((ret = search(addr, leaf->jmp.lock(), true)) != nullptr) {
            return ret;
        }
    }

    return nullptr;
}

std::string Graph::print_info(std::shared_ptr<BBlock> leaf) {
    std::stringstream ss;

    if(leaf == nullptr) leaf = root;
    ss << leaf->print_info() << std::endl;

    if(leaf->jmp.use_count() != 0)  ss << print_info(leaf->jmp.lock());
    if(leaf->fall.use_count() != 0) ss << print_info(leaf->fall.lock());

    return ss.str();
}

void Graph::print_dot_file(std::string path) {
    std::ofstream file(path);

    file << "digraph g {" << std::endl;
    vector_shared<BBlock> completed;
    file << print_dot_file(root, completed);
    file << "}";

    file.close();
}

std::string Graph::print_dot_file(std::shared_ptr<BBlock> leaf,
                                  vector_shared<BBlock> &completed) {
    std::stringstream ss;

    if(leaf == nullptr) return "";
    if(std::find(completed.begin(), completed.end(), leaf) != completed.end()) {
        return "";
    }
    completed.push_back(leaf);

    if(leaf->fall.use_count() != 0) {
        ss << "\t\"0x" << std::hex << leaf->get_loc()  << "\" -> \"0x"
                       << std::hex << leaf->get_fall() << "\""
                       << " [label=\" fall\"];"        << std::endl;
        ss << print_dot_file(leaf->fall.lock(), completed);
    }

    if(leaf->jmp.use_count() != 0) {
        ss << "\t\"0x" << std::hex << leaf->get_loc() << "\" -> \"0x"
                       << std::hex << leaf->get_jmp() << "\""
                       << " [label=\" jmp\"];"        << std::endl;
        ss << print_dot_file(leaf->jmp.lock(), completed);
    }

    return ss.str();
}

std::shared_ptr<BBlock> Graph::get_root() {
    return root;
}
