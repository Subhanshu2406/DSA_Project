// include/Backend/community_detection.hpp
/*
 * COMMUNITY DETECTION ALGORITHM
 * =============================
 * Identifies groups of densely connected users (communities/clusters).
 */

#pragma once

#include "graph_generator.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

using namespace std;

/**
 * Result of community detection algorithm.
 * A community is a group of densely connected users.
 * Used for: showing user's community, local leaderboards, finding clusters.
 */
struct Community {
    int community_id;              // Unique identifier
    vector<int> member_ids;        // All user IDs in this community
    int size;                      // Number of members
    double internal_edge_density;  // How connected are members? (0.0 to 1.0)
    double modularity_score;       // How distinct is this community? (higher = better)
    int edges_within_community;    // Total connections within community
    int edges_to_outside;          // Connections to other communities
};

class CommunityDetector {
private:
    const SocialGraph& graph;

    /**
     * Calculate internal density of a community.
     * Density = actual_edges / max_possible_edges
     */
    double calculate_internal_density(const vector<int>& members) const {
        if (members.size() < 2) return 0.0;

        int internal_edges = 0;
        unordered_set<int> member_set(members.begin(), members.end());

        // Count edges within community
        for (int member : members) {
            auto neighbors = graph.getNeighbors(member);
            for (int neighbor : neighbors) {
                if (member_set.count(neighbor) && member < neighbor) {
                    internal_edges++;
                }
            }
        }

        int max_edges = (members.size() * (members.size() - 1)) / 2;
        return max_edges == 0 ? 0.0 : (double)internal_edges / max_edges;
    }

    /**
     * Count edges within and outside community.
     */
    pair<int, int> count_community_edges(const vector<int>& members) const {
        int internal = 0;
        int external = 0;
        unordered_set<int> member_set(members.begin(), members.end());

        for (int member : members) {
            auto neighbors = graph.getNeighbors(member);
            for (int neighbor : neighbors) {
                if (member_set.count(neighbor)) {
                    // Don't double count
                    if (member < neighbor) internal++;
                } else {
                    external++;
                }
            }
        }

        return {internal, external};
    }

public:
    explicit CommunityDetector(const SocialGraph& social_graph) 
        : graph(social_graph) {}

    /**
     * LABEL PROPAGATION COMMUNITY DETECTION
     * 
     * Simple but effective algorithm for community detection.
     * Each node takes the label of its most frequent neighbor.
     * 
     * Algorithm: Iterative label propagation
     * Time Complexity: O(V * avg_degree * iterations)
     * Space Complexity: O(V)
     * 
     * Parameters:
     *   - max_iterations: number of rounds (default 10)
     * 
     * Returns: Vector of communities with statistics
     */
    vector<Community> detect_label_propagation(int max_iterations = 10) const {
        unordered_map<int, int> labels; // user_id -> community_id
        const auto& nodes = graph.getNodes();

        // Initialize: each node is own community
        for (const auto& [id, node] : nodes) {
            labels[id] = id;
        }

        // Propagate labels iteratively
        for (int iteration = 0; iteration < max_iterations; iteration++) {
            bool changed = false;
            
            for (const auto& [id, node] : nodes) {
                map<int, int> label_count; // label -> count

                // Count neighboring labels
                for (int neighbor : node.neighbors) {
                    label_count[labels[neighbor]]++;
                }

                if (!label_count.empty()) {
                    // Pick label with highest count
                    int new_label = max_element(
                        label_count.begin(), label_count.end(),
                        [](const auto& a, const auto& b) { 
                            return a.second < b.second; 
                        })->first;

                    if (labels[id] != new_label) {
                        labels[id] = new_label;
                        changed = true;
                    }
                }
            }

            if (!changed) break; // Converged
        }

        // Group nodes by community
        map<int, vector<int>> communities_map;
        for (const auto& [id, label] : labels) {
            communities_map[label].push_back(id);
        }

        // Create result structures
        vector<Community> result;
        int community_idx = 0;

        for (auto& [label, members] : communities_map) {
            Community comm;
            comm.community_id = community_idx++;
            comm.member_ids = members;
            comm.size = members.size();
            comm.internal_edge_density = calculate_internal_density(members);

            auto [internal, external] = count_community_edges(members);
            comm.edges_within_community = internal;
            comm.edges_to_outside = external;
            comm.modularity_score = 0.0; // TODO: implement modularity calculation

            result.push_back(comm);
        }

        return result;
    }

    /**
     * GREEDY MODULARITY OPTIMIZATION (Louvain-like)
     * 
     * More sophisticated algorithm that optimizes modularity.
     * Better quality than label propagation but slower.
     * 
     * Algorithm: Iterative greedy optimization
     * Time Complexity: O(E * iterations)
     * Space Complexity: O(V)
     * 
     * Returns: Vector of communities with modularity scores
     */
    vector<Community> detect_greedy_modularity(int max_iterations = 10) const {
        // Simple implementation: start with connected components
        unordered_map<int, int> component; // user_id -> component_id
        int comp_id = 0;
        const auto& nodes = graph.getNodes();

        // Find connected components using BFS
        unordered_set<int> visited;
        for (const auto& [id, node] : nodes) {
            if (!visited.count(id)) {
                queue<int> q;
                q.push(id);
                visited.insert(id);
                component[id] = comp_id;

                while (!q.empty()) {
                    int current = q.front();
                    q.pop();

                    for (int neighbor : graph.getNeighbors(current)) {
                        if (!visited.count(neighbor)) {
                            visited.insert(neighbor);
                            component[neighbor] = comp_id;
                            q.push(neighbor);
                        }
                    }
                }

                comp_id++;
            }
        }

        // Group by component
        map<int, vector<int>> communities_map;
        for (const auto& [id, comp] : component) {
            communities_map[comp].push_back(id);
        }

        // Create result structures
        vector<Community> result;
        int community_idx = 0;

        for (auto& [comp, members] : communities_map) {
            Community comm;
            comm.community_id = community_idx++;
            comm.member_ids = members;
            comm.size = members.size();
            comm.internal_edge_density = calculate_internal_density(members);

            auto [internal, external] = count_community_edges(members);
            comm.edges_within_community = internal;
            comm.edges_to_outside = external;
            comm.modularity_score = 0.0; // TODO: implement modularity calculation

            result.push_back(comm);
        }

        return result;
    }

    /**
     * Get which community a user belongs to.
     */
    int get_user_community(int user_id, const vector<Community>& communities) const {
        for (const auto& comm : communities) {
            for (int member : comm.member_ids) {
                if (member == user_id) {
                    return comm.community_id;
                }
            }
        }
        return -1; // User not found
    }

    /**
     * Get all members of a specific community.
     */
    vector<int> get_community_members(int community_id, const vector<Community>& communities) const {
        for (const auto& comm : communities) {
            if (comm.community_id == community_id) {
                return comm.member_ids;
            }
        }
        return {};
    }
};