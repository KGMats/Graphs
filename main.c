#include <stdio.h>
#include <time.h>

#include "Algorithms.h"
#include "Heuristics/deterministic/fullRandomHeuristic.h"
#include "Solutions.h"
#include "Graph.h"


int main(const int argc, char **argv) {
    const char *filename = "../graphs/facebook_combined.txt";

    if (argc > 1) {
        filename = argv[1];
    }
    Graph *graph = createGraphFromFilename(filename);

    // testLocalSearch(graph, farthestFromActivationHeuristic, hillClimbSimple);
    // testHeuristics(graph, farthestFromActivationHeuristic);
    // if (countActiveNodes(graph) == graph->n_nodes) {
    //     printf("\n\nSolucao encontrada testada com sucesso\n");
    // }

    const int seed = 1;
    const uint64_t sol = GRASP(graph, 10, seed, probabilisticFarthestHeuristic, hillClimbSimple);
    printf("Solucao: %ld\n", sol);
    printf("Seed:%ld\n", seed);

    freeGraph(graph);
    return 0;
}
