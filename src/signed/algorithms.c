#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "signed/signedGraph.h"
#include "signed/algorithms.h"
#include "Utils.h"
#include <string.h>

// Funcao utilizada para verificar se o número de vizinhos ativos
// é suficiente para ativar o nó passado como argumento
bool signedActivationFunction(SignedNode *node)
{
    if (node->n_active_neighbors == 0)
    {
        return false;
    }
    uint64_t th = threshold(node);
    int64_t abs_influence = node->n_active_neighbors;
    if (abs_influence < 0)
    {
        abs_influence = -abs_influence;
    }

    return abs_influence >= (int64_t) th;
}

// Nucleo da propagacao sincrona por ate t passos.
//  want_list : se false, nao monta a lista de nos ativos (evita um malloc e uma varredura O(n)).
//  trace     : opcional, t + 1 entradas (ver SignedStep em algorithms.h).
//  steps_run / converged : opcionais; passos em que algo mudou e se parou em ponto fixo.
static PropagationResult propagateCore(const SignedGraph *graph, const uint64_t t, const bool want_list,
                                       SignedStep *trace, uint64_t *steps_run, bool *converged) {
    const uint64_t n = graph->n_nodes;

    uint64_t *changed_nodes = malloc(sizeof(uint64_t) * n);
    int *new_states = malloc(sizeof(int) * n);
    int *old_states = malloc(sizeof(int) * n);

    uint64_t pos = 0, neg = 0;
    if (trace) {
        for (uint64_t i = 0; i < n; i++) {
            const int s = getSignedNodeState(graph->active_nodes, i);
            if (s > 0) pos++; else if (s < 0) neg++;
        }
        trace[0].changed = 0;
        trace[0].positive = pos;
        trace[0].negative = neg;
    }

    uint64_t step = 0;
    bool fixed_point = false;
    for (; step < t; step++) {
        uint64_t n_changed = 0;

        for (uint64_t i = 0; i < n; i++) {
            SignedNode *node = graph->nodes[i];
            int curr_s = getSignedNodeState(graph->active_nodes, i);

            int64_t I = node->n_active_neighbors;
            uint64_t th = threshold(node);
            int64_t abs_I = (I < 0) ? -I : I;

            int next_s;
            if (abs_I >= (int64_t) th) {
                next_s = (I > 0) ? 1 : -1;
            } else if (I == 0) {
                next_s = 0;
            } else {
                next_s = curr_s;
            }

            if (next_s != curr_s) {
                changed_nodes[n_changed] = i;
                old_states[n_changed] = curr_s;
                new_states[n_changed] = next_s;
                n_changed++;
            }
        }

        if (n_changed == 0) {
            fixed_point = true;
            break;
        }

        for (uint64_t k = 0; k < n_changed; k++) {
            uint64_t id = changed_nodes[k];
            int s_new = new_states[k];
            int s_old = old_states[k];
            int delta_s = s_new - s_old;

            setSignedNodeState(graph->active_nodes, id, s_new);

            if (trace) {
                if (s_old > 0) pos--; else if (s_old < 0) neg--;
                if (s_new > 0) pos++; else if (s_new < 0) neg++;
            }

            const SignedNode *node = graph->nodes[id];
            for (uint64_t j = 0; j < node->degree; j++) {
                SignedNode *tagged_neighbor = node->neighbors[j];
                SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
                int edge_sign = get_edge_sign(tagged_neighbor);

                clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
            }
        }

        if (trace) {
            trace[step + 1].changed = n_changed;
            trace[step + 1].positive = pos;
            trace[step + 1].negative = neg;
        }
    }

    if (steps_run) *steps_run = step;
    if (converged) *converged = fixed_point;

    free(changed_nodes);
    free(new_states);
    free(old_states);

    PropagationResult result = {0, NULL};
    if (want_list) {
        result.activated_nodes = malloc(sizeof(uint64_t) * n);
        for (uint64_t i = 0; i < n; i++) {
            if (getSignedNodeState(graph->active_nodes, i) != 0) {
                result.activated_nodes[result.count++] = i;
            }
        }
    }
    return result;
}

// Função que percorre o grafo e propaga a influência de forma síncrona por até t passos.
PropagationResult signedPropagate(const SignedGraph *graph, const uint64_t t) {
    return propagateCore(graph, t, true, NULL, NULL, NULL);
}

// Coloca os nos `ids` no estado +1 e atualiza os vizinhos (n_active_neighbors).
static void seedNodes(const SignedGraph *graph, const uint64_t n_ids, const uint64_t *ids) {
    for (uint64_t i = 0; i < n_ids; i++) {
        uint64_t id = ids[i];
        int curr_s = getSignedNodeState(graph->active_nodes, id);
        if (curr_s == 1) continue;

        int delta_s = 1 - curr_s;
        setSignedNodeState(graph->active_nodes, id, 1);

        const SignedNode *node = graph->nodes[id];
        for (uint64_t j = 0; j < node->degree; j++) {
            SignedNode *tagged_neighbor = node->neighbors[j];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
        }
    }
}

// Função similar a propagate, mas ativa inicialmente os nós dados por changed_nodes
PropagationResult signedPartialPropagate(const SignedGraph *graph, const uint64_t n_changed,
                                   const uint64_t *changed_nodes, const uint64_t t) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }

    seedNodes(graph, n_changed, changed_nodes);
    return signedPropagate(graph, t);
}

// Função similar ao partialPropagate, mas aqui ao invés de ativarmos os nós
// dados por changed_nodes, desativamos eles (estado 0) e propagamos as mudanças por até t passos.
PropagationResult signedPartialReversePropagate(const SignedGraph *graph, const uint64_t n_changed,
                                          const uint64_t *changed_nodes, const uint64_t t) {
    if (n_changed == 0) {
        PropagationResult result = {0, NULL};
        return result;
    }


    for (uint64_t i = 0; i < n_changed; i++) {
        uint64_t id = changed_nodes[i];
        int curr_s = getSignedNodeState(graph->active_nodes, id);
        if (curr_s == 0) continue;

        int delta_s = 0 - curr_s;
        setSignedNodeState(graph->active_nodes, id, 0);

        const SignedNode *node = graph->nodes[id];
        for (uint64_t j = 0; j < node->degree; j++) {
            SignedNode *tagged_neighbor = node->neighbors[j];
            SignedNode *clean_neighbor = get_clean_ptr(tagged_neighbor);
            int edge_sign = get_edge_sign(tagged_neighbor);

            clean_neighbor->n_active_neighbors += (int64_t) edge_sign * delta_s;
        }
    }

    return signedPropagate(graph, t);
}

// Função que verifica se um dado conjunto de nós (activeNodeIDs)
// é capaz de ativar inteiramente o grafo em até t passos
bool signedRunTest(const SignedGraph *graph, const uint64_t *activeNodeIDs, const uint64_t n_ids, const uint64_t t) {
    PropagationResult result = signedPartialPropagate(graph, n_ids, activeNodeIDs, t);
    free(result.activated_nodes);

    uint64_t counter = countActiveSignedNodes(graph);

    return counter == graph->n_nodes;
}

SignedOutcome signedEvaluate(const SignedGraph *graph, const uint64_t *seeds, const uint64_t k,
                             const uint64_t t, SignedStep *trace) {
    deactivateAllSigned(graph);
    seedNodes(graph, k, seeds);

    SignedOutcome out = {0, 0, 0, 0, true};
    const PropagationResult r = propagateCore(graph, t, false, trace, &out.steps_run, &out.converged);
    free(r.activated_nodes);
    if (trace) trace[0].changed = k;

    for (uint64_t i = 0; i < graph->n_nodes; i++) {
        const int s = getSignedNodeState(graph->active_nodes, i);
        if (s > 0) out.positive++;
        else if (s < 0) out.negative++;
        else out.neutral++;
    }
    return out;
}

static double nowSeconds(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double) ts.tv_sec + (double) ts.tv_nsec / 1e9;
}

// Serie 1,2,5,10,20,50,... ate 2% de n (pelo menos k = 1). Com limiar 1 a propagacao satura rapido;
// e nos orcamentos pequenos que as heuristicas se diferenciam.
static uint64_t defaultBudgets(const uint64_t n, uint64_t *out, const uint64_t capacity) {
    const uint64_t limit = n / 50 > 0 ? n / 50 : 1;
    uint64_t count = 0;
    for (uint64_t decade = 1; count < capacity; decade *= 10) {
        const uint64_t steps[3] = {1, 2, 5};
        for (int i = 0; i < 3 && count < capacity; i++) {
            const uint64_t k = steps[i] * decade;
            if (k > limit) return count ? count : (out[count++] = 1);
            out[count++] = k;
        }
    }
    return count;
}

void signedTestHeuristics(const SignedGraph *graph, const SignedHeuristic heuristic, const SignedBenchConfig *cfg) {
    const uint64_t n = graph->n_nodes;
    const uint64_t t = cfg->t;
    const uint64_t runs = cfg->runs ? cfg->runs : 1;

    // Grade de orcamentos k (crescente, sem repeticoes, limitada a n)
    uint64_t ks[64];
    uint64_t n_ks = 0;
    if (cfg->ks != NULL && cfg->n_ks > 0) {
        for (uint64_t i = 0; i < cfg->n_ks && n_ks < 64; i++) {
            uint64_t k = cfg->ks[i] > n ? n : cfg->ks[i];
            if (k >= 1 && (n_ks == 0 || k > ks[n_ks - 1])) ks[n_ks++] = k;
        }
    } else {
        n_ks = defaultBudgets(n, ks, 64);
    }
    if (n_ks == 0) { ks[0] = 1; n_ks = 1; }
    const uint64_t k_max = ks[n_ks - 1];

    uint64_t *seeds = malloc(sizeof(uint64_t) * k_max);
    double *sum_pos = calloc(n_ks, sizeof(double));
    double *sum_pos_avg = calloc(n_ks, sizeof(double));
    double *sum_neg = calloc(n_ks, sizeof(double));
    uint64_t *n_converged = calloc(n_ks, sizeof(uint64_t));
    SignedStep *trace = malloc(sizeof(SignedStep) * (t + 1));     // sempre: e barato e da a media dos 2 ultimos passos
    SignedStep *kept_trace = cfg->trace ? malloc(sizeof(SignedStep) * (t + 1)) : NULL;
    uint64_t kept_steps = 0;
    double heuristic_time = 0, eval_time = 0;
    uint64_t min_selected = k_max;

    for (uint64_t r = 0; r < runs; r++) {
        srand((unsigned) (cfg->seed + r));
        deactivateAllSigned(graph);

        double t0 = nowSeconds();
        const uint64_t selected = heuristic(graph, seeds, k_max, t);
        heuristic_time += nowSeconds() - t0;
        if (selected < min_selected) min_selected = selected;

        // Uma unica chamada com k_max; cada k e avaliado como prefixo (ver contrato em algorithms.h).
        t0 = nowSeconds();
        for (uint64_t i = 0; i < n_ks; i++) {
            const uint64_t k = ks[i] < selected ? ks[i] : selected;
            const SignedOutcome o = signedEvaluate(graph, seeds, k, t, trace);

            // Com oscilacao de periodo 2 o valor em t depende da paridade de t. Se nao convergiu,
            // a media dos passos t-1 e t nao depende da fase. Se convergiu, vale o estado final.
            double avg = (double) o.positive;
            if (!o.converged && o.steps_run >= 1)
                avg = 0.5 * ((double) trace[o.steps_run].positive + (double) trace[o.steps_run - 1].positive);

            sum_pos[i] += (double) o.positive;
            sum_pos_avg[i] += avg;
            sum_neg[i] += (double) o.negative;
            if (o.converged) n_converged[i]++;
            if (kept_trace && r == 0 && i == n_ks - 1) {
                memcpy(kept_trace, trace, sizeof(SignedStep) * (o.steps_run + 1));
                kept_steps = o.steps_run;
            }
        }
        eval_time += nowSeconds() - t0;
    }

    printf("Avaliacao: n=%llu, t=%llu, execucoes=%llu, semente=%llu\n",
           (unsigned long long) n, (unsigned long long) t,
           (unsigned long long) runs, (unsigned long long) cfg->seed);
    printf("%10s %9s %13s %15s %13s %12s %10s\n",
           "k", "k/n(%)", "positivos(%)", "pos.media2(%)", "negativos(%)", "neutros(%)", "convergiu");
    for (uint64_t i = 0; i < n_ks; i++) {
        const double pos = 100. * (sum_pos[i] / (double) runs) / (double) n;
        const double avg = 100. * (sum_pos_avg[i] / (double) runs) / (double) n;
        const double neg = 100. * (sum_neg[i] / (double) runs) / (double) n;
        printf("%10llu %9.4f %13.3f %15.3f %13.3f %12.3f %5llu/%-4llu\n",
               (unsigned long long) ks[i], 100. * (double) ks[i] / (double) n, pos, avg, neg, 100. - pos - neg,
               (unsigned long long) n_converged[i], (unsigned long long) runs);
    }
    printf("(positivos = estado no passo t, definicao do modelo; media2 = media dos passos t-1 e t quando nao convergiu)\n");
    if (min_selected < k_max)
        printf("AVISO: a heuristica devolveu so %llu das %llu sementes pedidas; os k maiores foram avaliados com esse numero.\n",
               (unsigned long long) min_selected, (unsigned long long) k_max);
    printf("Tempo: heuristica %.3f s | avaliacao %.3f s (medias por execucao)\n",
           heuristic_time / (double) runs, eval_time / (double) runs);

    if (kept_trace) {
        printf("Trajetoria para k=%llu (positivos apos cada passo; passo 0 = so as sementes):\n ",
               (unsigned long long) k_max);
        for (uint64_t s = 0; s <= kept_steps; s++) printf(" %llu", (unsigned long long) kept_trace[s].positive);
        printf("\nNos alterados por passo:\n ");
        for (uint64_t s = 1; s <= kept_steps; s++) printf(" %llu", (unsigned long long) kept_trace[s].changed);
        printf("\n");
    }

    free(seeds); free(sum_pos); free(sum_pos_avg); free(sum_neg); free(n_converged); free(trace); free(kept_trace);
}
