cmake_minimum_required(VERSION 3.30)
project(GraphSolver C)

set(CMAKE_C_STANDARD 11)


add_library(GraphsSolverLib
        src/Graph.c
        include/Graph.h
        src/Algorithms.c
        include/Algorithms.h
        src/Solutions.h
        src/Stack.h
        src/Stack.c
        src/LinkedList.h
        src/Utils.c
        src/Utils.h
        src/Tarjan.c
        src/Tarjan.h
        src/Heuristics/deterministic/MaximumDegreeHeuristic.c
        include/Heuristics/deterministic/MaximumDegreeHeuristic.h
        src/Heuristics/deterministic/bridgeHeuristic.c
        include/Heuristics/deterministic/bridgeHeuristic.h
        src/Heuristics/deterministic/hillClimbLocalSearch.c
        include/Heuristics/deterministic/hillClimbLocalSearch.h
        src/Heuristics/deterministic/hillClimbLocalSearch.c
        src/Meta-Heuristics/grasp.c
        include/Meta-Heuristics/grasp.h
        src/Heuristics/deterministic/fullRandomHeuristic.c
        include/Heuristics/deterministic/fullRandomHeuristic.h
        src/Heuristics/deterministic/farthestFromActivationHeuristic.c
        include/Heuristics/deterministic/farthestFromActivationHeuristic.h
        src/IndexedHeap.c
        src/IndexedHeap.h
        src/Heuristics/probabilistic/farthestFromActivation_r.c
        include/Heuristics/probabilistic/farthestFromActivation_r.h
)

target_compile_features(GraphsSolverLib PUBLIC)
target_include_directories(GraphsSolverLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Heuristics/deterministic
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Heuristics/probabilistic
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Meta-Heuristics
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
