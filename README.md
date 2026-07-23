# Unit interval graph recognition

A linear-time C++17 implementation of Corneil's three-sweep LBFS
recognition algorithm for unit interval graphs (equivalently, proper
interval graphs).

If the graph is a unit interval graph, the program also prints an
**umbrella ordering** \(v_1,\ldots,v_n\):

\[
i<j<k,\quad v_iv_k\in E
\quad\Longrightarrow\quad
v_iv_j,v_jv_k\in E.
\]

## Algorithm

1. Run an arbitrary LBFS and obtain \(\sigma\).
2. Run `LBFS+` with respect to \(\sigma\) and obtain \(\sigma^+\).
3. Run `LBFS+` with respect to \(\sigma^+\) and obtain \(\sigma^{++}\).
4. Accept exactly when every closed neighborhood is consecutive in
   \(\sigma^{++}\).

Among vertices tied by LBFS, `LBFS+` selects the vertex occurring last
in the preceding ordering.

Each sweep uses stable ordered-partition refinement. Before a sweep,
the adjacency lists are rebuilt in the required tie-breaking order.
Consequently, splitting a cell into neighbors and non-neighbors
preserves that order without sorting individual cells.

For the final check, let \(L_v\) and \(R_v\) be the first and last
positions occupied by \(N[v]\). Then \(N[v]\) is consecutive exactly
when

\[
R_v-L_v=\deg(v).
\]

The total time and memory complexity are both \(O(n+m)\).

Disconnected graphs are handled directly: an LBFS finishes its current
component before entering another one. The input must be a simple
undirected graph.

## Input and output

Input:

```text
n m
u1 v1
...
um vm
```

Vertices are numbered from 1 to \(n\).

Output:

```text
NO
```

or

```text
YES
v1 v2 ... vn
```

where the second line is a valid umbrella ordering.

## Build

Apple Clang:

```sh
clang++ -std=c++17 -O2 -Wall -Wextra -Wshadow -pedantic main.cpp -o unit_interval
```

GCC:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Wshadow -pedantic main.cpp -o unit_interval
```

## The BFS + MCS alternative

The following simpler \(O((n+m)\log n)\) contest implementation is also
correct when applied to each connected component:

1. Run ordinary BFS from any vertex.
2. In the last BFS level, choose a vertex \(s\) of minimum original
   degree.
3. Force MCS to start at \(s\). At every later step, maximize the
   number of already selected neighbors and then minimize the original
   degree. Remaining ties are arbitrary.
4. Apply the same consecutive-neighborhood check.

This is not an LBFS algorithm. It follows from Lemma 3.7 and Corollary
3.10 of Cao's paper: the BFS rule finds an end vertex, and
\(\mathrm{MCS}_\delta\) starting at an end vertex produces an umbrella
ordering. A lazy priority queue gives the stated complexity.

## Testing

The implementation was checked against a brute-force permutation
oracle on every labeled simple graph through \(n=6\), totaling 33,868
graphs.

The LBFS and BFS + MCS recognizers were also compared on:

- every connected labeled unit interval graph through \(n=6\), for
  every BFS root, every eligible starting vertex, and every legal MCS
  tie choice;
- 5,000 randomly generated unit interval graphs;
- 10,000 arbitrary random graphs.

All decisions matched. The code was additionally tested with Clang,
GCC 15, AddressSanitizer, and UndefinedBehaviorSanitizer.

## References

- D. G. Corneil,
  [A simple 3-sweep LBFS algorithm for the recognition of unit interval graphs](https://doi.org/10.1016/j.dam.2003.07.001),
  *Discrete Applied Mathematics* 138 (2004), 371–379.
- Y. Cao,
  [Recognizing (Unit) Interval Graphs by Zigzag Graph Searches](https://arxiv.org/abs/2010.03354),
  2020.
