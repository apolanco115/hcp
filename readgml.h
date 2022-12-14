// Header file for the parser for GML files
//
// Mark Newman  11 AUG 06
// modified by Austin Polanco 10 AUG 22

#ifndef HCP_READGML_H
#define HCP_READGML_H

#include <cstdio>
#include "network.h"

int read_network(NETWORK *network, std::string filepath);
void free_network(NETWORK *network);

#endif //HCP_READGML_H
