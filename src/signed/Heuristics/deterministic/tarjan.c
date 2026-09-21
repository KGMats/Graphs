#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Stack.h"
#include "Utils.h"
#include "signed/signedGraph.h"
#include "signed/heuristics/deterministic/tarjan.h"

static inline uint64_t min(const uint64_t a, const uint64_t b) {
    return (a < b) ? a : b;
}


// Função auxiliar de dfs utilizada pelo Algoritmo de Tarjan
static uint64_t dfs_AP(SignedNode *startNode, char *visited, uint64_t disc[], uint64_t low[], uint64_t parent[], char *isAP, uint64_t *time) {
    uint64_t root_children = 0;
    LinkedList* stack = NULL;

    // Cria e empilha o primeiro estado para o nó inicial
    StackFrame* start_frame = malloc(sizeof(StackFrame));
    start_frame->u = startNode;
    start_frame->neighbor_idx = 0;
    StackPush(&stack, start_frame);

    while (stack != NULL) {
        // Verifica o topo da pilha sem remover
        StackFrame* current_frame = stack->frame;
        const SignedNode* u = current_frame->u;
        const uint64_t u_id = u->ID;

        // Se é a primeira vez que processamos este nó, fazemos as ações de "pré-ordem"
        if (!getNodeState(visited, u_id)) {
            setNodeState(visited, u_id, 1);
            disc[u_id] = low[u_id] = ++(*time);
        }

        // Verifica se ainda há vizinhos para explorar a partir de 'u'
        if (current_frame->neighbor_idx < u->degree) {
            SignedNode* v = get_clean_ptr(u->neighbors[current_frame->neighbor_idx]);
            const uint64_t v_id = v->ID;

            // Incrementa o índice para a próxima vez que voltarmos a este frame
            current_frame->neighbor_idx++;

            // Ignora o pai
            if (v_id == parent[u_id]) {
                continue;
            }

            if (getNodeState(visited, v_id)) {
                // Se o vizinho já foi visitado, é um atalho (back-edge)
                low[u_id] = min(low[u_id], disc[v_id]);
            } else {
                // Se não foi visitado, definimos o pai e empilhamos um novo estado para 'v'
                parent[v_id] = u_id;
                if (u_id == startNode->ID)
                {
                    root_children++;
                }

                StackFrame* new_frame = malloc(sizeof(StackFrame));
                new_frame->u = v;
                new_frame->neighbor_idx = 0;
                StackPush(&stack, new_frame);
            }
        } else {
            // Se todos os vizinhos de 'u' já foram processados, simulamos a "volta" da recursão
            const uint64_t p_id = parent[u_id];

            // Se 'u' não for a raiz de uma árvore DFS, ele atualiza o low-link de seu pai
            if (p_id != UINT64_MAX) {
                low[p_id] = min(low[p_id], low[u_id]);

                if (parent[p_id] != UINT64_MAX && low[p_id] < disc[parent[p_id]])
                {
                    setNodeState(isAP, p_id, 1);

                }

                // E o pai verifica se é um ponto de articulação devido a 'u'
                if (low[u_id] >= disc[p_id]) {
                    setNodeState(isAP, p_id, 1);
                }
            }

            // Remove o estado de 'u' da pilha e libera sua memória
            StackFrame* done_frame = StackPop(&stack);
            free(done_frame);
        }
    }
}

// Algoritmo de tarjan para encontrar pontos de articulação em um grafo.
uint64_t* tarjanSigned(const SignedGraph* graph) {
    const uint64_t n_nodes = graph->n_nodes;

    // TODO: usar bitsets no isAP e no visited ao invés de arrays de bool.
    uint64_t* disc = malloc(n_nodes * sizeof(uint64_t));
    uint64_t* low = malloc(n_nodes * sizeof(uint64_t));
    uint64_t* parent = malloc(n_nodes * sizeof(uint64_t));
    char* visited = calloc((n_nodes / 8) + 1, sizeof(char));
    char* isAP = calloc((n_nodes / 8) + 1, sizeof(char));
    uint64_t* solutions = malloc(n_nodes * sizeof(uint64_t));
    uint64_t n_solutions = 0;
    uint64_t time = 0;

    for (uint64_t i = 0; i < n_nodes; i++) {
        parent[i] = UINT64_MAX;
        solutions[i] = UINT64_MAX;
    }

    if (visited == NULL || disc == NULL || low == NULL || parent == NULL || isAP == NULL || solutions == NULL) {
        printf("Memory error in tarjan.\n");
        exit(0);
    }

    for (uint64_t i = 0; i < n_nodes; i++) {
        if (!getNodeState(visited, i)) {
            SignedNode* start_node = graph->nodes[i];

            // A REGRA DA RAIZ precisa ser verificada aqui, após o término do DFS
            // para um componente. Contamos quantos filhos a raiz teve.
            const uint64_t root_children = dfs_AP(start_node, visited, disc, low, parent, isAP, &time);

            if (root_children > 1) {
                setNodeState(isAP, start_node->ID, 1);
            }
        }
    }

    for (uint64_t i = 0; i < n_nodes; i++) {
        if (getNodeState(isAP, i)) {
            solutions[n_solutions++] = i;
        }
    }

    free(visited);
    free(disc);
    free(low);
    free(parent);
    free(isAP);
    return solutions;
}