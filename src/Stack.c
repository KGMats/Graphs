#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>

// Função para empilhar um valor na pilha
void StackPush(LinkedList** stack, StackFrame* frame) {
    LinkedList* stackNode = (LinkedList*)malloc(sizeof(LinkedList));
    stackNode->frame = frame;
    stackNode->next = *stack;
    *stack = stackNode;
}

// Função para desempilhar um estado (frame) da pilha
// quem chama é responsável por liberar a memória do frame.
StackFrame* StackPop(LinkedList **stack) {
    if (!(*stack)) {
        return NULL;
    }

    LinkedList* topNode = *stack;
    StackFrame* frame = topNode->frame;
    *stack = topNode->next;
    free(topNode);
    return frame;
}
