#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "Graph.h"

uint64_t GRASP(Graph *graph, unsigned int max_iterations,
               const int seed, const bool randomGreedyFunction(const Graph *, uint64_t *result, const uint64_t),
               const uint64_t localSearchFunction(const Graph *graph, uint64_t *bestSolution, uint64_t nActiveNodes)) {
    srand(seed);
    float multiplier = 0.5f;

    uint64_t best_solution = UINT64_MAX; // 0 indica que não tem solucao
    uint64_t *result = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t *best_result = malloc(sizeof(uint64_t) * graph->n_nodes);

    while (max_iterations != 0) {
        max_iterations--;
        deactivateAll(graph);
        uint64_t solution_size = graph->n_nodes * multiplier;
        bool is_solution = randomGreedyFunction(graph, result, solution_size);
        if (!is_solution) {
            multiplier += 0.05f;
            solution_size = graph->n_nodes * multiplier;
            deactivateAll(graph);
            is_solution = randomGreedyFunction(graph, result, solution_size);
        }

        if (!is_solution) {
            continue;
        }

        // refina a solução construtiva antes de comparar com o melhor global
        solution_size = localSearchFunction(graph, result, solution_size);

        if (solution_size < best_solution) {
            best_solution = solution_size;
            memcpy(best_result, result, sizeof(uint64_t) * solution_size);
        }

        printf("%ld\n", best_solution);
    }

    free(result);
    free(best_result);
    return best_solution;
}