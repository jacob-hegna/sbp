#ifndef __VECTOR_SHARED_H_
#define __VECTOR_SHARED_H_

#include <vector>
#include <memory>

template<typename T>
using vector_shared = std::vector<std::shared_ptr<T>>;

#endif
