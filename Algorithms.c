#include <stdlib.h>
#include <stdio.h>

#include "Graph.h"
#include "Solutions.h"
#include "Stack.h"
#include "Algorithms.h"

#include <time.h>
#include <sys/types.h>

#include "Utils.h"
#include <sys/time.h>

// Funcao utilizada para verificar se o número de vizinhos ativos
// é suficiente para ativar o nó passado como argumento
bool activationFunction(const Node *node)
{
    if (node->n_active_neighbors == 0)
    {
        return false;
    }
    const long double n_active_neighbors = (long double) node->n_active_neighbors;
    const long double n_neighbors = (long double) node->n_neighbors;
    return (n_active_neighbors / n_neighbors) >= 0.5;
}



// Função que percorre o grafo e verifica quais nós satisfazem a função
// de ativação. Itera até que não hajam mais nós que possam ser ativados.
PropagationResult propagate(const Graph *graph) {
    uint64_t *activated_nodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t new_count = 0;

    for (int i = 0; i < graph->n_nodes; i++) {
        const Node* n1 = graph->nodes[i];
        if (getNodeState(graph->active_nodes, i)) {
            // Se o nó já está ativado, não precisamos verificar se ele precisa ser ativado.
            continue;
        }

        if (activationFunction(n1)) {
            setNodeState(graph->active_nodes, i, 1);
            activated_nodes[new_count++] = i;

            for (uint64_t j = 0; j < n1->n_neighbors; j++) {
                n1->neighbors[j]->n_active_neighbors++;
            }
        }
    }

    PropagationResult result = {new_count, activated_nodes};
    return result;
}



// Função que verifica se um dado conjunto de nós (activeNodeIDs)
// é capaz de ativar inteiramente o grafo
bool runTest(const Graph* graph, const uint64_t *activeNodeIDs, const uint64_t n_ids) {
    PropagationResult result = partialPropagate(graph, n_ids, activeNodeIDs);
    free(result.activated_nodes);

    uint64_t counter = countActiveNodes(graph);

    return counter == graph->n_nodes;
}



// Função similar a propagate, mas ao invés de verificar o grafo inteiramente
// em busca dos nós para ativar, ativa apenas os nós dados por changed_nodes
// e iterativamente os nós que serão ativados por propagação destes.
PropagationResult partialPropagate(const Graph *graph, const uint64_t n_changed, const uint64_t *changed_nodes) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }
    char *inQueue = calloc((graph->n_nodes / 8) + 1, sizeof(char));

    uint64_t *queue = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t front = 0;
    uint64_t back = 0;

    for (uint64_t i = 0; i < n_changed; i++) {
        if (!getNodeState(inQueue, changed_nodes[i])) {
            queue[back++] = changed_nodes[i];
            setNodeState(graph->active_nodes, changed_nodes[i], 1);
            setNodeState(inQueue, changed_nodes[i], 1);
        }
    }

    uint64_t *newly_activated = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t new_count = 0;

    while (front < back) {
        const uint64_t nodeID = queue[front++];
        const Node *node = graph->nodes[nodeID];

        for (uint64_t i = 0; i < node->n_neighbors; i++) {
            Node *neighbor = node->neighbors[i];

            neighbor->n_active_neighbors++;
            if (getNodeState(graph->active_nodes, neighbor->ID)) continue;


            if (activationFunction(neighbor)) {
                setNodeState(graph->active_nodes, neighbor->ID, 1);
                if (!getNodeState(inQueue, neighbor->ID)) {
                    queue[back++] = neighbor->ID;
                    setNodeState(inQueue, neighbor->ID, 1);
                }
                newly_activated[new_count++] = neighbor->ID;
            }
        }
    }

    free(queue);
    free(inQueue);

    PropagationResult result = {new_count, newly_activated};
    return result;
}


// Função similar ao partialPropagate, mas aqui ao invés de ativarmos os nós
// dados por changed_nodes, desativamos eles e possivelmente os nós vizinhos
// de forma iterativa.
PropagationResult partialReversePropagate(const Graph *graph, const uint64_t n_changed, const uint64_t *changed_nodes) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }

    uint64_t *queue = malloc(sizeof(uint64_t) * graph->n_nodes);

    char *inQueue = calloc((graph->n_nodes / 8) + 1, sizeof(char));

    uint64_t front = 0;
    uint64_t back = 0;

    for (uint64_t i = 0; i < n_changed; i++) {
        queue[back++] = changed_nodes[i];
        setNodeState(graph->active_nodes, changed_nodes[i], 0);
        setNodeState(inQueue, changed_nodes[i], 1);
    }

    uint64_t *deactivated_nodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t new_count = 0;
    while (front < back) {
        const uint64_t nodeID = queue[front++];
        const Node *node = graph->nodes[nodeID];

        for (uint64_t i = 0; i < node->n_neighbors; i++) {
            Node *neighbor = node->neighbors[i];
            neighbor->n_active_neighbors--;

            if (!getNodeState(graph->active_nodes, neighbor->ID)) continue;

            if (!activationFunction(neighbor)) {
                setNodeState(graph->active_nodes, neighbor->ID, 0);
                if (!getNodeState(inQueue, neighbor->ID)) {
                    queue[back++] = neighbor->ID;
                    setNodeState(inQueue, neighbor->ID, 1);
                }
                deactivated_nodes[new_count++] = neighbor->ID;
            }
        }
    }
    free(inQueue);
    free(queue);

    PropagationResult result = {new_count, deactivated_nodes};
    return result;
}


// TODO: Fazer essa função funcionar
PropagationResult partialReversePropagateBlackListed(const Graph *graph, const uint64_t n_changed,
                                                     const uint64_t *changed_nodes, const uint64_t *black_list,
                                                     const uint64_t black_list_size) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }

    uint64_t *queue = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t front = 0;
    uint64_t back = 0;

    for (uint64_t i = 0; i < n_changed; i++) {
        queue[back++] = changed_nodes[i];
        setNodeState(graph->active_nodes, changed_nodes[i], 0);
    }

    uint64_t *deactivated_nodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t new_count = 0;

    while (front < back) {
        const uint64_t nodeID = queue[front++];
        const Node *node = graph->nodes[nodeID];

        for (uint64_t i = 0; i < node->n_neighbors; i++) {
            Node *neighbor = node->neighbors[i];

            neighbor->n_active_neighbors--;

            bool isBlacklisted = false;

            for (uint64_t j = 0; j < black_list_size; j++) {
                if (neighbor->ID == black_list[j]) {
                    isBlacklisted = true;
                    break;
                }
            }

            if (!isBlacklisted) {

                if (!getNodeState(graph->active_nodes, neighbor->ID)) continue;

                if (!activationFunction(neighbor)) {
                    setNodeState(graph->active_nodes, neighbor->ID, 0);
                    queue[back++] = neighbor->ID;

                    deactivated_nodes[new_count++] = neighbor->ID;
                }
            }
        }
    }

    free(queue);
    PropagationResult result = {new_count, deactivated_nodes};
    return result;
}



void testHeuristics(const Graph* graph, bool heuristicFunction(const Graph*, uint64_t*, uint64_t)) {
    uint64_t low = 1;
    const uint64_t n_nodes = graph->n_nodes;
    uint64_t high = n_nodes;
    uint64_t best = 0;
    uint64_t *bestSolution = malloc(sizeof(uint64_t) * n_nodes);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    while (low <= high) {
        const uint64_t mid = (low + high) / 2;
        deactivateAll(graph);

        if (heuristicFunction(graph, bestSolution, mid)) {
            best = mid;
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }


    timespec_get(&end, TIME_UTC);

    int64_t delta_ns = end.tv_nsec - start.tv_nsec;
    int64_t delta_s = end.tv_sec - start.tv_sec;

    if (delta_ns < 0) {
        delta_ns += 1000000000;
        delta_s -= 1;
    }

    const uint64_t delta_hours = (delta_s / 3600);
    const uint64_t delta_minutes = (delta_s % 3600) / 60;
    const uint64_t delta_seconds = delta_s % 60;
    const uint64_t delta_milliseconds = (delta_ns / 1000000);

    if (best != 0) {
        printf("%llu/%llu Nos foram necessarios. Isso e %.5lf%% do total\n", best, n_nodes, 100. * ((double) best / (double) n_nodes));

        printf("A execucao demorou %llu horas %llu minutos %llu segundos e %llu milisegundos", delta_hours, delta_minutes, delta_seconds, delta_milliseconds);
        return;


    }

    printf("Nao existe solucao\n");
}


// Função que utiliza de uma heuristica para encontrar uma solução e, em seguida
// Utiliza uma função de busca local para tentar encontrar um vizinho melhor
// Que a solução previamente calculada pela heuristica.
void testLocalSearch(const Graph *graph, bool heuristicFunction(const Graph*, uint64_t*, uint64_t), uint64_t localSearchFunction(const Graph*, uint64_t*, uint64_t)) {
    // Tentando encontrar uma solução inicial com a heuristica
    uint64_t low = 1;
    const uint64_t n_nodes = graph->n_nodes;
    uint64_t high = n_nodes;
    uint64_t best = 0;
    uint64_t *bestSolution = malloc(sizeof(uint64_t) * n_nodes);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    while (low <= high) {
        const uint64_t mid = (low + high) / 2;
        deactivateAll(graph);

        if (heuristicFunction(graph, bestSolution, mid)) {
            best = mid;
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }

    if (best == 0) {
        printf("A heuristica não encontrou solução, portanto não há como fazer a busca local");
        free(bestSolution);
        return;
    }

    const uint64_t bestLocal = localSearchFunction(graph, bestSolution, best);

    deactivateAll(graph);
    PropagationResult p = partialPropagate(graph, bestLocal, bestSolution);
    free(p.activated_nodes);

    if (countActiveNodes(graph) != graph->n_nodes) {
        printf("A solução da busca local não é válida!");
        return;
    }

    timespec_get(&end, TIME_UTC);
    if (best != bestLocal)
    {
        int64_t delta_ns = end.tv_nsec - start.tv_nsec;
        int64_t delta_s = end.tv_sec - start.tv_sec;

        if (delta_ns < 0) {
            delta_ns += 1000000000;
            delta_s -= 1;
        }

        const uint64_t delta_hours = (delta_s / 3600);
        const uint64_t delta_minutes = (delta_s % 3600) / 60;
        const uint64_t delta_seconds = delta_s % 60;
        const uint64_t delta_milliseconds = (delta_ns / 1000000);

        printf("--------------------------------------------------\n");
        printf("Heuristica: %lu Nos ativos (%0.2f%% do total)\n", best, 100 *(float) best / graph->n_nodes);
        printf("Busca Local: %lu Nos ativos (%0.2f%% do total)\n", bestLocal, 100 * (float) bestLocal / graph->n_nodes);
        printf("Diferenca de %0.2f%% de redução\n", ((float) (best - bestLocal) / best) * 100);
        printf("Solucao: \n");
        for (int i = 0; i < bestLocal; i++) {
            printf("%llu, ", bestSolution[i]);
        }
        printf("\n");
        printf("A execucao demorou %llu horas %llu minutos %llu segundos e %llu milisegundos", delta_hours,
               delta_minutes, delta_seconds, delta_milliseconds);

        printf("--------------------------------------------------\n");
        free(bestSolution);
        return;
    }
    free(bestSolution);
    printf("A busca local não conseguiu melhorar o resultado da heuristica\n");
    printf("%lu Nos ativos (%0.2f%% do total)\n", best, 100 *(float) best / graph->n_nodes);
}

