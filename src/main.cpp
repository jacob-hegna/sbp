#include <iostream>
#include <string>

#include "bblock.h"
#include "parse.h"
#include "graph.h"

int main(int argc, char *argv[]) {
    std::string path;
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    path = std::string(argv[1]);

    BlockFile block_file = parse_file(path);

    std::vector<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);


    Graph graph;
    for(Graph g : graphs) {
        if(g.isolated() == false) {
            graph = g;
        }
    }
    
    graph.print_dot_file("dot.txt");
/*

    for(std::shared_ptr<BBlock> block : block_file.blocks) {
        block->predict();
    }

    uint total = 0;
    float accuracy = check_predictions(path, block_file.blocks, total);

    std::cout << 100 * accuracy << " " << total << std::endl;

    return 0;
*/
}
