#include "signed/signedGraph.h"
#include "IndexedHeap.h"
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/farthestFromActivationHeuristic.h"



static double calculateMissing(SignedNode *node) {
    return (double) threshold(node) - (double) node->n_active_neighbors;
}

uint64_t farthestFromActivationHeuristicSigned(const SignedGraph *graph, uint64_t *result, const uint64_t initialActiveNodes, uint64_t steps) {

    if (initialActiveNodes < 1) {
        return 0;
    }

    // Creating a heap to use as a priority-queue to select the nodes based on how distant
    // the node is from being active.
    IndexedHeap *heap = createIndexedHeap(graph->n_nodes);
    for (uint64_t nodeIndex = 0; nodeIndex < graph->n_nodes; nodeIndex++) {
        SignedNode *node = graph->nodes[nodeIndex];
        HeapElement element = {
            .ID = nodeIndex,
            .value = calculateMissing(node)
        };
        insertIndexedHeap(heap, element);
    }

    uint64_t selectedCount = 0;
    uint64_t iterations = initialActiveNodes;

    while (iterations >= 1 && !isEmpty(heap)) {
        // Getting the top of the heap
        HeapElement maxElement = extractMax(heap);
        uint64_t selectedNodeID = maxElement.ID;

        bool wasAlreadyActive = (getSignedNodeState(graph->active_nodes, selectedNodeID) == 1);

        // Adding the node ID to the solution array
        result[selectedCount++] = selectedNodeID;

        // Propagating the node influence
        PropagationResult propagationResult = {0, NULL};
        if (!wasAlreadyActive) {
            int s = (steps - iterations) < steps / 2 ? steps - iterations : steps / 2; 
            propagationResult = signedPartialPropagate(graph, 1, &selectedNodeID, s);
        }

        // Updating priorities for the activated nodes and their neighbors
        for (uint64_t i = 0; i < propagationResult.count; i++) {
            uint64_t activatedNodeID = propagationResult.activated_nodes[i];
            SignedNode *activatedNode = graph->nodes[activatedNodeID];
            update(heap, activatedNodeID, calculateMissing(activatedNode));

            for (uint64_t j = 0; j < activatedNode->degree; j++) {
                SignedNode *neighborNode = get_clean_ptr(activatedNode->neighbors[j]);
                uint64_t neighborID = neighborNode->ID;
                update(heap, neighborID, calculateMissing(neighborNode));
            }
        }

        free(propagationResult.activated_nodes);
        iterations--;
    }

    freeIndexedHeap(heap);
    return selectedCount;
}
