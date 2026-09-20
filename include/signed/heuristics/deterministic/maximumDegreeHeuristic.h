#ifndef SIGNED_MAXIMUMDEGREEHEURISTIC_H
#define SIGNED_MAXIMUMDEGREEHEURISTIC_H

#include <stdbool.h>
#include <stdint.h>

#include "signed/signedGraph.h"

bool maximumDegreeHeuristicSigned(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes, uint64_t t);

#endif //SIGNED_MAXIMUMDEGREEHEURISTIC_H