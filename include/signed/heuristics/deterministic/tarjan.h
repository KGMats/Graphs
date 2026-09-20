#ifndef SIGNED_TARJAN_H
#define SIGNED_TARJAN_H
#include <stdint.h>
#include "signed/SignedGraph.h"

uint64_t* tarjan(const SignedGraph* graph);

#endif