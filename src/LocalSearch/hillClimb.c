//
// created by elon pereira neto on 24/10/25.
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "utils.h"
#include "Algorithms.h"
#include "hillClimb.h"

#include "Utils.h"


// função de busca local utilizando a estratégia "tira dois, põe um"
uint64_t hillClimbSimple(const Graph *graph, uint64_t *bestsolution, uint64_t nactivenodes) {
    const uint64_t max_iterations = 500;
    uint64_t *toremove = malloc(sizeof(uint64_t) * 2);
    uint64_t *toactivate = malloc(sizeof(uint64_t) * 1);
    uint64_t *inactivelist = malloc(sizeof(uint64_t) * graph->n_nodes);


    bool melhorou = true;

    uint32_t iteracao = 0;
    while (melhorou) {
        iteracao++;
        melhorou = false;
        //const uint64_t max_iterations = (graph->n_nodes - nactivenodes + 2) * nactivenodes * (nactivenodes - 1) / 2;

        // nao tem como "tirar dois e colocar um" se não tiverem pelo menos dois ativos.
        if (nactivenodes < 2) break;

        printf("iteração (%u): solucao atual: %llu\n", iteracao, nactivenodes);

        if (inactivelist == NULL) {
            perror("erro na alocacao de memoria nas listas do hill climb");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < max_iterations; ++i) {
            uint64_t active_index = nactivenodes;
            uint64_t inactive_index = 0;

            toremove[0] = bestsolution[rand() % nactivenodes];
            do {
                toremove[1] = bestsolution[rand() % nactivenodes];
            } while (toremove[1] == toremove[0]);

            removeIDFromList(bestsolution, toremove[0], &active_index);
            removeIDFromList(bestsolution, toremove[1], &active_index);
            deactivateAll(graph);
            PropagationResult p = partialPropagate(graph, active_index, bestsolution);
            free(p.activated_nodes);

            for (uint64_t j = 0; j < graph->n_nodes; j++) {
                if (!getNodeState(graph->active_nodes, j)) inactivelist[inactive_index++] = j;
            }

            if (inactive_index != 0) {
                *toactivate = inactivelist[rand() % inactive_index];

                // testando se é uma solução válida
                PropagationResult p2 = partialPropagate(graph, 1, toactivate);
                free(p2.activated_nodes);
                const uint64_t totalativos = countActiveNodes(graph);

                if (totalativos == graph->n_nodes) {
                    nactivenodes -= 1;
                    bestsolution[active_index] = *toactivate;
                    melhorou = true;
                    break;
                }

                // se não for, restaura os estados anteriores
                bestsolution[active_index] = toremove[0];
                bestsolution[active_index + 1] = toremove[1];
            } else {
                nactivenodes -= 2;
                melhorou = true;
                break;
            }
        }
    }

    free(toactivate);
    free(toremove);
    free(inactivelist);


    return nactivenodes;
}