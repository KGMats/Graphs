#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../Graph.h"


Graph *createGraph(const uint64_t n_nodes) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        perror("Erro ao alocar Graph");
        exit(EXIT_FAILURE);
    }
    graph->nodes = malloc(n_nodes * sizeof(Node*));
    if (!graph->nodes) {
        perror("Erro ao alocar nodes");
        exit(EXIT_FAILURE);
    }
    graph->n_edges = 0;
    graph->n_nodes = 0;
    graph->active_nodes = malloc((n_nodes / 8) + 1);
    memset(graph->active_nodes, 0, (n_nodes / 8) + 1);
    return graph;
}

Node *createNode(const Graph *graph, const uint64_t ID, const uint64_t n_edges) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL)
    {
        perror("Falha ao alocar Node\n");
        exit(EXIT_FAILURE);
    }
    node->ID = ID;
    node->neighbors = calloc(n_edges, sizeof(Node*));
    node->n_edges = 0;
    node->n_neighbors = 0;
    node->n_active_neighbors = 0;
    graph->nodes[ID] = node;
    return node;
}

void connectNodes(Graph* graph, const Node *n1, const Node *n2) {
    Edge *edge = malloc(sizeof(Edge));
    edge->node1 = n1;
    edge->node2 = n2;
    graph->edges[graph->n_edges] = edge;
    graph->n_edges++;
}

Graph *createGraphFromFilename(const char *filename) {

    uint64_t id1 = 0;
    uint64_t id2 = 0;

    uint64_t *n_edges = calloc(MAX_NODES, sizeof(uint64_t));
    uint64_t n_total_edges = 0;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    while (2 == fscanf(file, "%lu %lu", &id1, &id2)) {
        n_edges[id1] += 1;
        n_edges[id2] += 1;
        n_total_edges += 1;
    }

    fclose(file);

    uint64_t n_nodes = 0;
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (n_edges[i] == 0) {
            n_nodes = i;
            break;
        }
    }

    Graph *graph = createGraph(n_nodes);
    graph->edges = malloc(sizeof (Edge*) * n_total_edges);
    if (!graph->edges) {
        perror("Erro ao alocar edges");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_nodes; i++) {
        createNode(graph, i, n_edges[i]);
        graph->nodes[i]->n_neighbors = n_edges[i];
        graph->n_nodes++;
    }

    file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    while (2 == fscanf(file, "%lu %lu", &id1, &id2)) {
        Node *n1 = graph->nodes[id1];
        n1->neighbors[n1->n_edges++] = graph->nodes[id2];

        Node *n2 = graph->nodes[id2];
        n2->neighbors[n2->n_edges++] = graph->nodes[id1];
        connectNodes(graph, n1, n2);
    }

    fclose(file);
    free(n_edges);

    return graph;

}



// Função de set em um array de bits. Utilizando bits ao invez de um array de bools
// economizamos por volta de 87% de memória.
void setNodeState(char *activeNodesArray, const uint64_t id, const int newState) {
    const uint64_t i = id / 8;
    const uint64_t j = id % 8;
    const char mask = (char) (0b10000000 >> j);

    if (newState == 1) {
        activeNodesArray[i] = activeNodesArray[i] | mask;
    }
    else {
        activeNodesArray[i] = activeNodesArray[i] & ~mask;
    }
}



// Função que retorna o valor de um bit em um array de bits.
bool getNodeState(const char* activeNodesArray, const uint64_t id) {
    const uint64_t i = id / 8;
    const uint64_t j = id % 8;
    const char mask = (char) (0b10000000 >> j);

    return (activeNodesArray[i] & mask) != 0;
}


// Função que lê um arquivo em que cada linha é um numero e ativa
// os nós cujos IDs são iguais as linhas do arquivo.
void activateFromFile(const Graph *graph, const char *filename) {
    FILE *file = fopen(filename, "r");
    uint64_t id;
    while (1 == fscanf(file, "%llu", &id)) {
        setNodeState(graph->active_nodes, id, 1);
        const Node* node = graph->nodes[id];
        for (uint64_t i = 0; i < node->n_neighbors; i++) {
            node->neighbors[i]->n_active_neighbors++;
        }
    }
    fclose(file);
}



void activateFromIDArray(const Graph *graph, const uint64_t *IDs, const uint64_t n_ids) {
    for (uint64_t i = 0; i < n_ids; i++) {
        setNodeState(graph->active_nodes, IDs[i], 1);
        const Node *node = graph->nodes[IDs[i]];
        for (uint64_t j = 0; j < node->n_neighbors; j++) {
            node->neighbors[j]->n_active_neighbors++;
        }
    }
}

void deactivateAll(const Graph *graph) {
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        graph->nodes[i]->n_active_neighbors = 0;
    }
   memset(graph->active_nodes, 0, (graph->n_nodes / 8) + 1);
}



void freeGraph(Graph *graph) {
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


    if (graph->edges) {
        for (uint64_t i = 0; i < graph->n_edges; i++) {
            if (graph->edges[i])
                free(graph->edges[i]);
        }
        free(graph->edges);
    }


    if (graph->active_nodes)
        free(graph->active_nodes);

    free(graph);
}



void saveSolution(Graph *graph, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao tentar abrir o arquivo %s", filename);
        exit(EXIT_FAILURE);
    }


    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        Node* node = graph->nodes[i];
        if (getNodeState(graph->active_nodes, node->ID)) {
            printf("%ld", node->n_active_neighbors);
        }
    }
}

uint64_t countActiveNodes(const Graph* graph) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        if (getNodeState(graph->active_nodes, i)) { //
            count++;
        }
    }
    return count;
}