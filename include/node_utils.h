#ifndef NODE_UTILS_H
#define NODE_UTILS_H

#include "structs.h"

DynamicNodeArray *init_dynamic_node_array(int initial_capacity);
void resize_dynamic_node_array(DynamicNodeArray *dna, int new_capacity);
void push_node(DynamicNodeArray *dna, Node *node);
void free_dynamic_node_array(DynamicNodeArray *dna);

#endif