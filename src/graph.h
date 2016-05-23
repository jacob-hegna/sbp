#ifndef __GRAPH_H_
#define __GRAPH_H_

#include "BBlock"

#include <stdint.h>
#include <vector>

class Node {
public:

private:
    Node *left, *right;
}

class Graph {
public:
    Graph();
    ~Graph();

    Node* search(uint64_t);
private:
    Node *root;
};

#endif