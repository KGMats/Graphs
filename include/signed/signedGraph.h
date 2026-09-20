#ifndef SIGNED_GRAPH_H
#define SIGNED_GRAPH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_NODES 4847573

typedef struct SignedNode SignedNode;
struct SignedNode {
    uint64_t ID;
    uint64_t degree;
    uint64_t positive_edges;
    int64_t n_active_neighbors;
    SignedNode **neighbors;
};

typedef struct signedGraph {
    uint64_t n_nodes;
    SignedNode **nodes;
    uint64_t *active_nodes;
} SignedGraph;

static inline SignedNode* get_clean_ptr(const SignedNode *ptr) {
    return (SignedNode*)((uintptr_t)ptr & ~1ULL);
}

static inline int get_edge_sign(const SignedNode *ptr) {
    return ((uintptr_t)ptr & 1ULL) ? 1 : -1;
}

static inline SignedNode* tag_ptr(const SignedNode *ptr, int is_positive) {
    uintptr_t p = (uintptr_t)ptr;
    if (is_positive) {
        p |= 1ULL; // Seta o bit para 1
        return (SignedNode*)p;
    }

    p &= ~1ULL; // Garante que o bit seja 0
    return (SignedNode*)p;
}

SignedGraph *createSignedGraph(uint64_t n_nodes);
SignedNode *createSignedNode(const SignedGraph *graph, uint64_t ID, uint64_t n_edges);
void setSignedNodeState(uint64_t *activeSignedNodesArray, uint64_t id, int newState);
int getSignedNodeState(const uint64_t *activeSignedNodesArray, uint64_t id);
SignedGraph *createSignedGraphFromFilename(const char *filename);
void activateSignedFromFile(const SignedGraph *graph, const char *filename);
void activateSignedFromIDArray(const SignedGraph *graph, const uint64_t *IDs, uint64_t n_ids);
void deactivateAllSigned(const SignedGraph *graph);
void freeSignedGraph(SignedGraph *graph);
void saveSignedGraphToFile(const SignedGraph *graph, const char *filename);
uint64_t countActiveSignedNodes(const SignedGraph *graph);
uint64_t threshold(SignedNode* node);


#endif //SIGNED_GRAPH_H
