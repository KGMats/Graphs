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
    graph->original_ids = NULL;
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
    node->positive_edges = 0;
    graph->nodes[ID] = node;
    return node;
}

typedef struct {
    uint64_t source;
    uint64_t target;
    int sign;
} RawEdge;

static int compareRawEdges(const void *first, const void *second) {
    const RawEdge *edgeA = (const RawEdge *)first;
    const RawEdge *edgeB = (const RawEdge *)second;

    if (edgeA->source != edgeB->source) {
        return (edgeA->source < edgeB->source) ? -1 : 1;
    }
    if (edgeA->target != edgeB->target) {
        return (edgeA->target < edgeB->target) ? -1 : 1;
    }
    return 0;
}

SignedGraph *createSignedGraphFromFilename(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    uint64_t rawCapacity = 1024;
    uint64_t *rawDegrees = calloc(rawCapacity, sizeof(uint64_t));
    if (!rawDegrees) {
        perror("Erro ao alocar rawDegrees");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    uint64_t edgeCapacity = 1024;
    RawEdge *edgeList = malloc(edgeCapacity * sizeof(RawEdge));
    if (!edgeList) {
        perror("Erro ao alocar edgeList");
        free(rawDegrees);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    uint64_t maxRawId = 0;
    uint64_t edgeCount = 0;
    char line[512];

    while (fgets(line, sizeof(line), file)) {
        char *linePtr = line;
        while (*linePtr == ' ' || *linePtr == '\t') {
            linePtr++;
        }

        if (*linePtr == '\0' || *linePtr == '#' || *linePtr == '%' || *linePtr == '\n' || *linePtr == '\r') {
            continue;
        }

        uint64_t rawSource = 0;
        uint64_t rawTarget = 0;
        int rating = 0;

        if (3 != sscanf(linePtr, "%lu%*[ \t,;]%lu%*[ \t,;]%d", &rawSource, &rawTarget, &rating)) {
            continue;
        }

        if (rawSource == rawTarget || rating == 0) {
            continue;
        }

        uint64_t topRawId = (rawSource > rawTarget) ? rawSource : rawTarget;
        if (topRawId >= rawCapacity) {
            uint64_t newCapacity = rawCapacity;
            while (newCapacity <= topRawId) {
                newCapacity *= 2;
            }

            uint64_t *resizedDegrees = realloc(rawDegrees, newCapacity * sizeof(uint64_t));
            if (!resizedDegrees) {
                perror("Erro ao redimensionar rawDegrees");
                free(rawDegrees);
                free(edgeList);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            memset(resizedDegrees + rawCapacity, 0, (newCapacity - rawCapacity) * sizeof(uint64_t));
            rawDegrees = resizedDegrees;
            rawCapacity = newCapacity;
        }

        rawDegrees[rawSource]++;
        rawDegrees[rawTarget]++;

        if (topRawId > maxRawId) {
            maxRawId = topRawId;
        }

        if (edgeCount >= edgeCapacity) {
            edgeCapacity *= 2;
            RawEdge *resizedEdges = realloc(edgeList, edgeCapacity * sizeof(RawEdge));
            if (!resizedEdges) {
                perror("Erro ao redimensionar edgeList");
                free(rawDegrees);
                free(edgeList);
                fclose(file);
                exit(EXIT_FAILURE);
            }
            edgeList = resizedEdges;
        }

        edgeList[edgeCount].source = rawSource;
        edgeList[edgeCount].target = rawTarget;
        edgeList[edgeCount].sign = (rating > 0) ? 1 : -1;
        edgeCount++;
    }

    fclose(file);

    uint64_t uniqueNodeCount = 0;
    for (uint64_t rawId = 0; rawId <= maxRawId; rawId++) {
        if (rawDegrees[rawId] > 0) {
            uniqueNodeCount++;
        }
    }

    if (uniqueNodeCount == 0) {
        fprintf(stderr, "Erro: nenhuma aresta valida em '%s'\n", filename);
        free(rawDegrees);
        free(edgeList);
        return NULL;
    }

    uint64_t *rawToMapped = malloc((maxRawId + 1) * sizeof(uint64_t));
    uint64_t *originalIds = malloc(uniqueNodeCount * sizeof(uint64_t));
    if (!rawToMapped || !originalIds) {
        perror("Erro ao alocar mapas de IDs");
        free(rawDegrees);
        free(edgeList);
        exit(EXIT_FAILURE);
    }

    uint64_t currentMappedIndex = 0;
    for (uint64_t rawId = 0; rawId <= maxRawId; rawId++) {
        rawToMapped[rawId] = UINT64_MAX;
        if (rawDegrees[rawId] > 0) {
            rawToMapped[rawId] = currentMappedIndex;
            originalIds[currentMappedIndex] = rawId;
            currentMappedIndex++;
        }
    }
    free(rawDegrees);

    for (uint64_t i = 0; i < edgeCount; i++) {
        uint64_t mappedSource = rawToMapped[edgeList[i].source];
        uint64_t mappedTarget = rawToMapped[edgeList[i].target];

        if (mappedSource > mappedTarget) {
            uint64_t temp = mappedSource;
            mappedSource = mappedTarget;
            mappedTarget = temp;
        }

        edgeList[i].source = mappedSource;
        edgeList[i].target = mappedTarget;
    }
    free(rawToMapped);

    qsort(edgeList, edgeCount, sizeof(RawEdge), compareRawEdges);

    uint64_t uniqueEdgeCount = 0;
    for (uint64_t i = 0; i < edgeCount; i++) {
        if (uniqueEdgeCount > 0 &&
            edgeList[i].source == edgeList[uniqueEdgeCount - 1].source &&
            edgeList[i].target == edgeList[uniqueEdgeCount - 1].target) {
            edgeList[uniqueEdgeCount - 1].sign += edgeList[i].sign;
        } else {
            edgeList[uniqueEdgeCount++] = edgeList[i];
        }
    }

    for (uint64_t i = 0; i < uniqueEdgeCount; i++) {
        edgeList[i].sign = (edgeList[i].sign >= 0) ? 1 : -1;
    }

    uint64_t *nodeDegrees = calloc(uniqueNodeCount, sizeof(uint64_t));
    if (!nodeDegrees) {
        perror("Erro ao alocar nodeDegrees");
        free(originalIds);
        free(edgeList);
        exit(EXIT_FAILURE);
    }

    for (uint64_t i = 0; i < uniqueEdgeCount; i++) {
        nodeDegrees[edgeList[i].source]++;
        nodeDegrees[edgeList[i].target]++;
    }

    SignedGraph *graph = createSignedGraph(uniqueNodeCount);
    graph->original_ids = originalIds;

    for (uint64_t i = 0; i < uniqueNodeCount; i++) {
        createSignedNode(graph, i, nodeDegrees[i]);
    }
    free(nodeDegrees);

    for (uint64_t i = 0; i < uniqueEdgeCount; i++) {
        uint64_t sourceIndex = edgeList[i].source;
        uint64_t targetIndex = edgeList[i].target;
        int isPositive = (edgeList[i].sign > 0);

        SignedNode *sourceNode = graph->nodes[sourceIndex];
        SignedNode *targetNode = graph->nodes[targetIndex];

        sourceNode->neighbors[sourceNode->degree++] = tag_ptr(targetNode, isPositive);
        sourceNode->positive_edges += isPositive;

        targetNode->neighbors[targetNode->degree++] = tag_ptr(sourceNode, isPositive);
        targetNode->positive_edges += isPositive;
    }

    free(edgeList);
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
bool signedGraphIndexOf(const SignedGraph *graph, const uint64_t originalId, uint64_t *index) {
    if (graph->original_ids == NULL) {
        if (originalId >= graph->n_nodes) {
            return false;
        }
        *index = originalId;
        return true;
    }

    uint64_t left = 0;
    uint64_t right = graph->n_nodes;

    while (left < right) {
        uint64_t middle = left + (right - left) / 2;
        if (graph->original_ids[middle] < originalId) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }

    if (left < graph->n_nodes && graph->original_ids[left] == originalId) {
        *index = left;
        return true;
    }

    return false;
}

void activateSignedFromFile(const SignedGraph *graph, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return;
    }
    uint64_t originalId = 0;
    uint64_t mappedId = 0;
    while (1 == fscanf(file, "%lu", &originalId)) {
        if (!signedGraphIndexOf(graph, originalId, &mappedId)) {
            continue;
        }
        setSignedNodeState(graph->active_nodes, mappedId, 1);
        const SignedNode* node = graph->nodes[mappedId];
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

    if (graph->original_ids)
        free(graph->original_ids);

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
            uint64_t savedId = (graph->original_ids != NULL) ? graph->original_ids[node->ID] : node->ID;
            fprintf(file, "%lu\n", savedId);
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

