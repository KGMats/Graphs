#ifndef SIGNED_MAXIMUMDEGREEHeuristic_H
#define SIGNED_MAXIMUMDEGREEHeuristic_H

#include <stdbool.h>
#include <stdint.h>

#include "signed/SignedGraph.h"

bool maximumDegreeHeuristic(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes);

#endif