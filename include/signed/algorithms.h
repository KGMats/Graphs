#ifndef SIGNED_ALGORITHMS_H
#define SIGNED_ALGORITHMS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "signed/signedGraph.h"

#ifndef PROPAGATION_RESULT_DEFINED
#define PROPAGATION_RESULT_DEFINED
typedef struct {
    uint64_t count;
    uint64_t *activated_nodes;
} PropagationResult;
#endif

bool signedActivationFunction(SignedNode *node);

PropagationResult signedPropagate(const SignedGraph *graph, uint64_t t);

PropagationResult signedPartialPropagate(const SignedGraph *graph, uint64_t n_changed,
                                   const uint64_t *changed_nodes, uint64_t t);

PropagationResult signedPartialReversePropagate(const SignedGraph *graph, uint64_t n_changed,
                                          const uint64_t *changed_nodes, uint64_t t);

bool signedRunTest(const SignedGraph *graph, const uint64_t *activeNodeIDs, uint64_t n_ids, uint64_t t);

// ---------------------------------------------------------------------------
// Avaliacao de solucoes
// ---------------------------------------------------------------------------

// Uma heuristica so SELECIONA sementes. Ela preenche `result` com ate `k` IDs distintos,
// em ordem de prioridade, e devolve quantos preencheu. Contrato: os primeiros k' IDs
// (k' <= k) sao a solucao da propria heuristica para o orcamento k'. Assim uma unica
// chamada com k_max serve para avaliar varios orcamentos menores (prefixos).
// O grafo chega sem nos ativos; quem avalia a solucao e signedEvaluate, nao a heuristica.
typedef uint64_t (*SignedHeuristic)(const SignedGraph *graph, uint64_t *result, uint64_t k, uint64_t steps);

// Estado do grafo depois da simulacao.
typedef struct {
    uint64_t positive;      // nos com estado +1 (objetivo: maximizar)
    uint64_t negative;      // nos com estado -1
    uint64_t neutral;       // nos com estado 0
    uint64_t steps_run;     // passos em que alguma coisa mudou (== t se nao parou antes)
    bool     converged;     // true: chegou a um ponto fixo antes (ou no) de t passos
} SignedOutcome;

// Foto do grafo apos cada passo. trace[0] = logo apos ativar as sementes (changed = k);
// trace[s] = apos o passo s. Precisa ter espaco para t + 1 entradas; so as primeiras
// steps_run + 1 sao preenchidas.
typedef struct {
    uint64_t changed;       // nos que mudaram de estado neste passo
    uint64_t positive;
    uint64_t negative;
} SignedStep;

// Zera o grafo, ativa `seeds[0..k)` com +1 no instante 0 e simula ate t passos.
// `trace` pode ser NULL.
SignedOutcome signedEvaluate(const SignedGraph *graph, const uint64_t *seeds, uint64_t k,
                             uint64_t t, SignedStep *trace);

typedef struct {
    uint64_t t;             // passos de propagacao
    uint64_t runs;          // execucoes (>1 so faz sentido para heuristicas aleatorias); 0 = 1
    uint64_t seed;          // semente base do rand(); a execucao r usa seed + r
    bool     trace;          // imprime a trajetoria (positivos por passo) do maior k
    const uint64_t *ks;      // orcamentos k a avaliar (crescentes); NULL = serie 1,2,5,10,20,50,... ate 2% de n
    uint64_t n_ks;
} SignedBenchConfig;

void signedTestHeuristics(const SignedGraph *graph, SignedHeuristic heuristic, const SignedBenchConfig *cfg);

#endif //SIGNED_ALGORITHMS_H
