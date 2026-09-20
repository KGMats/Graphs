#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "signed/SignedGraph.h"
#include "signed/Algorithms.h"
#include "Utils.h"

// Funcao utilizada para verificar se o número de vizinhos ativos
// é suficiente para ativar o nó passado como argumento
bool activationFunction(SignedNode *node)
{
    if (node->n_active_neighbors == 0)
    {
        return false;
    }
    uint64_t th = threshold(node);
    int64_t abs_influence = node->n_active_neighbors;
    if (abs_influence < 0)
    {
        abs_influence = -abs_influence;
    }

    return abs_influence >= (int64_t) th;
}

// Função que percorre o grafo e propaga a influência de forma síncrona por até t passos.
PropagationResult propagate(const SignedGraph *graph, const uint64_t t) {
    uint64_t *activated_nodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t new_count = 0;

    uint64_t *changed_nodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    int *new_states = malloc(sizeof(int) * graph->n_nodes);
    int *old_states = malloc(sizeof(int) * graph->n_nodes);

    for (uint64_t step = 0; step < t; step++) {
        uint64_t n_changed = 0;

        for (uint64_t i = 0; i < graph->n_nodes; i++) {
            SignedNode *node = graph->nodes[i];
            int curr_s = getSignedNodeState(graph->active_nodes, i);

            int64_t I = node->n_active_neighbors;
            uint64_t th = threshold(node);
            int64_t abs_I = (I < 0) ? -I : I;

            int next_s;
            if (abs_I >= (int64_t) th) {
                next_s = (I > 0) ? 1 : -1;
            } else if (I == 0) {
                next_s = 0;
            } else {
                next_s = curr_s;
            }

            if (next_s != curr_s) {
                changed_nodes[n_changed] = i;
                old_states[n_changed] = curr_s;
                new_states[n_changed] = next_s;
                n_changed++;
            }
        }

        if (n_changed == 0) {
            break;
        }

        for (uint64_t k = 0; k < n_changed; k++) {
            uint64_t id = changed_nodes[k];
            int s_new = new_states[k];
            int s_old = old_states[k];
            int delta_s = s_new - s_old;

            setSignedNodeState(graph->active_nodes, id, s_new);

            const SignedNode *node = graph->nodes[id];
            for (uint64_t j = 0; j < node->degree; j++) {
                SignedNode *tagged_neighbor = node->neighbors[j];
                SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
                int edge_sign = get_edge_sign(tagged_neighbor);

                clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
            }
        }
    }

    free(changed_nodes);
    free(new_states);
    free(old_states);

    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        if (getSignedNodeState(graph->active_nodes, i) != 0) {
            activated_nodes[new_count++] = i;
        }
    }

    PropagationResult result = {new_count, activated_nodes};
    return result;
}

// Função similar a propagate, mas ativa inicialmente os nós dados por changed_nodes
// com estado +1 e propaga a influência de forma síncrona por até t passos.
PropagationResult partialPropagate(const SignedGraph *graph, const uint64_t n_changed,
                                   const uint64_t *changed_nodes, const uint64_t t) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }

    for (uint64_t i = 0; i < n_changed; i++) {
        uint64_t id = changed_nodes[i];
        int curr_s = getSignedNodeState(graph->active_nodes, id);
        if (curr_s == 1) continue;

        int delta_s = 1 - curr_s;
        setSignedNodeState(graph->active_nodes, id, 1);

        const SignedNode *node = graph->nodes[id];
        for (uint64_t j = 0; j < node->degree; j++) {
            SignedNode *tagged_neighbor = node->neighbors[j];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
        }
    }

    return propagate(graph, t);
}

// Função similar ao partialPropagate, mas aqui ao invés de ativarmos os nós
// dados por changed_nodes, desativamos eles (estado 0) e propagamos as mudanças por até t passos.
PropagationResult partialReversePropagate(const SignedGraph *graph, const uint64_t n_changed,
                                          const uint64_t *changed_nodes, const uint64_t t) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }

    for (uint64_t i = 0; i < n_changed; i++) {
        uint64_t id = changed_nodes[i];
        int curr_s = getSignedNodeState(graph->active_nodes, id);
        if (curr_s == 0) continue;

        int delta_s = 0 - curr_s;
        setSignedNodeState(graph->active_nodes, id, 0);

        const SignedNode *node = graph->nodes[id];
        for (uint64_t j = 0; j < node->degree; j++) {
            SignedNode *tagged_neighbor = node->neighbors[j];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
        }
    }

    return propagate(graph, t);
}

// Função que verifica se um dado conjunto de nós (activeNodeIDs)
// é capaz de ativar inteiramente o grafo em até t passos
bool runTest(const SignedGraph *graph, const uint64_t *activeNodeIDs, const uint64_t n_ids, const uint64_t t) {
    PropagationResult result = partialPropagate(graph, n_ids, activeNodeIDs, t);
    free(result.activated_nodes);

    uint64_t counter = countActiveSignedNodes(graph);

    return counter == graph->n_nodes;
}

void testHeuristics(const SignedGraph *graph,
                    bool heuristicFunction(const SignedGraph *, uint64_t *, uint64_t, uint64_t),
                    const uint64_t t) {
    uint64_t low = 1;
    const uint64_t n_nodes = graph->n_nodes;
    uint64_t high = n_nodes;
    uint64_t best = 0;
    uint64_t *bestSolution = malloc(sizeof(uint64_t) * n_nodes);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    while (low <= high) {
        const uint64_t mid = (low + high) / 2;
        deactivateAllSigned(graph);

        if (heuristicFunction(graph, bestSolution, mid, t)) {
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
        printf("%llu/%llu Nos foram necessarios. Isso e %.5lf%% do total\n", (unsigned long long)best, (unsigned long long)n_nodes, 100. * ((double) best / (double) n_nodes));

        printf("A execucao demorou %llu horas %llu minutos %llu segundos e %llu milisegundos\n", (unsigned long long)delta_hours, (unsigned long long)delta_minutes, (unsigned long long)delta_seconds, (unsigned long long)delta_milliseconds);
        free(bestSolution);
        return;
    }

    free(bestSolution);
    printf("Nao existe solucao\n");
}
