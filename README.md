# Unit interval graph recognition

Two C++17 recognizers for unit interval graphs (equivalently, proper
interval graphs):

| implementation | method | complexity |
|---|---|---|
| [`three_sweep_lbfs.cpp`](three_sweep_lbfs.cpp) | Corneil's three-sweep `LBFS+` algorithm | \(O(n+m)\) |
| [`bfs_mcs.cpp`](bfs_mcs.cpp) | BFS + maximum-selected-neighbor greedy (MCS) | \(O((n+m)\log n)\) |

Both programs output an **umbrella ordering** when the answer is
positive and use the same exact certificate verifier from
[`common.hpp`](common.hpp). See [CORRECTNESS.md](CORRECTNESS.md) for
the proofs.

## Input and output

Input:

```text
n m
u1 v1
...
um vm
```

Vertices are numbered from 1 to \(n\). The input must be a simple
undirected graph.

Output:

```text
NO
```

or

```text
YES
v1 v2 ... vn
```

where the second line is an umbrella ordering:

\[
i<j<k,\quad v_iv_k\in E
\quad\Longrightarrow\quad
v_iv_j,v_jv_k\in E.
\]

## Build

```sh
clang++ -std=c++17 -O2 -Wall -Wextra -Wshadow -pedantic \
    three_sweep_lbfs.cpp -o three_sweep_lbfs

clang++ -std=c++17 -O2 -Wall -Wextra -Wshadow -pedantic \
    bfs_mcs.cpp -o bfs_mcs
```

The same commands work with GCC.

## BFS + maximum cardinality search (MCS)

Maximum cardinality search is the standard name introduced by Tarjan
and Yannakakis. Here “cardinality” is simply `score[v]`, the number of
already selected neighbors of \(v\). Meanwhile, `degree[v]` is its
original static degree.

```text
answer_order = []

for every connected component C:
    choose an arbitrary r in C
    dist = BFS(r)

    D = maximum dist[v] over v in C
    s = a minimum-degree vertex among {v in C : dist[v] = D}

    for v in C:
        score[v] = 0
        selected[v] = false

    component_order = []
    x = s

    while |component_order| < |C|:
        append x to component_order
        selected[x] = true

        for y in adj[x]:
            if not selected[y]:
                score[y] += 1

        if |component_order| < |C|:
            x = an unselected vertex maximizing
                (score[x], -degree[x])

    append component_order to answer_order

verify that every closed neighborhood N[v]
is consecutive in answer_order

if verification fails:
    return NO
else:
    return YES and answer_order
```

The vertex \(s\) is forced to be selected first. Any tie remaining
after both score and degree may be resolved arbitrarily. A lazy
priority queue implements the selection step.

## Three-sweep LBFS

The linear implementation computes

\[
\sigma=\operatorname{LBFS}(G),\qquad
\sigma^+=\operatorname{LBFS+}(G,\sigma),\qquad
\sigma^{++}=\operatorname{LBFS+}(G,\sigma^+).
\]

`LBFS+` is ordinary LBFS except that vertices with equal labels are
tied by choosing the vertex occurring last in the preceding ordering.
Stable ordered-partition refinement makes each sweep linear.

## Testing

Both implementations were checked against a brute-force
umbrella-ordering oracle on every labeled simple graph through
\(n=6\), totaling 33,868 graphs.

They were also compared on:

- every connected labeled unit interval graph through \(n=6\), for
  every BFS root, every eligible MCS start, and every legal remaining
  MCS tie;
- 5,000 randomly generated unit interval graphs;
- 10,000 arbitrary random graphs.

All decisions matched. Both implementations were additionally tested
with Apple Clang, GCC 15, AddressSanitizer, and
UndefinedBehaviorSanitizer.

The returned orderings need not be identical. On a connected positive
instance, umbrella orderings may differ by reversal and by permutations
of true twins.

## References

- D. G. Corneil,
  [A simple 3-sweep LBFS algorithm for the recognition of unit interval graphs](https://doi.org/10.1016/j.dam.2003.07.001),
  *Discrete Applied Mathematics* 138 (2004), 371–379.
- Y. Cao,
  [Recognizing (Unit) Interval Graphs by Zigzag Graph Searches](https://arxiv.org/abs/2010.03354),
  2020.
- R. E. Tarjan and M. Yannakakis,
  [Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs](https://doi.org/10.1137/0213035),
  *SIAM Journal on Computing* 13(3), 1984.
