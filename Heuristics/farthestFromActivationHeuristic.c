#include <stdlib.h>
#include <stdio.h>
#include "farthestFromActivationHeuristic.h"
#include "Algorithms.h"
#include "IndexedHeap.h"

static double calculateMissing(const Node *node) {
    return (node->n_neighbors / 2.0) - (double)node->n_active_neighbors;
}

bool farthestFromActivationHeuristic(const Graph *graph, uint64_t *result, const uint64_t initialActiveNodes) {
    if (initialActiveNodes < 1) {
        return false;
    }

    // Creating a heap to use as a priority-queue to select the nodes based on how distant
    // The node is from being active.
    IndexedHeap *heap = createIndexedHeap(graph->n_nodes);
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        HeapElement element = {
            .ID = i,
            .value = calculateMissing(graph->nodes[i])
        };
        insertIndexedHeap(heap, element);
    }

    uint64_t selectedCount = 0;
    uint64_t iterations = initialActiveNodes;

    while (iterations >= 1 && !isEmpty(heap)) {
        // Getting the top of the heap
        HeapElement maxElement = extractMax(heap);
        uint64_t ID = maxElement.ID;
        
        bool wasAlreadyActive = getNodeState(graph->active_nodes, ID);


        // adding the node ID to the solution array
        result[selectedCount++] = ID;


        // Propagating the node influence
        PropagationResult p = {0, NULL};
        if (!wasAlreadyActive) {
            p = partialPropagate(graph, 1, &ID);
        }

        if (!wasAlreadyActive) {
            const Node *selectedNode = graph->nodes[ID];
            for (uint64_t i = 0; i < selectedNode->n_neighbors; i++) {
                Node *neighbor = selectedNode->neighbors[i];
                uint64_t neighborID = neighbor->ID;
                
                if (!getNodeState(graph->active_nodes, neighborID)) {
                    double newValue = calculateMissing(neighbor);
                    update(heap, neighborID, newValue); 
                }
            }
        }

        // Updating the priorities for the activated nodes and their neighbors
        for (uint64_t i = 0; i < p.count; i++) {
            uint64_t activatedNodeID = p.activated_nodes[i];
            const Node *activatedNode = graph->nodes[activatedNodeID];
            removeIndexedHeap(heap, activatedNodeID);

            for (uint64_t j = 0; j < activatedNode->n_neighbors; j++) {
                Node *neighbor = activatedNode->neighbors[j];
                uint64_t neighborID = neighbor->ID;
                
                if (!getNodeState(graph->active_nodes, neighborID)) {
                    double newValue = calculateMissing(neighbor);
                    update(heap, neighborID, newValue);
                }
            }
        }
        
        free(p.activated_nodes);
        iterations--;
    }
    freeIndexedHeap(heap);
    return runTest(graph, result, 0);
}
