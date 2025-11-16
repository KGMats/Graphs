cmake_minimum_required(VERSION 3.30)
project(SemNome C)

set(CMAKE_C_STANDARD 11)


include_directories(${CMAKE_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/Heuristics/deterministic")
include_directories("${CMAKE_SOURCE_DIR}/Heuristics/probabilistic")
include_directories("${CMAKE_SOURCE_DIR}/Meta-Heuristics")

add_executable(SemNome main.c
        Graph.c
        Graph.h
        Algorithms.c
        Algorithms.h
        Solutions.h
        Stack.h
        Stack.c
        LinkedList.h
        Utils.c
        Utils.h
        Tarjan.c
        Tarjan.h
        Heuristics/deterministic/MaximumDegreeHeuristic.c
        Heuristics/deterministic/MaximumDegreeHeuristic.h
        Heuristics/deterministic/bridgeHeuristic.c
        Heuristics/deterministic/bridgeHeuristic.h
        Heuristics/deterministic/hillClimbLocalSearch.c
        Heuristics/deterministic/hillClimbLocalSearch.h
        Heuristics/deterministic/hillClimbLocalSearch.c
        Meta-Heuristics/grasp.c
        Meta-Heuristics/grasp.h
        Heuristics/deterministic/fullRandomHeuristic.c
        Heuristics/deterministic/fullRandomHeuristic.h
        Heuristics/deterministic/farthestFromActivationHeuristic.c
        Heuristics/deterministic/farthestFromActivationHeuristic.h
        IndexedHeap.c
        IndexedHeap.h
        Heuristics/probabilistic/farthestFromActivation_r.c
        Heuristics/probabilistic/farthestFromActivation_r.h
)
