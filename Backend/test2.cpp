// Utility driver to experiment with path finding and distance caching.
#include <iostream>
#include <string>
#include "graph_generator.hpp"
#include "Features/short_path.hpp"

using namespace std;

int main() {
    SocialGraph graph;

    const string base_path = "../dataset/data/generated/2024-01-06";
    const string nodes_path = base_path + "/nodes.json";
    const string edges_path = base_path + "/edges.json";
    const string metadata_path = base_path + "/metadata.json";

    if (!graph.initializeGraph(nodes_path, edges_path, metadata_path)) {
        cerr << "ERROR: Failed to initialize graph" << endl;
        cerr << "Make sure the JSON files exist at:" << endl;
        cerr << "  - " << nodes_path << endl;
        cerr << "  - " << edges_path << endl;
        cerr << "  - " << metadata_path << endl;
        return 1;
    }

    OptimizedDistanceCalculator distance_calc(graph);

    cout << "=== Interactive Path-Finding Test ===" << endl;
    cout << "Enter two user IDs separated by space (Ctrl+D to exit):" << endl;

    int source_id, target_id;
    while (cin >> source_id >> target_id) {
        auto result = distance_calc.find_path(source_id, target_id);
        if (result.path_exists) {
            cout << "Path (" << result.path_length << " hops): "
                 << result.path_description << endl;
        } else {
            cout << "No path found between " << source_id
                 << " and " << target_id << endl;
        }
        cout << endl;
    }

    distance_calc.print_cache_stats();
    cout << "\n=== Session Complete ===" << endl;
    return 0;
}
