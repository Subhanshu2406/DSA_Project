#pragma once

#include "graph_generator.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <set>


using namespace std;


struct PathFindResult {
    bool path_exists;           // Whether connection found
    vector<int> path_node_ids;  // IDs of users in the path (source to target)
    int path_length;            // Number of hops (edges)
    string path_description;    // Human readable: "A -> B -> C"
    
    // Constructor to initialize empty result (no path found)
    PathFindResult() 
        : path_exists(false), path_node_ids({}), path_length(-1), path_description("") {}
    
    // Constructor to initialize with values
    PathFindResult(bool exists, vector<int> nodes, int length, string description)
        : path_exists(exists), path_node_ids(nodes), path_length(length), path_description(description) {}
};


class OptimizedDistanceCalculator {
private:
    const SocialGraph& graph;
    unordered_map<long long, int> cache;
    unordered_map<long long, PathFindResult> result_cache;


    // FIX #5: Safer encoding that avoids overflow
    long long encode_pair(int source, int target) const {
        if (source > target) swap(source, target);
        // Safe encoding for node IDs up to 1 billion
        return (long long)source * 1000000000LL + target;
    }


    // Helper function to create success result
    PathFindResult create_success_result(const vector<int>& path_nodes) const {
        string description = "";
        for (int i = 0; i < path_nodes.size(); i++) {
            if (i > 0) description += " -> ";
            description += to_string(path_nodes[i]);
        }
        int hops = path_nodes.size() - 1;
        return PathFindResult(true, path_nodes, hops, description);
    }


    // Helper function to create failure result
    PathFindResult create_failure_result() const {
        return PathFindResult(false, vector<int>{}, -1, "");
    }


    // Helper to reconstruct path from meeting node
    vector<int> reconstruct_path(int meeting_node, 
                                 const unordered_map<int, int>& parent_src,
                                 const unordered_map<int, int>& parent_tgt) const {
        // Path from source to meeting node
        vector<int> path_forward;
        int current = meeting_node;
        while (current != -1) {
            path_forward.push_back(current);
            auto it = parent_src.find(current);
            if (it != parent_src.end()) {
                current = it->second;
            } else {
                break;
            }
        }
        reverse(path_forward.begin(), path_forward.end());

        // Path from meeting node to target
        vector<int> path_backward;
        current = meeting_node;
        while (current != -1) {
            path_backward.push_back(current);
            auto it = parent_tgt.find(current);
            if (it != parent_tgt.end()) {
                current = it->second;
            } else {
                break;
            }
        }

        // Combine paths (avoid duplicating meeting node)
        vector<int> full_path = path_forward;
        for (size_t i = 1; i < path_backward.size(); i++) {
            full_path.push_back(path_backward[i]);
        }

        return full_path;
    }


    // Core bidirectional BFS with path reconstruction
    PathFindResult bidirectional_bfs(int source_id, int target_id) const {
        if (source_id == target_id) {
            return create_success_result(vector<int>{source_id});
        }
        
        unordered_map<int, int> dist_src;
        unordered_map<int, int> dist_tgt;
        unordered_map<int, int> parent_src;
        unordered_map<int, int> parent_tgt;
        queue<int> q_src, q_tgt;
        
        q_src.push(source_id);
        q_tgt.push(target_id);
        dist_src[source_id] = 0;
        dist_tgt[target_id] = 0;
        parent_src[source_id] = -1;
        parent_tgt[target_id] = -1;

        int meeting_node = -1;
        bool found = false;

        while ((!q_src.empty() || !q_tgt.empty()) && !found) {
            // Forward direction - process one level
            if (!q_src.empty()) {
                int level_size = q_src.size();
                for (int i = 0; i < level_size && !found; i++) {
                    int u = q_src.front();
                    q_src.pop();
                    
                    set<int> outgoing = graph.getFollowing(u);
                    for (int v : outgoing) {
                        // Check if reached from other direction
                        if (dist_tgt.count(v)) {
                            if (!parent_src.count(v)) {
                                parent_src[v] = u;
                                dist_src[v] = dist_src[u] + 1;
                            }
                            meeting_node = v;
                            found = true;
                            break;
                        }
                        
                        // Mark as visited
                        if (!dist_src.count(v)) {
                            dist_src[v] = dist_src[u] + 1;
                            parent_src[v] = u;
                            q_src.push(v);
                        }
                    }
                }
            }

            if (found) break;

            // Backward direction - process one level
            if (!q_tgt.empty()) {
                int level_size = q_tgt.size();
                for (int i = 0; i < level_size && !found; i++) {
                    int u = q_tgt.front();
                    q_tgt.pop();
                    
                    set<int> outgoing = graph.getFollowing(u);
                    for (int v : outgoing) {
                        // Check if reached from other direction
                        if (dist_src.count(v)) {
                            if (!parent_tgt.count(v)) {
                                parent_tgt[v] = u;
                                dist_tgt[v] = dist_tgt[u] + 1;
                            }
                            meeting_node = v;
                            found = true;
                            break;
                        }
                        
                        // Mark as visited
                        if (!dist_tgt.count(v)) {
                            dist_tgt[v] = dist_tgt[u] + 1;
                            parent_tgt[v] = u;
                            q_tgt.push(v);
                        }
                    }
                }
            }
        }
        
        // No path found
        if (!found) {
            return create_failure_result();
        }

        // Reconstruct and return path
        vector<int> full_path = reconstruct_path(meeting_node, parent_src, parent_tgt);
        return create_success_result(full_path);
    }


    // Simple unidirectional BFS as fallback (guaranteed to find target if it exists)
    PathFindResult simple_bfs(int source_id, int target_id) const {
        if (source_id == target_id) {
            return create_success_result(vector<int>{source_id});
        }

        unordered_map<int, int> dist;
        unordered_map<int, int> parent;
        queue<int> q;

        q.push(source_id);
        dist[source_id] = 0;
        parent[source_id] = -1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            // Found target
            if (u == target_id) {
                // Reconstruct path
                vector<int> path;
                int current = target_id;
                while (current != -1) {
                    path.push_back(current);
                    current = parent[current];
                }
                reverse(path.begin(), path.end());
                return create_success_result(path);
            }

            set<int> outgoing = graph.getFollowing(u);
            for (int v : outgoing) {
                if (!dist.count(v)) {
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        // No path found
        return create_failure_result();
    }

    PathFindResult compute_path_internal(int source_id, int target_id) const {
        PathFindResult result = bidirectional_bfs(source_id, target_id);
        if (!result.path_exists) {
            result = simple_bfs(source_id, target_id);
        }
        return result;
    }


public:
    OptimizedDistanceCalculator(const SocialGraph& g) : graph(g) {}


    // Main function: Bidirectional BFS + Caching (returns PathFindResult)
    PathFindResult find_path(int source_id, int target_id) {
        // Normalize for cache
        int norm_src = source_id;
        int norm_tgt = target_id;
        if (norm_src > norm_tgt) swap(norm_src, norm_tgt);
        long long key = encode_pair(norm_src, norm_tgt);


        // Check cache first
        if (result_cache.count(key)) {
            return result_cache[key];
        }


        // Use bidirectional BFS with fallback to guarantee correctness
        PathFindResult result = compute_path_internal(source_id, target_id);
        
        // Store in cache
        result_cache[key] = result;
        if (result.path_exists) {
            cache[key] = result.path_length;
        }
        return result;
    }


    // Legacy function: Get shortest distance only (for backward compatibility)
    int calculate_shortest_distance(int source_id, int target_id) {
        if (source_id == target_id) return 0;
        
        // Normalize for cache
        int norm_src = source_id;
        int norm_tgt = target_id;
        if (norm_src > norm_tgt) swap(norm_src, norm_tgt);
        long long key = encode_pair(norm_src, norm_tgt);


        // Check cache first
        if (cache.count(key)) {
            return cache[key];
        }


        PathFindResult result = compute_path_internal(source_id, target_id);
        
        result_cache[key] = result;
        cache[key] = result.path_exists ? result.path_length : -1;
        return result.path_length;
    }


    // Batch find paths
    vector<PathFindResult> find_paths_batch(int source_id, const vector<int>& target_ids) {
        vector<PathFindResult> results;
        results.reserve(target_ids.size());
        for (int target_id : target_ids) {
            results.push_back(find_path(source_id, target_id));
        }
        return results;
    }


    // Batch calculate distances (original, returns only distances)
    vector<int> calculate_distances_batch(int source_id, const vector<int>& target_ids) {
        vector<int> results;
        results.reserve(target_ids.size());
        for (int target_id : target_ids) {
            results.push_back(calculate_shortest_distance(source_id, target_id));
        }
        return results;
    }


    // Cache management
    int get_cache_size() const {
        return result_cache.size();
    }


    void clear_cache() {
        cache.clear();
        result_cache.clear();
    }


    void print_cache_stats() const {
        cout << "\n=== Cache Statistics ===" << endl;
        cout << "Cached queries: " << result_cache.size() << endl;
        cout << "Memory used: ~" << (result_cache.size() * 64) / 1024 << " KB" << endl;
    }
};
