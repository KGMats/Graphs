//
// Created by Elon Pereira Neto on 24/10/25.
//

#include <stdlib.h>
#include <stdio.h>

#include "Utils.h"


tuple *orderedList(const Graph *graph) {
    tuple *list = malloc(sizeof(tuple) * graph->n_nodes);
    for (int i = 0; i < graph->n_nodes; i++) {
        list[i].ID = i;
        list[i].degree = graph->nodes[i]->n_neighbors;
    }

    mergeSort(list, 0, graph->n_nodes - 1);

    return list;
}

tuple *orderedSetList(const Graph *graph, const uint64_t *IDsList) {
    const uint64_t n_nodes = graph->n_nodes;
    uint64_t n_ids = 0;
    tuple *list = calloc(n_nodes,sizeof(tuple));
    for (uint64_t i = 0; IDsList[i] != UINT64_MAX; i++) {
        list[i].ID = IDsList[i];
        list[i].degree = graph->nodes[IDsList[i]]->n_neighbors;
        n_ids++;
    }

    uint64_t id_search = 0;
    for (uint64_t i = 0; i < n_nodes; i++) {
        bool isInList = false;
        for (uint64_t j = 0; j < n_ids; j++) if (i == IDsList[j]) isInList = true;

        if (!isInList) {
            list[id_search + n_ids].ID = i;
            list[id_search + n_ids].degree = graph->nodes[i]->n_neighbors;
            id_search++;
        }
    }

    mergeSort(list, 0, n_ids - 1);
    mergeSort(list, n_ids, n_nodes - 1);
    return list;
}



// Função auxiliar do merge sort
void merge(tuple *arr, const uint64_t left, const uint64_t mid, const uint64_t right) {
    const uint64_t n1 = mid - left + 1;
    const uint64_t n2 = right - mid;

    tuple *L = malloc(n1 * sizeof(tuple));
    tuple *R = malloc(n2 * sizeof(tuple));

    for (uint64_t i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (uint64_t j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    uint64_t i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i].degree <= R[j].degree)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(tuple *arr, const uint64_t left, const uint64_t right) {
    if (arr == NULL) {
        printf("tuple argument null\n");
        exit(0);
    };
    
    if (left < right) {
        const uint64_t mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

void printBits(size_t size, const char* ptr) {
    for (uint64_t i = 0; i < size; i++) {
        printf("%c", getNodeState(ptr, i ) ? '1' : '0');

        // Adicionando um espaco a cada 8 bits para tornar mais legivel
        if ((i + 1) % 8 == 0 && (i + 1) < size) {
            printf(" ");
        }
    }
    printf("\n");
}

void removeIDFromList(uint64_t *list, const uint64_t ID, uint64_t *size) {
    uint64_t IDPosition = -1;
    for (uint64_t i = 0; i < *size; i++) {
        if (list[i] == ID) {
            IDPosition = i;
        }
    }

    if (IDPosition != -1) {
        *size -= 1;
        if (IDPosition != *size) {
            list[IDPosition] = list[*size];
            list[*size] = 0;
        }
    }
}