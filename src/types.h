#ifndef __TYPES_H_NOCONFLICT
#define __TYPES_H_NOCONFLICT

#include <vector>
#include <memory>

template<typename T>
using vector_shared = std::vector<std::shared_ptr<T>>;

#endif
