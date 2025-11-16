#include "farthestFromActivation_r.h"
#include "Algorithms.h"
#include <stdlib.h>
#include <float.h>
#include <stdio.h>

// 0.0 = 100% Random, 1.0 = 100% Greedy
#define GRASP_ALPHA 0.8

static double calculateMissing(const Node *node) {
    return (node->n_neighbors / 2.0) - (double) node->n_active_neighbors;
}

bool probabilisticFarthestHeuristic(const Graph *graph, uint64_t *result, const uint64_t initialActiveNodes) {
    if (initialActiveNodes < 1) {
        return false;
    }

    // Allocating RCL
    uint64_t *RCL = malloc(sizeof(uint64_t) * graph->n_nodes);
    if (!RCL) {
        perror("Failed to allocate RCL for GRASP");
        return false;
    }

    uint64_t selectedCount = 0;
    uint64_t iterations = initialActiveNodes;

    while (iterations >= 1) {
        double max_score = -DBL_MAX;
        uint64_t best_node_id = 0;
        bool has_candidates = false;

        for (uint64_t i = 0; i < graph->n_nodes; i++) {
            // Skipping already active nodes
            if (getNodeState(graph->active_nodes, i)) {
                continue;
            }

            double score = calculateMissing(graph->nodes[i]);

            // Capturing the first valid or best score
            if (!has_candidates || score > max_score) {
                max_score = score;
                best_node_id = i; // Storing the best node (fallback)
                has_candidates = true;
            }
        }

        // Breaking if no non-active nodes are left
        if (!has_candidates) {
            break;
        }

        // Calculating the score limit for the RCL
        double limite;
        if (max_score <= 0) {
            // Being purely greedy if max_score is negative
            limite = max_score;
        } else {
            limite = max_score * GRASP_ALPHA;
        }

        // Building the RCL
        uint64_t rcl_count = 0; // Resetting RCL counter
        for (uint64_t i = 0; i < graph->n_nodes; i++) {
            // Skipping already active nodes
            if (getNodeState(graph->active_nodes, i)) {
                continue;
            }

            double score = calculateMissing(graph->nodes[i]);

            // Adding node to RCL if its score is >= limit
            if (score >= limite) {
                RCL[rcl_count++] = i;
            }
        }

        uint64_t selectedID;
        if (rcl_count == 0) {
            // Using the best node if RCL is empty (fallback)
            selectedID = best_node_id;
        } else {
            // Selecting a random node from the RCL
            uint64_t randomIndex = rand() % rcl_count;
            selectedID = RCL[randomIndex];
        }

        // Adding the selected node to the solution
        result[selectedCount++] = selectedID;

        // Propagating the selected node's activation
        PropagationResult p = partialPropagate(graph, 1, &selectedID);
        free(p.activated_nodes);

        iterations--;
    }

    // Freeing the RCL memory
    free(RCL);

    // Testing if the solution is valid
    return runTest(graph, result, 0);
}
