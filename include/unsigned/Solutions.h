#ifndef SEMNOME_SOLUTIONS_H
#define SEMNOME_SOLUTIONS_H


#include "unsigned/Graph.h"

// Heuristicas Deterministicas
#include "unsigned/Heuristics/deterministic/MaximumDegreeHeuristic.h"
#include "unsigned/Heuristics/deterministic/bridgeHeuristic.h"
#include "unsigned/LocalSearch/hillClimb.h"
#include "unsigned/Heuristics/deterministic/farthestFromActivationHeuristic.h"

// Heuristicas probabilisticas
#include "unsigned/Heuristics/probabilistic/farthestFromActivation_r.h"
#include "unsigned/Heuristics/probabilistic/fullRandomHeuristic.h"

// Meta-Heuristicas
#include "unsigned/Meta-Heuristics/grasp.h"

#endif //SEMNOME_SOLUTIONS_H
