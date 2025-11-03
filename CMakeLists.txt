cmake_minimum_required(VERSION 3.30)
project(SemNome C)

set(CMAKE_C_STANDARD 11)


include_directories(${CMAKE_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/Heuristics")
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
        Heuristics/MaximumDegreeHeuristic.c
        Heuristics/MaximumDegreeHeuristic.h
        Heuristics/bridgeHeuristic.c
        Heuristics/bridgeHeuristic.h
        Heuristics/hillClimbLocalSearch.c
        Heuristics/hillClimbLocalSearch.h
        Heuristics/hillClimbLocalSearch.c
        Meta-Heuristics/grasp.c
        Meta-Heuristics/grasp.h
        Heuristics/fullRandomHeuristic.c
        Heuristics/fullRandomHeuristic.h
        Heuristics/farthestFromActivationHeuristic.c
        Heuristics/farthestFromActivationHeuristic.h
)
