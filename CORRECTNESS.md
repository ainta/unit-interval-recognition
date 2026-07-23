# Correctness proofs

The proofs below are for finite simple undirected graphs. We first
prove the shared certificate verifier, then prove completeness of each
search algorithm.

## 1. Umbrella orderings and the verifier

An ordering \(v_1,\ldots,v_n\) is an **umbrella ordering** if

\[
i<j<k,\quad v_iv_k\in E
\quad\Longrightarrow\quad
v_iv_j\in E\ \text{and}\ v_jv_k\in E.
\]

A classical characterization says that a graph is a unit interval
graph if and only if it has an umbrella ordering.

### Lemma 1

An ordering is an umbrella ordering if and only if every closed
neighborhood \(N[v]\) is consecutive in the ordering.

### Proof

Suppose first that every \(N[v]\) is consecutive. If \(i<j<k\) and
\(v_iv_k\in E\), then \(v_i,v_k\in N[v_i]\). Therefore the vertex
between them, \(v_j\), also belongs to \(N[v_i]\), so
\(v_iv_j\in E\). The same argument applied to \(N[v_k]\) gives
\(v_jv_k\in E\).

Conversely, suppose the umbrella condition holds. Fix \(v_p\), and let

\[
L=\min\{i:v_i\in N[v_p]\},\qquad
R=\max\{i:v_i\in N[v_p]\}.
\]

We have \(L\le p\le R\). For \(L<j<p\), the edge \(v_Lv_p\) and the
umbrella condition imply \(v_jv_p\in E\). Similarly, for
\(p<j<R\), the edge \(v_pv_R\) implies \(v_pv_j\in E\). Thus every
position from \(L\) through \(R\) belongs to \(N[v_p]\). ∎

### Exact verifier

For each vertex \(v\), `is_umbrella_order` computes the extreme
positions \(L_v,R_v\) occupied by \(N[v]\). Since
\(|N[v]|=\deg(v)+1\), this neighborhood fills its entire span exactly
when

\[
R_v-L_v+1=\deg(v)+1,
\]

or equivalently,

\[
R_v-L_v=\deg(v).
\]

By Lemma 1, the verifier accepts exactly the umbrella orderings.
Consequently, neither search algorithm can accept a non-unit-interval
graph, regardless of the candidate permutation it constructs.

## 2. Three-sweep LBFS

For a preceding ordering \(\rho\), `LBFS+` performs an LBFS and resolves
equal-label ties by choosing the vertex occurring last in \(\rho\).
The algorithm computes

\[
\tau=\operatorname{LBFS}(G),\qquad
\sigma=\operatorname{LBFS+}(G,\tau),\qquad
\eta=\operatorname{LBFS+}(G,\sigma).
\]

We prove that \(\eta\) is an umbrella ordering whenever a connected
input graph \(G\) is a unit interval graph.

### Structural theorem

The following is the nontrivial graph-search result behind the
algorithm.

**Theorem (Cao, Theorem 3.6).** Let \(G\) be a connected unit interval
graph, and let \(\rho\) be an LBFS ordering that starts at an LBFS end
vertex. Then

\[
\operatorname{LBFS+}(G,\rho)
\]

is an umbrella ordering.

Cao proves this through a clique path \(K_1,\ldots,K_q\). An LBFS from
one end progresses through the clique path in order and determines the
first clique containing each vertex. The `LBFS+` sweep starts at the
other end; its reversed tie-breaking supplies the missing order among
vertices that begin in the same clique. The resulting order is the
canonical umbrella order, up to permutations of true twins.

The full argument is Cao's Lemma 3.5 followed by Theorem 3.6. We use
that theorem explicitly instead of abbreviating its clique-path proof
into an incomplete case analysis.

### Completeness proof

The last vertex \(s\) of \(\tau\) is an LBFS end vertex by definition.
Because all labels are initially equal, `LBFS+` chooses the vertex
occurring last in the preceding ordering first. Therefore
\(\sigma=\operatorname{LBFS+}(G,\tau)\) is an LBFS ordering starting
at \(s\).

Applying Cao's theorem to \(\sigma\) shows that

\[
\eta=\operatorname{LBFS+}(G,\sigma)
\]

is an umbrella ordering. The verifier accepts it. Conversely, the
exact verifier cannot accept a non-unit-interval graph. Hence the
three-sweep algorithm accepts exactly the unit interval graphs.

### Why the code implements `LBFS+`

During a sweep, the ordered partition maintains:

1. vertices in one cell have identical current LBFS labels;
2. cells occur in decreasing lexicographic-label order;
3. vertices inside a cell occur in reverse order of the preceding
   sweep.

Thus the first vertex of the first cell is precisely the vertex
prescribed by `LBFS+`.

After selecting a pivot, every unselected neighbor receives the same
new label entry. In each old cell, these neighbors form a new cell
immediately before the non-neighbors. `ordered_neighbors` lists them
in the required tie-breaking order, so this split is stable and all
three invariants are preserved.

The first call uses the natural vertex order only to resolve otherwise
arbitrary LBFS ties, so it is a valid arbitrary first LBFS. The next
two calls use the preceding sweep and are exactly the required
`LBFS+` sweeps.

Every directed edge incidence is processed a constant number of times,
and every list operation is constant-time. One sweep, and therefore
the full algorithm, takes \(O(n+m)\) time and memory.

### Disconnected graphs

Once LBFS enters a component, some unvisited vertex in that component
has a nonempty label until the component is exhausted. Untouched
components still have empty labels. Hence every component is a
contiguous block in every sweep, and the restriction of the global
sweeps to each block is the connected algorithm above.

Concatenating component umbrella orderings remains an umbrella
ordering because there are no edges between different blocks.

## 3. BFS followed by degree-tied MCS

For one connected component, the second recognizer does the following:

1. Run ordinary BFS from any root.
2. In the last BFS level, choose a vertex \(s\) of minimum original
   degree.
3. Force MCS to start at \(s\). Afterwards maximize the number of
   selected neighbors and, among ties, minimize original degree.
4. Run the exact umbrella-order verifier.

We now give a direct completeness proof.

### Monotone neighborhood boundaries

Fix any umbrella ordering \(v_1,\ldots,v_n\), and write

\[
\ell_i=\min\{j:v_j\in N[v_i]\},\qquad
r_i=\max\{j:v_j\in N[v_i]\}.
\]

By Lemma 1,

\[
N[v_i]=\{v_{\ell_i},v_{\ell_i+1},\ldots,v_{r_i}\}.
\]

Both boundary sequences are nondecreasing. For example, if \(i<j\)
but \(\ell_i>\ell_j\), then
\(\ell_j<i<j\). The edge \(v_{\ell_j}v_j\) would force
\(v_{\ell_j}v_i\), contradicting the definition of \(\ell_i\).
The proof for \(r_i\le r_j\) is symmetric. Also,

\[
\deg(v_i)=r_i-\ell_i.
\]

Equal pairs \((\ell_i,r_i)\) are exactly true twins.

### Why BFS finds an endpoint

Every BFS ball is an interval of positions in the umbrella ordering.
Indeed, if the current ball is \([a,b]\), then its closed neighborhood
is

\[
[\ell_a,r_b].
\]

This follows from monotonicity of the boundaries and neighborhood
consecutiveness, so induction from the one-vertex initial ball proves
the claim.

The singleton component is immediate, so suppose the BFS eccentricity
is positive. Let it be \(D\), and suppose the radius-\(D-1\) ball is
\([a,b]\). The last BFS level is

\[
[1,a-1]\ \cup\ [b+1,n],
\]

where either part may be empty.

If the left part is nonempty, the next ball covers the whole connected
component, so \(\ell_a=1\). Therefore \([1,a]\) is a clique, every
\(i<a\) has \(\ell_i=1\), and monotonicity gives

\[
\deg(v_i)=r_i-1\ge r_1-1=\deg(v_1).
\]

Equality is possible only when \(v_i\) is a true twin of \(v_1\).
Symmetrically, every minimum-degree vertex in the right part is a true
twin of \(v_n\). A minimum-degree vertex in the entire last level is
therefore a twin of one of the two endpoints.

After reversing the umbrella order and exchanging true twins if
necessary, we may assume that the chosen vertex is \(s=v_1\).

### Why MCS chooses the umbrella order

Suppose inductively that MCS has selected
\(v_1,\ldots,v_t\). If true twins were selected in another order,
exchange those twins in our fixed umbrella ordering; this changes no
adjacency and preserves the induction hypothesis.

For every unselected \(v_j\),

\[
\operatorname{score}(v_j)
=|N(v_j)\cap\{v_1,\ldots,v_t\}|
=\max(0,t-\ell_j+1).
\]

Because the component is connected and \(t<n\), an edge crosses the
selected prefix, so the maximum score is positive. Since the
\(\ell_j\) are nondecreasing, maximizing score is exactly minimizing
\(\ell_j\).

Among vertices with the same \(\ell_j\), minimizing the original
degree

\[
\deg(v_j)=r_j-\ell_j
\]

is exactly minimizing \(r_j\). Hence MCS with the degree tie-break
chooses \(v_{t+1}\). If both boundaries tie, the candidates are true
twins, so any remaining tie choice is harmless.

Induction proves that the constructed order is an umbrella ordering.
Therefore every unit interval component is accepted. The exact
verifier supplies the converse, so the algorithm is a correct
recognizer.

This direct argument proves the content used from Cao's Lemma 3.7 and
Corollary 3.10.

### Components and complexity

The implementation runs BFS and MCS separately in every connected
component and concatenates the resulting orders. Global degree equals
component degree because there are no cross-component edges.
Concatenation is valid for the same reason as in the LBFS proof.

The lazy heap key is

```text
(score, -original_degree, -vertex_id).
```

Every vertex contributes one initial heap entry. Each edge increases
one endpoint's score exactly once, causing one more entry. Stale
entries are discarded when popped. Thus the running time is
\(O((n+m)\log n)\) and memory usage is \(O(n+m)\).

## References

- D. G. Corneil,
  [A simple 3-sweep LBFS algorithm for the recognition of unit interval graphs](https://doi.org/10.1016/j.dam.2003.07.001),
  *Discrete Applied Mathematics* 138 (2004), 371–379.
- Y. Cao,
  [Recognizing (Unit) Interval Graphs by Zigzag Graph Searches](https://arxiv.org/abs/2010.03354),
  especially Section 3.
