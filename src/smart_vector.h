#ifndef __SMART_VECTOR_H_
#define __SMART_VECTOR_H_

#include <vector>
#include <memory>

template<typename T>
using vector_shared = std::vector<std::shared_ptr<T>>;

template<typename T>
using vector_weak = std::vector<std::weak_ptr<T>>;

// forward declare BBlock for the below function declaration
class BBlock;

/*
 * useful utility function which uses vector_shared
 * if tag is false, we search based on the given address
 * if tag is true, we assume the "address" is actually a block tag
 */
std::shared_ptr<BBlock> search_bblocks(vector_shared<BBlock> blocks,
    uint64_t addr, bool tag = false);

/*
 * lock the contents of a vector_weak
 */
template<class T>
vector_shared<T> vector_weak_lock(vector_weak<T> vector_param) {
    vector_shared<T> vector_ret;
    for(std::weak_ptr<T> i : vector_param) {
        vector_ret.push_back(i.lock());
    }
    return vector_ret;
}
#endif
