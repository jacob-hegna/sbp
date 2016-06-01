#ifndef __TYPES_H_
#define __TYPES_H_

#include <vector>
#include <memory>

template<typename T>
using vector_shared = std::vector<std::shared_ptr<T>>;

#endif