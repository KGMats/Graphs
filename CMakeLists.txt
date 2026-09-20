cmake_minimum_required(VERSION 3.30)
project(GraphSolver C)

set(CMAKE_C_STANDARD 11)
add_library(GraphsSolverLib
        include/signed/algorithms.h
        include/signed/heuristics/deterministic/bridgeHeuristic.h
        include/signed/heuristics/deterministic/farthestFromActivationHeuristic.h
        include/signed/heuristics/deterministic/fullRandomHeuristic.h
        include/signed/heuristics/deterministic/maximumDegreeHeuristic.h
        include/signed/heuristics/deterministic/tarjan.h
        include/signed/signedGraph.h
        include/signed/Solutions.h
        include/unsigned/Solutions.h
        include/unsigned/algorithms.h
        include/unsigned/graph.h
        include/unsigned/heuristics/deterministic/bridgeHeuristic.h
        include/unsigned/heuristics/deterministic/farthestFromActivationHeuristic.h
        include/unsigned/heuristics/deterministic/maximumDegreeHeuristic.h
        include/unsigned/heuristics/deterministic/tarjan.h
        include/unsigned/heuristics/probabilistic/farthestFromActivation_r.h
        include/unsigned/heuristics/probabilistic/fullRandomHeuristic.h
        include/unsigned/localSearch/hillClimb.h
        include/unsigned/metaHeuristics/grasp.h
        src/IndexedHeap.c
        src/IndexedHeap.h
        src/LinkedList.h
        src/Stack.c
        src/Stack.h
        src/Utils.c
        src/Utils.h
        src/signed/Heuristics/deterministic/bridgeHeuristic.c
        src/signed/Heuristics/deterministic/farthestFromActivationHeuristic.c
        src/signed/Heuristics/deterministic/fullRandomHeuristic.c
        src/signed/Heuristics/deterministic/maximumDegreeHeuristic.c
        src/signed/Heuristics/deterministic/tarjan.c
        src/signed/signedGraph.c
        src/signed/algorithms.c
        src/unsigned/Heuristics/deterministic/bridgeHeuristic.c
        src/unsigned/Heuristics/deterministic/farthestFromActivationHeuristic.c
        src/unsigned/Heuristics/deterministic/fullRandomHeuristic.c
        src/unsigned/Heuristics/deterministic/maximumDegreeHeuristic.c
        src/unsigned/Heuristics/deterministic/tarjan.c
        src/unsigned/Heuristics/probabilistic/farthestFromActivation_r.c
        src/unsigned/LocalSearch/hillClimb.c
        src/unsigned/Meta-Heuristics/grasp.c
        src/unsigned/algorithms.c
        src/unsigned/graph.c
)

target_compile_features(GraphsSolverLib PUBLIC)
target_include_directories(GraphsSolverLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


add_subdirectory(tools)
