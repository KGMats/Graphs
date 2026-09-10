cmake_minimum_required(VERSION 3.30)
project(GraphSolver C)

set(CMAKE_C_STANDARD 11)
add_library(GraphsSolverLib
        src/Graph.c
        include/unsigned/Graph.h
        src/unsigned/Algorithms.c
        include/unsigned/Algorithms.h
        include/unsigned/Solutions.h
        src/Stack.h
        src/Stack.c
        src/LinkedList.h
        src/Utils.c
        src/Utils.h
        src/unsigned/Heuristics/deterministic/Tarjan.c
        include/unsigned/Heuristics/deterministic/Tarjan.h
        src/unsigned/Heuristics/deterministic/MaximumDegreeHeuristic.c
        include/unsigned/Heuristics/deterministic/MaximumDegreeHeuristic.h
        src/unsigned/Heuristics/deterministic/bridgeHeuristic.c
        include/unsigned/Heuristics/deterministic/bridgeHeuristic.h
        src/unsigned/LocalSearch/hillClimb.c
        include/unsigned/LocalSearch/hillClimb.h
        src/unsigned/Meta-Heuristics/grasp.c
        include/unsigned/Meta-Heuristics/grasp.h
        src/unsigned/Heuristics/deterministic/fullRandomHeuristic.c
        include/unsigned/Heuristics/probabilistic/fullRandomHeuristic.h
        src/unsigned/Heuristics/deterministic/farthestFromActivationHeuristic.c
        include/unsigned/Heuristics/deterministic/farthestFromActivationHeuristic.h
        src/IndexedHeap.c
        src/IndexedHeap.h
        src/unsigned/Heuristics/probabilistic/farthestFromActivation_r.c
        include/unsigned/Heuristics/probabilistic/farthestFromActivation_r.h
)

target_compile_features(GraphsSolverLib PUBLIC)
target_include_directories(GraphsSolverLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


add_subdirectory(tools)
