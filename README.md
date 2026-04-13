# Target Set Selection on Graphs

A C library implementing and benchmarking heuristic and meta-heuristic algorithms for the **Target Set Selection (TSS)** problem — developed as a directed study (*Estudo Dirigido*) at UNICAMP, under supervision of Prof. Santiago Valdés Ravelo.

---

## What is Target Set Selection?

Given a graph G = (V, E) where each vertex has an **activation threshold** θ(v), the goal is to find the smallest possible **seed set** S ⊆ V such that, when the vertices in S are activated at time t = 0, all remaining vertices eventually become active through a cascade propagation process.

A vertex v becomes active at time t+1 if the number of already-active neighbors meets or exceeds its threshold. Once active, a vertex stays active forever.

This study uses the **Simple Majority** activation rule: a vertex v activates when at least half of its neighbors are already active.

TSS is a generalization of the classical **Dominating Set** problem (the special case where θ(v) = 1 for all v and propagation occurs in a single step).

---

## Algorithms Implemented

### Constructive Heuristics

| Heuristic | Strategy |
|---|---|
| **Maximum Degree** | Selects the k highest-degree vertices |
| **Bridge Heuristic** | Prioritizes articulation points (via Tarjan's algorithm), falls back to degree |
| **Random** | Selects k vertices uniformly at random |
| **Farthest From Activation** | Greedily selects the vertex hardest to activate by propagation, updating priorities dynamically via an indexed max-heap |

### Refinement

- **Hill Climb Local Search** - "Remove two, add one" swap strategy to reduce |S| while maintaining full coverage
- **GRASP** - Iterates between a probabilistic variant of Farthest From Activation and Hill Climb to escape local optima

---

## Results

Tested on three instances of increasing scale, from a hand-crafted debug graph to a real-world social network with nearly 5 million nodes. All real instances were obtained from the [Stanford SNAP dataset collection](http://snap.stanford.edu/data) and treated as undirected.

### Constructive Heuristics

| Instance | Heuristic | \|S\| | % of Graph | Time | Memory |
|---|---|---|---|---|---|
| **Synthetic** (13 nodes) | Max Degree | 2 | 15.38% | < 0.01s | 1.4 MB |
| | Bridge | 2 | 15.38% | < 0.01s | 1.4 MB |
| | Farthest | 2 | 15.38% | < 0.01s | 1.3 MB |
| **facebook_combined** (~4k nodes) | Max Degree | 3,796 | 93.98% | 0.02s | 5.9 MB |
| | Bridge | 3,471 | 85.93% | 0.09s | 6.7 MB |
| | Random | 3,630 | 89.87% | 0.03s | 3.9 MB |
| | Farthest | 579 | 14.33% | 0.05s | 3.9 MB |
| **soc-LiveJournal1** (~4.8M nodes) | Max Degree | 4,847,237 | 99.99% | 1m 29s | 1.5 GB |
| | Bridge | 4,544,599 | 93.75% | 1h 32m | 1.7 GB |
| | Farthest | 180,265 | 3.71% | 4m 20s | 1.5 GB |

Static topological strategies (Max Degree and Bridge) tend to select nearly the entire graph on real TSS instances. Farthest From Activation, by accounting for the propagation dynamics at each step, achieves results orders of magnitude better - 3.71% of the LiveJournal graph vs. 99.99% for Max Degree.

### Refinement on `facebook_combined`

| Method | Final \|S\| | Reduction | Total Time |
|---|---|---|---|
| Max Degree + Hill Climb | 742 | 80.45% | 13.2s |
| Farthest (no refinement) | 579 | — | 0.05s |
| Farthest + Hill Climb | 516 | 10.88% | 5.5s |
| GRASP (Probabilistic Farthest + Hill Climb, 50 iter.) | **477** | **17.61%** | 1m 21s |

GRASP achieved the best result overall. Hill Climb alone was essential to rescue poor initial solutions (Max Degree dropped from 3,796 to 742), though even after refinement it didn't match the Farthest constructive baseline. Applying GRASP to `soc-LiveJournal1` proved computationally infeasible (30+ hours without completion).

---

## Implementation Highlights

**Bitset state vector** - Node activation state is stored 1 bit per node (8 nodes per byte), reducing memory usage by ~87% compared to a boolean array. For `soc-LiveJournal1`, this shrinks the state vector from ~4.8 MB to ~600 KB, keeping it entirely in CPU cache and dramatically accelerating propagation.

**Cached active-neighbor count** - Each node stores an `n_active_neighbors` counter. When a node activates, it increments each neighbor's counter. The activation condition `n_active_neighbors / n_neighbors >= 0.5` is then checked immediately in constant time, without scanning the full adjacency list on every propagation step.

**Indexed Max-Heap** - The Farthest From Activation heuristic maintains a priority queue over all unselected nodes, keyed by their "distance to activation". When a node is added to the seed set and its influence propagates, affected neighbors get their priority updated in-place. This makes the full ~4.8M-node LiveJournal run complete in under 5 minutes, compared to hours with a naive re-sort approach.

---

## Project Structure

**`include/`** — Public headers. Everything a consumer of the library needs to call into it lives here, mirroring the layout of `src/`.

- `Graph.h` / `Algorithms.h` — Core graph types (`Graph`, `Node`, `Edge`) and propagation routines (`propagate`, `partialPropagate`, `runTest`).
- `Heuristics/deterministic/` — Headers for all deterministic constructive heuristics (Max Degree, Bridge, Farthest, Random).
- `Heuristics/probabilistic/` — Header for the randomized variant of Farthest From Activation used inside GRASP.
- `LocalSearch/` — Header for Hill Climb, kept separate from constructive heuristics since it operates on an existing solution rather than building one from scratch.
- `Meta-Heuristics/` — Header for the GRASP driver, which wires together any constructive heuristic and local search.

**`src/`** — Implementation files.

- `Graph.c` — Graph allocation, file loading (edge-list format), the bitset state vector, node activation/deactivation.
- `Algorithms.c` — Forward and reverse propagation engine, test runners.
- `IndexedHeap.c` — Indexed max-heap used by Farthest From Activation for O(log n) priority updates.
- `Tarjan.c` / `Stack.c` — Tarjan's articulation-point algorithm and its supporting stack.
- `Utils.c` — Shared utility functions (timing, sorting helpers).
- `LocalSearch/` — Hill Climb implementation.
- `Heuristics/` — One file per constructive heuristic, organized into `deterministic/` and `probabilistic/` subdirectories matching `include/`.

---

## Usage

```
./GraphSolver <graph_path> <heuristic> [mode]
```

`<graph_path>` — path to an edge-list file (see Input Format below).

`<heuristic>` — constructive heuristic to build the initial seed set:

| Value | Heuristic |
|---|---|
| `maxdegree` | Maximum Degree |
| `bridge` | Bridge (Articulation Points) |
| `farthest` | Farthest From Activation |
| `random` | Random |

`[mode]` — optional refinement strategy applied on top of the heuristic (defaults to no refinement):

| Value | Behavior |
|---|---|
| `simple` | No refinement, returns the constructive solution as-is |
| `hillclimb` | Applies Hill Climb local search to reduce the seed set |
| `grasp` | Runs GRASP: 50 iterations of probabilistic Farthest + Hill Climb |

**Examples:**

```bash
# Farthest From Activation, no refinement
./GraphSolver graphs/facebook.txt farthest

# Farthest From Activation + Hill Climb
./GraphSolver graphs/facebook.txt farthest hillclimb

# Full GRASP
./GraphSolver graphs/facebook.txt farthest grasp
```

## Building

**Requirements:** CMake 3.30+, GCC (C11)

```bash
git clone https://github.com/KGMats/Graphs.git
cd Graphs
mkdir build && cd build
cmake ..
make
```

---

## Input Format

Graph files should be edge lists with one edge per line:

```
0 1
0 2
1 3
...
```

Nodes must be zero-indexed integers. The graph is treated as **undirected and unweighted**.

To load a graph and run a heuristic:

```c
Graph *g = createGraphFromFilename("my_graph.txt");

uint64_t solution[MAX_NODES];
farthestFromActivationHeuristic(g, solution, k);

// optionally refine:
uint64_t new_size = hillClimbSimple(g, solution, k);
```

You can also run GRASP directly:

```c
uint64_t best = GRASP(g, 50, seed, probabilisticFarthestHeuristic, hillClimbSimple);
```

---

## Datasets Used

- **facebook_combined** — Facebook friendship circles (~4k nodes, ~88k edges)
- **soc-LiveJournal1** — LiveJournal social network (~4.8M nodes, ~69M edges)

Both available at [snap.stanford.edu/data](http://snap.stanford.edu/data).

---

## References

- Kempe, D., Kleinberg, J., & Tardos, É. (2003). *Maximizing the spread of influence through a social network.* KDD.
- Feo, T. A., & Resende, M. G. (1995). *Greedy randomized adaptive search procedures.* Journal of Global Optimization.
- Tarjan, R. (1972). *Depth-first search and linear graph algorithms.* SIAM Journal on Computing.
- Leskovec, J., & Krevl, A. (2014). *SNAP Datasets: Stanford Large Network Dataset Collection.*
- Garey, M. R., & Johnson, D. S. (1979). *Computers and Intractability.*

---

## Authors

- **Elon Pereira Neto** (RA 169205)
- **Kayky Gibran de Oliveira Matos** (RA 173048)

UNICAMP — Computer Science, 2025  
Advisor: Prof. Santiago Valdés Ravelo