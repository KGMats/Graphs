#include <math.h>
#include <stdint.h>
#include "Utils.h"
#include "Algorithms.h"
#include <string.h>
#include "farthestFromActivationHeuristic.h"

bool farthestFromActivationHeuristic(const Graph *graph, uint64_t *result, uint64_t initialActiveNodes) {
    if (initialActiveNodes < 1)
    {
        return false;
    }

    uint64_t ID = 0;
    double max = 0;

    for (uint64_t i = 0; i < graph->n_nodes;i++)
    {
        Node *node = graph->nodes[i];
        double missing = (node->n_neighbors / 2.0) - node->n_active_neighbors;
        if (missing > max)
        {
            ID = i;
            max = missing;
        }
    }


    PropagationResult propagationResult = partialPropagate(graph, 1, &ID);
    free(propagationResult.activated_nodes);

    tuple *list = malloc(sizeof(tuple) * graph->n_nodes);
    for (uint64_t i = 0; i < graph->n_nodes;i++)
    {
        Node *node = graph->nodes[i];
        list[i].ID = i;
        list[i].degree = (node->n_neighbors / 2.0) - node->n_active_neighbors;
    }

    initialActiveNodes--;


    while (initialActiveNodes >= 1)
    {
        max = 0;
        for (uint64_t i = 0; i < graph->n_nodes;i++)
        {
            Node *node = graph->nodes[i];
            double missing = (node->n_neighbors / 2.0) - node->n_active_neighbors;
            if (missing > max)
            {
                ID = i;
                max = missing;
            }
        }

        PropagationResult p = partialPropagate(graph, 1, &ID);


        for (int i = 0; i < p.count; i++) {
            for (int j = 0; j < graph->nodes[p.activated_nodes[i]]->n_neighbors; j++) {
                Node *node = graph->nodes[p.activated_nodes[i]]->neighbors[j];
                list[node->ID].degree = (node->n_neighbors / 2.0) - node->n_active_neighbors;
            }
        }
        free(p.activated_nodes);

        initialActiveNodes--;
    }

    free(list);
    return runTest(graph, result, initialActiveNodes);

}
