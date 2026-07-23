#include "common.hpp"

#include <queue>
#include <tuple>

namespace uig {

Order maximum_cardinality_search(
    const Graph& graph, const Order& component, int start,
    std::vector<int>& score, std::vector<bool>& selected) {
    using Entry = std::tuple<int, int, int>;
    std::priority_queue<Entry> candidates;
    for (int v : component) {
        candidates.emplace(0, -static_cast<int>(graph[v].size()), -v);
    }

    Order order;
    order.reserve(component.size());
    int current = start;

    while (true) {
        selected[current] = true;
        order.push_back(current);
        for (int u : graph[current]) {
            if (selected[u]) continue;
            ++score[u];
            candidates.emplace(
                score[u], -static_cast<int>(graph[u].size()), -u);
        }

        if (order.size() == component.size()) break;

        while (true) {
            Entry entry = candidates.top();
            candidates.pop();
            int v = -std::get<2>(entry);
            if (!selected[v] && std::get<0>(entry) == score[v]) {
                current = v;
                break;
            }
        }
    }
    return order;
}

Result recognize_with_bfs_mcs(const Graph& graph) {
    const int n = graph.size();
    std::vector<int> distance(n, -1);
    std::vector<int> score(n);
    std::vector<bool> selected(n);
    Order order;
    order.reserve(n);

    for (int root = 0; root < n; ++root) {
        if (distance[root] != -1) continue;

        std::queue<int> queue;
        Order component;
        queue.push(root);
        distance[root] = 0;

        while (!queue.empty()) {
            int v = queue.front();
            queue.pop();
            component.push_back(v);
            for (int u : graph[v]) {
                if (distance[u] == -1) {
                    distance[u] = distance[v] + 1;
                    queue.push(u);
                }
            }
        }

        int start = component.front();
        for (int v : component) {
            if (distance[v] > distance[start] ||
                (distance[v] == distance[start] &&
                 graph[v].size() < graph[start].size())) {
                start = v;
            }
        }

        Order part = maximum_cardinality_search(
            graph, component, start, score, selected);
        order.insert(order.end(), part.begin(), part.end());
    }

    if (!is_umbrella_order(graph, order)) return std::nullopt;
    return order;
}

}  // namespace uig

int main() {
    return uig::run(uig::recognize_with_bfs_mcs);
}
