//
// Created by Elon Pereira Neto on 24/10/25.
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "Utils.h"
#include "Algorithms.h"
#include "../../../include/Heuristics/deterministic/hillClimbLocalSearch.h"


// Função de busca local utilizando a estratégia "tira dois, põe um"
uint64_t hillClimbSimple(const Graph *graph, uint64_t *bestSolution, uint64_t nActiveNodes) {
    const uint64_t MAX_ITERATIONS = 500;
    uint64_t *toRemove = malloc(sizeof(uint64_t) * 2);
    uint64_t *toActivate = malloc(sizeof(uint64_t) * 1);
    uint64_t *inactiveList = malloc(sizeof(uint64_t) * graph->n_nodes);


    bool melhorou = true;

    uint32_t iteracao = 0;
    while (melhorou) {
        iteracao++;
        melhorou = false;
        //const uint64_t MAX_ITERATIONS = (graph->n_nodes - nActiveNodes + 2) * nActiveNodes * (nActiveNodes - 1) / 2;

        // Nao tem como "Tirar dois e colocar um" se não tiverem pelo menos dois ativos.
        if (nActiveNodes < 2) break;

        printf("Iteração (%u): Solucao atual: %llu\n", iteracao, nActiveNodes);

        if (inactiveList == NULL) {
            perror("Erro na alocacao de memoria nas listas do Hill Climb");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < MAX_ITERATIONS; ++i) {
            uint64_t active_index = nActiveNodes;
            uint64_t inactive_index = 0;

            toRemove[0] = bestSolution[rand() % nActiveNodes];
            do {
                toRemove[1] = bestSolution[rand() % nActiveNodes];
            } while (toRemove[1] == toRemove[0]);

            removeIDFromList(bestSolution, toRemove[0], &active_index);
            removeIDFromList(bestSolution, toRemove[1], &active_index);
            deactivateAll(graph);
            PropagationResult p = partialPropagate(graph, active_index, bestSolution);
            free(p.activated_nodes);

            for (uint64_t j = 0; j < graph->n_nodes; j++) {
                if (!getNodeState(graph->active_nodes, j)) inactiveList[inactive_index++] = j;
            }

            if (inactive_index != 0) {
                *toActivate = inactiveList[rand() % inactive_index];

                // Testando se é uma solução válida
                PropagationResult p2 = partialPropagate(graph, 1, toActivate);
                free(p2.activated_nodes);
                const uint64_t totalAtivos = countActiveNodes(graph);

                if (totalAtivos == graph->n_nodes) {
                    nActiveNodes -= 1;
                    bestSolution[active_index] = *toActivate;
                    melhorou = true;
                    break;
                }

                // Se não for, restaura os estados anteriores
                bestSolution[active_index] = toRemove[0];
                bestSolution[active_index + 1] = toRemove[1];
            } else {
                nActiveNodes -= 2;
                melhorou = true;
                break;
            }
        }
    }

    free(toActivate);
    free(toRemove);
    free(inactiveList);


    return nActiveNodes;
}