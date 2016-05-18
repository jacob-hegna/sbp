#ifndef __FILE_H_
#define __FILE_H_

#include <vector>

class File {
public:
    File() {
        
    }
    ~File() {
        
    }
private:
    std::vector<BBlock> blocks;
};

#endif