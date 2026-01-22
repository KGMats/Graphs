//
// Created by ra173048 on 10/5/25.
//

#ifndef SEMNOME_LINKEDLIST_H
#define SEMNOME_LINKEDLIST_H

#include <stdint.h>
#include "../include/Graph.h"

typedef struct StackFrame {
    Node* u;
    uint64_t neighbor_idx;  // O índice do próximo vizinho a ser explorado
} StackFrame;

// Sua LinkedList utilizada para armazenar um ponteiro para o estado (StackFrame)
typedef struct LinkedList {
    StackFrame* frame;
    struct LinkedList* next;
} LinkedList;

#endif //SEMNOME_LINKEDLIST_H
