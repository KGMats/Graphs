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
    uint64_t runs = 0;              // 0 = padrao da heuristica (5 para random, 1 para as demais)
    bool showTrace = false;
    uint64_t ks[32];
    uint64_t n_ks = 0;
    double fractions[16];
    uint64_t n_fractions = 0;
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
        } else if (strcmp(argv[i], "--runs") == 0 && i + 1 < argc) {
            runs = strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--trace") == 0) {
            showTrace = true;
        } else if (strcmp(argv[i], "--k") == 0 && i + 1 < argc) {
            // orcamentos absolutos separados por virgulas, ex.: 1,5,10,50
            for (char *tok = strtok(argv[++i], ","); tok && n_ks < 32; tok = strtok(NULL, ","))
                ks[n_ks++] = strtoull(tok, NULL, 10);
        } else if (strcmp(argv[i], "--fractions") == 0 && i + 1 < argc) {
            // lista separada por virgulas, ex.: 0.001,0.01,0.05
            char *list = argv[++i];
            for (char *tok = strtok(list, ","); tok && n_fractions < 16; tok = strtok(NULL, ","))
                fractions[n_fractions++] = strtod(tok, NULL);
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
        fprintf(stderr, "  --runs <num>         Execuções (média); padrão: 5 para random, 1 para as demais\n");
        fprintf(stderr, "  --k <lista>          Orçamentos k a avaliar, ex.: 1,5,10,50 (padrão: 1,2,5,10,... até 2%% de n)\n");
        fprintf(stderr, "  --fractions <lista>  Alternativa a --k, em frações de n, ex.: 0.001,0.01\n");
        fprintf(stderr, "  --trace              Mostra a trajetória (positivos por passo) do maior k\n");
        return 1;
    }

    const char *filename = positional[0];
    const char *heuristicName = positional[1];
    const char *modeStr = (posCount > 2) ? positional[2] : "simple";

    if (isSigned) {
        SignedGraph *graph = createSignedGraphFromFilename(filename);
        if (!graph) return 1;

        // Ponteiros para funções de heurísticas sinalizadas
        SignedHeuristic signedHeuristicFunc = NULL;

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
            if (n_ks == 0) {   // --fractions -> valores absolutos (crescentes, sem repeticao)
                for (uint64_t i = 0; i < n_fractions; i++) {
                    uint64_t k = (uint64_t) (fractions[i] * (double) graph->n_nodes + 0.999999);
                    if (k >= 1 && (n_ks == 0 || k > ks[n_ks - 1])) ks[n_ks++] = k;
                }
            }
            SignedBenchConfig cfg = {
                .t = steps,
                .runs = runs ? runs : (strcmp(heuristicName, "random") == 0 ? 5 : 1),
                .seed = RAND_SEED,
                .trace = showTrace,
                .ks = n_ks ? ks : NULL,
                .n_ks = n_ks,
            };
            signedTestHeuristics(graph, signedHeuristicFunc, &cfg);
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