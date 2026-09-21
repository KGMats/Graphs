#include <stdlib.h>
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/fullRandomHeuristic.h"

// rand() so garante 15 bits em algumas plataformas; combina varias chamadas para cobrir grafos grandes.
static uint64_t rand64(void) {
    return ((uint64_t) rand() << 45) ^ ((uint64_t) rand() << 30) ^ ((uint64_t) rand() << 15) ^ (uint64_t) rand();
}

// Sorteia k nos distintos (Fisher-Yates parcial). O primeiro elemento sorteado e o primeiro de `result`,
// entao os prefixos tambem sao amostras aleatorias sem reposicao.
uint64_t fullRandomHeuristicSigned(const SignedGraph *graph, uint64_t *result, const uint64_t initialActiveNodes, const uint64_t steps)
{
    (void) steps;
    const uint64_t n = graph->n_nodes;
    const uint64_t selected = initialActiveNodes < n ? initialActiveNodes : n;

    uint64_t *ids = malloc(sizeof(uint64_t) * n);
    for (uint64_t i = 0; i < n; i++) ids[i] = i;

    for (uint64_t i = 0; i < selected; i++) {
        const uint64_t j = i + rand64() % (n - i);
        const uint64_t tmp = ids[i]; ids[i] = ids[j]; ids[j] = tmp;
        result[i] = ids[i];
    }

    free(ids);
    return selected;
}
