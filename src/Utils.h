//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef GRAFOS_UTILS_H
#define GRAFOS_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "../include/Graph.h"

typedef struct {
    uint64_t ID;
    uint64_t degree;
} tuple;

tuple *orderedList(const Graph *graph);
tuple *orderedSetList(const Graph *graph, const uint64_t *IDsList);
void mergeSort(tuple *arr, uint64_t left, uint64_t right);
void printBits(size_t size, const char* ptr);
void removeIDFromList(uint64_t *list, uint64_t ID, uint64_t *size);

#endif //GRAFOS_UTILS_H