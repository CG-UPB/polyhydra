//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_DIJKSTRA_H
#define VOLUMESHOS_DIJKSTRA_H

#include <vector>

class dijkstra {
public:
    void init();
    void run();
    void step();
private:
    std::vector<float> distances;
    std::vector<int> predecessors;
    int vertices_amount = 0;
};


#endif //VOLUMESHOS_DIJKSTRA_H
