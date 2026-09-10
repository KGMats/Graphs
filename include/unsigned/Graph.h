#ifndef SEMNOME_GRAPH_H
#define SEMNOME_GRAPH_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_NODES 4847573

typedef struct Node Node;
struct Node{
    uint64_t ID;
    uint64_t n_edges;
    uint64_t n_neighbors;
    uint64_t n_active_neighbors;
    Node **neighbors;
};

typedef struct Graph {
    uint64_t n_edges;
    uint64_t n_nodes;
    Node **nodes;
    char *active_nodes;
} Graph;


Graph *createGraph(uint64_t n_nodes);
Node *createNode(const Graph *graph, uint64_t ID, uint64_t n_edges);
void setNodeState(char *activeNodesArray, uint64_t id, int newState);
bool getNodeState(const char* activeNodesArray, uint64_t id);
Graph *createGraphFromFilename(const char *filename);
void activateFromFile(const Graph *graph, const char *filename);
void activateFromIDArray(const Graph *graph, const uint64_t *IDs, uint64_t n_ids);
void deactivateAll(const Graph *graph);
void freeGraph(Graph *graph);
void saveGraphToFile(Graph *graph, const char* filename);
uint64_t countActiveNodes(const Graph* graph);
#endif //SEMNOME_GRAPH_H
