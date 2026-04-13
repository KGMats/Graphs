add_executable(GraphSolver
        main.c
)

target_link_libraries(GraphSolver
        PRIVATE
        GraphsSolverLib
)

target_include_directories(
        GraphSolver
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)