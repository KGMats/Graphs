#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "signed/signedGraph.h"

SignedGraph *createSignedGraph(const uint64_t n_nodes) {
    SignedGraph *graph = malloc(sizeof(SignedGraph));
    if (!graph) {
        perror("Erro ao alocar Graph");
        exit(EXIT_FAILURE);
    }
    graph->nodes = malloc(n_nodes * sizeof(SignedNode*));
    if (!graph->nodes) {
        perror("Erro ao alocar nodes");
        exit(EXIT_FAILURE);
    }
    graph->n_nodes = n_nodes;

    size_t n_chunks = (n_nodes / 40) + 1;
    graph->active_nodes = calloc(n_chunks, sizeof(uint64_t));
    return graph;
}

SignedNode *createSignedNode(const SignedGraph *graph, const uint64_t ID, const uint64_t n_edges) {
    SignedNode* node = malloc(sizeof(SignedNode));
    if (node == NULL) {
        perror("Falha ao alocar Node\n");
        exit(EXIT_FAILURE);
    }
    node->ID = ID;
    node->neighbors = calloc(n_edges, sizeof(SignedNode*));
    node->degree = 0;
    node->n_active_neighbors = 0;
    graph->nodes[ID] = node;
    return node;
}

SignedGraph *createSignedGraphFromFilename(const char *filename) {
    uint64_t id1 = 0;
    uint64_t id2 = 0;
    int edge_signal = 0;

    uint64_t *n_edges = calloc(MAX_NODES, sizeof(uint64_t));

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    while (3 == fscanf(file, "%lu %lu %d", &id1, &id2, &edge_signal)) {
        n_edges[id1] += 1;
        n_edges[id2] += 1;
    }

    fclose(file);

    uint64_t n_nodes = 0;
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (n_edges[i] == 0) {
            n_nodes = i;
            break;
        }
    }

    SignedGraph *graph = createSignedGraph(n_nodes);

    for (int i = 0; i < n_nodes; i++) {
        createSignedNode(graph, i, n_edges[i]);
    }

    file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    while (3 == fscanf(file, "%lu %lu %d", &id1, &id2, &edge_signal)) {
        SignedNode *n1 = graph->nodes[id1];
        SignedNode *n2 = graph->nodes[id2];

        int is_positive = (edge_signal == 1);
        n1->neighbors[n1->degree++] = tag_ptr(n2, is_positive);
        n1->positive_edges += is_positive;

        n2->neighbors[n2->degree++] = tag_ptr(n1, is_positive);
        n2->positive_edges += is_positive;
    }

    fclose(file);
    free(n_edges);

    return graph;
}

// Tabela de potências de 3 pré-calculadas (3^0 até 3^39)
static const uint64_t POW3[40] = {
    1ULL, 3ULL, 9ULL, 27ULL, 81ULL, 243ULL, 729ULL, 2187ULL, 6561ULL, 19683ULL,
    59049ULL, 177147ULL, 531441ULL, 1594323ULL, 4782969ULL, 14348907ULL, 43046721ULL, 
    129140163ULL, 387420489ULL, 1162261467ULL, 3486784401ULL, 10460353203ULL, 
    31381059609ULL, 94143178827ULL, 282429536481ULL, 847288609443ULL, 2541865828329ULL, 
    7625597484987ULL, 22876792454961ULL, 68630377364883ULL, 205891132094649ULL, 
    617673396283947ULL, 1853020188851841ULL, 5559060566555523ULL, 16677181699666569ULL, 
    50031545098999707ULL, 150094635296999121ULL, 450283905890997363ULL, 1350851717672992089ULL, 
    4052555153018976267ULL
};

// Gets the state of a signed node. Returns 0 (neutral), 1 (positive) or -1 (negative)
int getSignedNodeState(const uint64_t *activeNodesArray, const uint64_t id) {
    int signals[] = {0, 1, -1};
    uint64_t idx_chunk = id / 40;
    uint_fast8_t idx_local = id % 40;

    uint64_t chunk = activeNodesArray[idx_chunk];
    return signals[((chunk / POW3[idx_local]) % 3)];
}

void setSignedNodeState(uint64_t *activeNodesArray, const uint64_t id, const int newState) {
    uint64_t idx_chunk = id / 40;
    uint_fast8_t idx_local = id % 40;
    
    uint64_t chunk = activeNodesArray[idx_chunk];
    uint64_t p = POW3[idx_local];
    
    int currentState = (int)((chunk / p) % 3);
    int stateVal = (newState == -1) ? 2 : newState;
    
    chunk = chunk - (currentState * p) + (stateVal * p);

    activeNodesArray[idx_chunk] = chunk;
}

// Função que lê um arquivo em que cada linha é um numero e ativa
// os nós cujos IDs são iguais as linhas do arquivo.
void activateSignedFromFile(const SignedGraph *graph, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    uint64_t id;
    while (1 == fscanf(file, "%lu", &id)) {
        setSignedNodeState(graph->active_nodes, id, 1);
        const SignedNode* node = graph->nodes[id];
        for (uint64_t i = 0; i < node->degree; i++) {
            SignedNode *tagged_neighbor = node->neighbors[i];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += edge_sign;
        }
    }
    fclose(file);
}

void activateSignedFromIDArray(const SignedGraph *graph, const uint64_t *IDs, const uint64_t n_ids) {
    for (uint64_t i = 0; i < n_ids; i++) {
        setSignedNodeState(graph->active_nodes, IDs[i], 1);
        const SignedNode *node = graph->nodes[IDs[i]];
        for (uint64_t j = 0; j < node->degree; j++) {
            SignedNode *tagged_neighbor = node->neighbors[j];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += edge_sign;
        }
    }
}

void deactivateAllSigned(const SignedGraph *graph) {
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        graph->nodes[i]->n_active_neighbors = 0;
    }
    size_t n_chunks = (graph->n_nodes / 40) + 1;
    memset(graph->active_nodes, 0, n_chunks * sizeof(uint64_t));
}

void freeSignedGraph(SignedGraph *graph) {
    if (!graph) return;

    if (graph->nodes) {
        for (uint64_t i = 0; i < graph->n_nodes; i++) {
            if (graph->nodes[i]) {
                if (graph->nodes[i]->neighbors)
                    free(graph->nodes[i]->neighbors);
                free(graph->nodes[i]);
            }
        }
        free(graph->nodes);
    }

    if (graph->active_nodes)
        free(graph->active_nodes);

    free(graph);
}

void saveSignedGraphToFile(const SignedGraph *graph, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao tentar abrir o arquivo");
        return;
    }

    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        const SignedNode* node = graph->nodes[i];
        if (getSignedNodeState(graph->active_nodes, node->ID)) {
            fprintf(file, "%lu\n", node->ID);
        }
    }
    fclose(file);
}

uint64_t countActiveSignedNodes(const SignedGraph* graph) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        if (getSignedNodeState(graph->active_nodes, i)) {
            count++;
        }
    }
    return count;
}

uint64_t threshold(SignedNode* node)
{
    return 1;
}
