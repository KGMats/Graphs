#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/Algorithms.h"
#include "../include/Graph.h"
#include "../Solutions.h"
#include "../Heuristics/probabilistic/farthestFromActivation_r.h"

int main(const int argc, char **argv) {
    // Semente aleatória baseada no tempo
    srand(time(NULL));

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <caminho_grafo> <heuristica> [modo]\n", argv[0]);
        fprintf(stderr, "Heurísticas: maxdegree, bridge, farthest, random\n");
        fprintf(stderr, "Modos: simple (padrão), hillclimb, grasp\n");
        return 1;
    }

    const char *filename = argv[1];
    const char *heuristicName = argv[2];
    const char *modeStr = (argc > 3) ? argv[3] : "simple";

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
        int seed = rand(); // Gera uma seed única para esta execução

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