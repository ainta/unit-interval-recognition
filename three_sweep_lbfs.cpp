#include "common.hpp"

#include <iterator>
#include <list>
#include <numeric>

namespace uig {

struct Cell;
using CellList = std::list<Cell>;

struct Cell {
    std::list<int> vertices;
    CellList::iterator position;
    Cell* split = nullptr;
};

Order lex_bfs_plus(const Graph& graph, const Order& previous) {
    const int n = graph.size();
    if (n == 0) return {};

    // LBFS+ prefers the last tied vertex in the preceding ordering.
    Order priority(previous.rbegin(), previous.rend());

    // Sorting every neighborhood by that priority makes each cell split stable.
    Graph ordered_neighbors(n);
    for (int v = 0; v < n; ++v) {
        ordered_neighbors[v].reserve(graph[v].size());
    }
    for (int v : priority) {
        for (int u : graph[v]) ordered_neighbors[u].push_back(v);
    }

    CellList cells;
    auto first = cells.emplace(cells.end());
    first->position = first;

    std::vector<Cell*> owner(n);
    std::vector<std::list<int>::iterator> location(n);
    for (int v : priority) {
        first->vertices.push_back(v);
        location[v] = std::prev(first->vertices.end());
        owner[v] = &*first;
    }

    Order order;
    order.reserve(n);

    while (!cells.empty()) {
        Cell* front = &cells.front();
        int v = front->vertices.front();
        front->vertices.pop_front();
        owner[v] = nullptr;
        order.push_back(v);

        if (front->vertices.empty()) cells.erase(front->position);

        std::vector<Cell*> refined;
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

Result recognize_with_three_sweep_lbfs(const Graph& graph) {
    Order order(graph.size());
    std::iota(order.begin(), order.end(), 0);

    // The first result is an arbitrary LBFS ordering; the next two are LBFS+.
    for (int sweep = 0; sweep < 3; ++sweep) {
        order = lex_bfs_plus(graph, order);
    }

    if (!is_umbrella_order(graph, order)) return std::nullopt;
    return order;
}

}  // namespace uig

int main() {
    return uig::run(uig::recognize_with_three_sweep_lbfs);
}
