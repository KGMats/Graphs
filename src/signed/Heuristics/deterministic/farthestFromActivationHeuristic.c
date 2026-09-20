#include "signed/signedGraph.h"
#include "IndexedHeap.h"
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/farthestFromActivationHeuristic.h"

bool farthestFromActivationHeuristicSigned(const SignedGraph *graph, uint64_t *result, const uint64_t initialActiveNodes, uint64_t steps) {
    if (initialActiveNodes < 1) {
        return false;
    }

    // Creating a heap to use as a priority-queue to select the nodes based on how distant
    // The node is from being active.

    IndexedHeap *heap = createIndexedHeap(graph->n_nodes);
    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        SignedNode *node = graph->nodes[i];
        HeapElement element = {
            .ID = i,
            .value = node->positive_edges - (node->degree - node->positive_edges)
        };
        insertIndexedHeap(heap, element);
    }

    uint64_t selectedCount = 0;
    uint64_t iterations = initialActiveNodes;

    while (iterations >= 1 && !isEmpty(heap)) {
        // Getting the top of the heap
        HeapElement maxElement = extractMax(heap);
        uint64_t ID = maxElement.ID;

        bool wasAlreadyActive = getSignedNodeState(graph->active_nodes, ID);


        // adding the node ID to the solution array
        result[selectedCount++] = ID;


        // Propagating the node influence
        PropagationResult p = {0, NULL};
        if (!wasAlreadyActive) {
            p = signedPartialPropagate(graph, 1, &ID, steps);
        }

        if (!wasAlreadyActive) {
            const SignedNode *selectedNode = graph->nodes[ID];
            for (uint64_t i = 0; i < selectedNode->degree; i++) {
                SignedNode *neighbor = get_clean_ptr(selectedNode->neighbors[i]);
                uint64_t neighborID = neighbor->ID;

                if (!getSignedNodeState(graph->active_nodes, neighborID)) {
                    double newValue = neighbor->positive_edges - (neighbor->degree - neighbor->positive_edges);
                    update(heap, neighborID, newValue);
                }
            }
        }

        // Updating the priorities for the activated nodes and their neighbors
        for (uint64_t i = 0; i < p.count; i++) {
            uint64_t activatedNodeID = p.activated_nodes[i];
            const SignedNode *activatedNode = graph->nodes[activatedNodeID];
            removeIndexedHeap(heap, activatedNodeID);

            for (uint64_t j = 0; j < activatedNode->degree; j++) {
                SignedNode *neighbor = get_clean_ptr(activatedNode->neighbors[j]);
                uint64_t neighborID = neighbor->ID;

                if (!getSignedNodeState(graph->active_nodes, neighborID)) {
                    double newValue = neighbor->positive_edges - (neighbor->degree - neighbor->positive_edges);
                    update(heap, neighborID, newValue);
                }
            }
        }

        free(p.activated_nodes);
        iterations--;
    }
    freeIndexedHeap(heap);
    return signedRunTest(graph, result, 0, steps);
}