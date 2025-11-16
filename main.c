#include <stdio.h>
#include <time.h>

#include "Algorithms.h"
#include "fullRandomHeuristic.h"
#include "Solutions.h"
#include "Graph.h"


int main(const int argc, char **argv) {
    const char *filename = "../graphs/facebook_combined.txt";

    if (argc > 1) {
        filename = argv[1];
    }
    Graph *graph = createGraphFromFilename(filename);

    // testLocalSearch(graph, farthestFromActivationHeuristic, hillClimbSimple);
    testHeuristics(graph, farthestFromActivationHeuristic);
    if (countActiveNodes(graph) == graph->n_nodes) {
        printf("\n\nSolucao encontrada testada com sucesso\n");
    }
    // int sol = GRASP(graph, 10, time(NULL), fullRandomHeuristic, hillClimbSimple);
    // printf("%d\n", sol);

    freeGraph(graph);
    return 0;
}
