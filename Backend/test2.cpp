

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include "graphgen.hpp"
#include "short_path.hpp"


using namespace std;


int main(){
    // Create calculator once
    SocialGraph graph;
    
    // FIX #1 & #2: Check return value and use appropriate paths
    string nodes_path = "your relvative path to 2024-01-06/nodes.json";
    string edges_path = "your relvative path to 2024-01-06/edges.json";
    string metadata_path = "your relvative path to 2024-01-06/metadata.json";
    
    if (!graph.initializeGraph(nodes_path, edges_path, metadata_path)) {
        cerr << "ERROR: Failed to initialize graph" << endl;
        cerr << "Make sure the JSON files exist at:" << endl;
        cerr << "  - " << nodes_path << endl;
        cerr << "  - " << edges_path << endl;
        cerr << "  - " << metadata_path << endl;
        return 1;
    }
    
    OptimizedDistanceCalculator distance_calc(graph);
    
    // FIX #3: Validate node IDs before querying
    vector<int> node_ids = {1, 2, 3, 4, 6, 17, 98, 76};
    
    cout << "=== Testing PathFindResult with Real Graph ===" << endl << endl;
    
    // First query: bidirectional BFS (~6x faster)
    cout << "Query 1: Finding path from 1 to 130" << endl;
    int n,m;
    cin >> n >> m;
    PathFindResult result1 = distance_calc.find_path(n, m);
    if (result1.path_exists) {
        cout << "  Path exists: YES" << endl;
        cout << "  Path length (hops): " << result1.path_length << endl;
        cout << "  Path description: " << result1.path_description << endl;
        cout << "  Node count: " << result1.path_node_ids.size() << endl;
    } else {
        cout << "  Path exists: NO" << endl;
    }
    cout << endl;
    
    // Second query: from cache (~50x faster)
    //cout << "Query 2: Finding path from 111 to 923" << endl;
    cin >> n >> m;
    PathFindResult result2 = distance_calc.find_path(n, m);
    if (result2.path_exists) {
        cout << "  Path exists: YES" << endl;
        cout << "  Path length (hops): " << result2.path_length << endl;
        cout << "  Path description: " << result2.path_description << endl;
        cout << "  Node count: " << result2.path_node_ids.size() << endl;
    } else {
        cout << "  Path exists: NO" << endl;
        cout << "  Reason: Nodes are either not in graph or in different components" << endl;
    }
    cout << endl;
    
    // // Batch queries with paths
    // cout << "Query 3: Batch finding paths from 1 to [17, 98, 999]" << endl;
    // vector<PathFindResult> batch_results = distance_calc.find_paths_batch(1, {17, 98, 999});
    // for (size_t i = 0; i < batch_results.size(); i++) {
    //     int target = (i == 0) ? 17 : (i == 1) ? 98 : 999;
    //     cout << "  Target " << target << ":" << endl;
    //     if (batch_results[i].path_exists) {
    //         cout << "    Path exists: YES" << endl;
    //         cout << "    Path length: " << batch_results[i].path_length << endl;
    //         cout << "    Path: " << batch_results[i].path_description << endl;
    //     } else {
    //         cout << "    Path exists: NO" << endl;
    //     }
    // }
    // cout << endl;
    
    // // Legacy: Get only distances (backward compatibility)
    // cout << "Query 4: Legacy - Getting distances only from 1 to [17, 98, 999]" << endl;
    // vector<int> distances = distance_calc.calculate_distances_batch(1, {17, 98, 999});
    // cout << "  Distances from 1: ";
    // for (size_t i = 0; i < distances.size(); i++) {
    //     if (i > 0) cout << ", ";
    //     if (distances[i] == -1) {
    //         cout << "NO_PATH";
    //     } else {
    //         cout << distances[i];
    //     }
    // }
    // cout << endl << endl;
    
    // View cache stats
    distance_calc.print_cache_stats();
    
    cout << "\n=== All Tests Complete ===" << endl;
    
    return 0;
}