#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "unsigned/algorithms.h"
#include "unsigned/graph.h"
#include "unsigned/Solutions.h"

#include "signed/algorithms.h"
#include "signed/signedGraph.h"
#include "signed/Solutions.h"

#define RAND_SEED 1

int main(const int argc, char **argv) {
    // Semente aleatória baseada no tempo
    srand(time(NULL));

    bool isSigned = false;
    uint64_t steps = 10;
    const char *positional[3] = {NULL, NULL, NULL};
    int posCount = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--signed") == 0) {
            isSigned = true;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--steps") == 0) {
            if (i + 1 < argc) {
                steps = strtoull(argv[++i], NULL, 10);
            } else {
                fprintf(stderr, "Erro: Opção '%s' requer um valor numérico.\n", argv[i]);
                return 1;
            }
        } else if (strncmp(argv[i], "--steps=", 8) == 0) {
            steps = strtoull(argv[i] + 8, NULL, 10);
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Aviso: Opção desconhecida '%s'\n", argv[i]);
        } else {
            if (posCount < 3) {
                positional[posCount++] = argv[i];
            }
        }
    }

    if (posCount < 2) {
        fprintf(stderr, "Uso: %s <caminho_grafo> <heuristica> [modo] [--signed|-s] [-t|--steps <passos>]\n", argv[0]);
        fprintf(stderr, "Heurísticas: maxdegree, bridge, farthest, random\n");
        fprintf(stderr, "Modos: simple (padrão), hillclimb, grasp\n");
        fprintf(stderr, "Opções:\n");
        fprintf(stderr, "  -s, --signed         Indica que o grafo é sinalizado\n");
        fprintf(stderr, "  -t, --steps <num>    Passos de propagação para grafos sinalizados (padrão: 10)\n");
        return 1;
    }

    const char *filename = positional[0];
    const char *heuristicName = positional[1];
    const char *modeStr = (posCount > 2) ? positional[2] : "simple";

    if (isSigned) {
        SignedGraph *graph = createSignedGraphFromFilename(filename);
        if (!graph) return 1;

        // Ponteiros para funções de heurísticas sinalizadas
        bool (*signedHeuristicFunc)(const SignedGraph*, uint64_t*, uint64_t, uint64_t) = NULL;

        if (strcmp(heuristicName, "maxdegree") == 0) {
            signedHeuristicFunc = maximumDegreeHeuristicSigned;
        } else if (strcmp(heuristicName, "bridge") == 0) {
            signedHeuristicFunc = bridgeHeuristicSigned;
        } else if (strcmp(heuristicName, "farthest") == 0) {
            signedHeuristicFunc = farthestFromActivationHeuristicSigned;
        } else if (strcmp(heuristicName, "random") == 0) {
            signedHeuristicFunc = fullRandomHeuristicSigned;
        } else {
            fprintf(stderr, "Erro: Heuristica '%s' desconhecida.\n", heuristicName);
            freeSignedGraph(graph);
            return 1;
        }

        printf("=== Executando Instancia Sinalizada: %s | Algoritmo: %s | Modo: %s | Passos (t): %lu ===\n",
               filename, heuristicName, modeStr, (unsigned long)steps);

        // --- SELETOR DE MODO SINALIZADO ---
        if (strcmp(modeStr, "simple") == 0) {
            signedTestHeuristics(graph, signedHeuristicFunc, steps);
        } else if (strcmp(modeStr, "hillclimb") == 0 || strcmp(modeStr, "grasp") == 0) {
            printf("Aviso: O modo '%s' ainda não foi adaptado para grafos sinalizados.\n", modeStr);
        } else {
            fprintf(stderr, "Modo '%s' invalido.\n", modeStr);
        }

        freeSignedGraph(graph);
        return 0;
    }

    // Fluxo para grafo não-sinalizado (unsigned)
    Graph *graph = createGraphFromFilename(filename);
    if (!graph) return 1;

    // Ponteiros para funções
    bool (*heuristicFunc)(const Graph*, uint64_t*, uint64_t) = NULL;

    // Mapeamento de strings para funções
    if (strcmp(heuristicName, "maxdegree") == 0) {
        heuristicFunc = maximumDegreeHeuristic;
    } else if (strcmp(heuristicName, "bridge") == 0) {
        heuristicFunc = bridgeHeuristic;
    } else if (strcmp(heuristicName, "farthest") == 0) {
        heuristicFunc = farthestFromActivationHeuristic;
    } else if (strcmp(heuristicName, "random") == 0) {
        heuristicFunc = fullRandomHeuristic;
    } else {
        fprintf(stderr, "Erro: Heuristica '%s' desconhecida.\n", heuristicName);
        freeGraph(graph);
        return 1;
    }

    printf("=== Executando Instancia: %s | Algoritmo: %s | Modo: %s ===\n", filename, heuristicName, modeStr);

    // --- SELETOR DE MODO ---
    if (strcmp(modeStr, "simple") == 0) {
        testHeuristics(graph, heuristicFunc);
    }
    else if (strcmp(modeStr, "hillclimb") == 0) {
        testLocalSearch(graph, heuristicFunc, hillClimbSimple);
    }
    else if (strcmp(modeStr, "grasp") == 0) {
        // Configuração do GRASP
        int max_iter = 50;
        int seed = RAND_SEED;

        // Verifica se a heurística escolhida tem versão probabilística implementada
        if (strcmp(heuristicName, "farthest") == 0) {
            printf("Iniciando GRASP com Probabilistic Farthest (%d iterações)...\n", max_iter);

            uint64_t sol = GRASP(graph, max_iter, seed,
                                 probabilisticFarthestHeuristic, // Versão Randomizada
                                 hillClimbSimple);               // Busca Local

            printf("Melhor solucao GRASP encontrada: %lu\n", sol);
        } else {
            printf("Aviso: A heurística '%s' não possui versão probabilística para GRASP implementada neste teste.\n", heuristicName);
        }
    }
    else {
        fprintf(stderr, "Modo '%s' invalido.\n", modeStr);
    }

    freeGraph(graph);
    return 0;
}