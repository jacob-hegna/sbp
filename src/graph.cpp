#include "graph.h"

/*
 * TO FIX
 *  - calls to dynamic addresses
 *  - ending on the correct RET
 */

std::vector<Graph> make_graphs(vector_shared<BBlock> super_set,
                               std::vector<uint64_t> calls) {
    std::vector<Graph> graphs;
    std::vector<uint64_t> finished_calls;

    int a = 0;
    while(finished_calls.size() != calls.size()) {
        for(uint64_t call : calls) {
            if(std::find(finished_calls.begin(), finished_calls.end(), call)
                != finished_calls.end()) continue;
            if(Graph::good_call(super_set, call, finished_calls)) {
                graphs.push_back(Graph(super_set, call));

                // add the call from the regular vector to the finished vector
                finished_calls.push_back(call);
            }
        }
        if(a++ > 1000) break;
    }

    return graphs;
}

bool Graph::good_call(vector_shared<BBlock> super_set, uint64_t addr,
                      const std::vector<uint64_t> finished_calls) {
    std::shared_ptr<BBlock> block = nullptr;
    for(std::shared_ptr<BBlock> i : super_set) {
        block = i;
        if(block->get_loc() == addr) break;
    }

    // we return true, because even though we cannot find a block matching the
    // addr, returning false creates an infinite loop - but returning true
    // creates the desired output
    if(block == nullptr) return true;

    // record how many calls are made inside the relevant call
    static int depth = 0;

    // if the call block has another call inside of it, it's "bad"
    for(std::shared_ptr<Ins> ins : block->get_ins()) {
        if(ins->get_ins_type() == InsType::CALL) {
            ++depth;
            bool finished = false;
            // ...unless that call and all sub-calls have already been accounted
            // for
            for(uint64_t finished_call : finished_calls) {
                if(addr == finished_call) {
                    finished = true;
                    break;
                }
            }
            if(!finished) return false;
        }
    }

    vector_shared<Ins> ins = block->get_ins();
    if(ins.size() >= 2) {
        if(ins[ins.size()-2]->get_ins_type() == InsType::RET) {
            if(depth == 0) {
                return true;
            }
            --depth;
        }
    }

    return good_call(super_set, block->get_jmp(), finished_calls);
           good_call(super_set, block->get_fall(), finished_calls);
}

Graph::Graph() {
    root = nullptr;
}

Graph::Graph(vector_shared<BBlock> super_set, uint64_t addr) {
    init(super_set, addr);
}

void Graph::init(vector_shared<BBlock> super_set, uint64_t leaf) {
    
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