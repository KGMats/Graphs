#ifndef SEMNOME_SOLUTIONS_H
#define SEMNOME_SOLUTIONS_H


#include "unsigned/graph.h"

// Heuristicas Deterministicas
#include "unsigned/heuristics/deterministic/maximumDegreeHeuristic.h"
#include "unsigned/heuristics/deterministic/bridgeHeuristic.h"
#include "unsigned/localSearch/hillClimb.h"
#include "unsigned/heuristics/deterministic/farthestFromActivationHeuristic.h"

// Heuristicas probabilisticas
#include "unsigned/heuristics/probabilistic/farthestFromActivation_r.h"
#include "unsigned/heuristics/probabilistic/fullRandomHeuristic.h"

// Meta-Heuristicas
#include "unsigned/metaHeuristics/grasp.h"

#endif //SEMNOME_SOLUTIONS_H
