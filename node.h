//
// Written by Austin Polanco on 14 JUL 22.
//

#ifndef HCP_NODE_H
#define HCP_NODE_H
#include <vector>
class node {

    public:
        std::vector<int> data;
        node* next;

        node(std::vector<int> data)
                :data(data), next(nullptr) {}

};

#endif //HCP_CPP_NODE_H
