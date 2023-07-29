#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

void memory_error()
{
    fprintf(stderr, "メモリ確保に失敗したらしいです\n");
    exit(1);
}

DynamicNodeArray *init_dynamic_node_array(int initial_capacity)
{
    DynamicNodeArray *dna = calloc(1, sizeof(DynamicNodeArray));
    if (!dna)
        memory_error();
    dna->data = calloc(initial_capacity, sizeof(Node *));
    if (!dna->data)
        memory_error();
    dna->capacity = initial_capacity;
    dna->len = 0;

    return dna;
}

void resize_dynamic_node_array(DynamicNodeArray *dna, int new_capacity)
{
    Node **new_data = realloc(dna->data, new_capacity * sizeof(Node *));
    if (!new_data)
        memory_error();
    dna->data = new_data;
    dna->capacity = new_capacity;
};

void push_node(DynamicNodeArray *dna, Node *node)
{
    if (dna->len >= dna->capacity)
        resize_dynamic_node_array(dna, dna->capacity * 2);
    dna->data[dna->len++] = node;
};

void free_dynamic_node_array(DynamicNodeArray *dna)
{
    free(dna->data);
    free(dna);
};