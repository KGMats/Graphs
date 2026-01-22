#ifndef SEMNOME_STACK_H
#define SEMNOME_STACK_H

#include <stdint.h>
#include "LinkedList.h"


void StackPush(LinkedList** stack, StackFrame* frame);
StackFrame* StackPop(LinkedList **stack);

#endif //SEMNOME_STACK_H
