#include <algorithm>
#include <iostream>
#include <list>
#include <numeric>
#include <optional>
#include <vector>
using namespace std;

using Graph = vector<vector<int>>;

struct Cell;
using CellList = list<Cell>;

struct Cell {
    list<int> vertices;
    CellList::iterator position;
    Cell* split = nullptr;
};

vector<int> lex_bfs_plus(const Graph& graph, const vector<int>& previous) {
    const int n = graph.size();
    if (n == 0) return {};

    // LBFS+ prefers the last tied vertex in the preceding ordering.
    vector<int> priority(previous.rbegin(), previous.rend());

    // Sorting every neighborhood by that priority makes each cell split stable.
    vector<vector<int>> ordered_neighbors(n);
    for (int v = 0; v < n; ++v) {
        ordered_neighbors[v].reserve(graph[v].size());
    }
    for (int v : priority) {
        for (int u : graph[v]) ordered_neighbors[u].push_back(v);
    }

    CellList cells;
    auto first = cells.emplace(cells.end());
    first->position = first;

    vector<Cell*> owner(n);
    vector<list<int>::iterator> location(n);
    for (int v : priority) {
        first->vertices.push_back(v);
        location[v] = prev(first->vertices.end());
        owner[v] = &*first;
    }

    vector<int> order;
    order.reserve(n);

    while (!cells.empty()) {
        Cell* front = &cells.front();
        int v = front->vertices.front();
        front->vertices.pop_front();
        owner[v] = nullptr;
        order.push_back(v);

        if (front->vertices.empty()) cells.erase(front->position);

        vector<Cell*> refined;
        for (int u : ordered_neighbors[v]) {
            Cell* old = owner[u];
            if (old == nullptr) continue;

            if (old->split == nullptr) {
                auto it = cells.emplace(old->position);
                it->position = it;
                old->split = &*it;
                refined.push_back(old);
            }

            // Updated vertices form a new cell immediately before the old one.
            Cell* next = old->split;
            next->vertices.splice(
                next->vertices.end(), old->vertices, location[u]);
            owner[u] = next;
        }

        for (Cell* old : refined) {
            old->split = nullptr;
            if (old->vertices.empty()) cells.erase(old->position);
        }
    }
    return order;
}

bool has_consecutive_neighborhoods(
    const Graph& graph, const vector<int>& order) {
    const int n = graph.size();
    vector<int> position(n);
    for (int i = 0; i < n; ++i) position[order[i]] = i;

    for (int v = 0; v < n; ++v) {
        int left = position[v];
        int right = position[v];
        for (int u : graph[v]) {
            left = min(left, position[u]);
            right = max(right, position[u]);
        }
        if (right - left != static_cast<int>(graph[v].size())) return false;
    }
    return true;
}

optional<vector<int>> unit_interval_order(const Graph& graph) {
    vector<int> order(graph.size());
    iota(order.begin(), order.end(), 0);

    // The first result is an arbitrary LBFS ordering; the next two are LBFS+.
    for (int sweep = 0; sweep < 3; ++sweep) {
        order = lex_bfs_plus(graph, order);
    }

    if (!has_consecutive_neighborhoods(graph, order)) return nullopt;
    return order;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    Graph graph(n);
    while (m--) {
        int u, v;
        cin >> u >> v;
        --u;
        --v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    auto order = unit_interval_order(graph);
    if (!order) {
        cout << "NO\n";
        return 0;
    }

    cout << "YES\n";
    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << (*order)[i] + 1;
    }
    cout << '\n';
    return 0;
}
