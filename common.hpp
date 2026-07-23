#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

namespace uig {

using Graph = std::vector<std::vector<int>>;
using Order = std::vector<int>;
using Result = std::optional<Order>;
using Recognizer = Result (*)(const Graph&);

inline bool is_umbrella_order(const Graph& graph, const Order& order) {
    const int n = graph.size();
    std::vector<int> position(n);
    for (int i = 0; i < n; ++i) position[order[i]] = i;

    for (int v = 0; v < n; ++v) {
        int left = position[v];
        int right = position[v];
        for (int u : graph[v]) {
            left = std::min(left, position[u]);
            right = std::max(right, position[u]);
        }
        if (right - left != static_cast<int>(graph[v].size())) {
            return false;
        }
    }
    return true;
}

inline int run(Recognizer recognize) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;

    Graph graph(n);
    while (m--) {
        int u, v;
        std::cin >> u >> v;
        --u;
        --v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    Result order = recognize(graph);
    if (!order) {
        std::cout << "NO\n";
        return 0;
    }

    std::cout << "YES\n";
    for (int i = 0; i < n; ++i) {
        if (i) std::cout << ' ';
        std::cout << (*order)[i] + 1;
    }
    std::cout << '\n';
    return 0;
}

}  // namespace uig
