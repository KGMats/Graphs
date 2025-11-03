#include <math.h>
#include <stdint.h>
#include "Utils.h"
#include "Algorithms.h"
#include <string.h>
#include "farthestFromActivationHeuristic.h"

bool farthestFromActivationHeuristic(const Graph *graph, uint64_t *result, uint64_t initialActiveNodes)
{

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


    partialPropagate(graph,1, &ID);

    tuple *list = malloc(sizeof(tuple) * graph->n_nodes);
    for (uint64_t i = 0; i < graph->n_nodes;i++)
    {
        Node *node = graph->nodes[i];
        list[i].ID = i;
        list[i].degree = (node->n_neighbors / 2.0) - node->n_active_neighbors;
    }

    initialActiveNodes--;

    char *previousState = malloc((graph->n_nodes / 8) + 1);
    char *hasBeenUpdated = malloc((graph->n_nodes / 8) + 1);


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

        memcpy(previousState, graph->active_nodes, (graph->n_nodes / 8) + 1);
        memset(hasBeenUpdated, 0, (graph->n_nodes / 8) + 1);

        partialPropagate(graph, 1, &ID);


        for (int i = 0; i < graph->n_nodes;i++)
        {
            if (getNodeState(graph->active_nodes, i) != getNodeState(previousState, i))
            {
                for (int j = 0; j < graph->nodes[i]->n_neighbors; j++)
                {
                    Node* node = graph->nodes[i]->neighbors[j];
                    if (!getNodeState(hasBeenUpdated, node->ID))
                    {
                        setNodeState(hasBeenUpdated, node->ID, 1);
                        list[node->ID].degree = (node->n_neighbors / 2.0) - node->n_active_neighbors;
                    }

                }
            }
        }

        initialActiveNodes--;
    }

    free(previousState);
    free(hasBeenUpdated);
    free(list);
    return runTest(graph, result, initialActiveNodes);

}
