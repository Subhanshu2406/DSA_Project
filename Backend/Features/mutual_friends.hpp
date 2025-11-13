// include/Backend/mutual_friends.hpp
/*
 * MUTUAL FRIENDS ALGORITHM
 * ========================
 * Analyzes common connections between two users.
 */

#pragma once

#include "graph_generator.hpp"
#include "Backend/algo_utils.hpp"
#include <vector>

using namespace std;

/**
 * Result of mutual friends analysis between two users.
 * Used for displaying: "You have X mutual friends with User Y"
 */
struct MutualFriendsResult {
    int user_id_1;           // First user
    int user_id_2;           // Second user
    vector<int> mutual_ids;  // IDs of all mutual friends
    double similarity_ratio; // 0.0 to 1.0 (how similar their friend lists are)
    int total_degree_1;      // Total friends of user 1
    int total_degree_2;      // Total friends of user 2
};

class MutualFriendsAnalyzer {
private:
    const SocialGraph& graph;

public:
    explicit MutualFriendsAnalyzer(const SocialGraph& social_graph) 
        : graph(social_graph) {}

    /**
     * MUTUAL FRIENDS ANALYZER
     * 
     * Find all common friends between two users.
     * Shows how connected two users are in the network.
     * 
     * Algorithm: intersection of neighbor sets
     * Time Complexity: O(min(deg1, deg2))
     * Space Complexity: O(min(deg1, deg2))
     * 
     * Returns: MutualFriendsResult with IDs and similarity ratio
     */
    MutualFriendsResult analyze(int user_id_1, int user_id_2) const {
        const Node* node1 = graph.getNode(user_id_1);
        const Node* node2 = graph.getNode(user_id_2);

        MutualFriendsResult result;
        result.user_id_1 = user_id_1;
        result.user_id_2 = user_id_2;
        result.similarity_ratio = 0.0;

        if (!node1 || !node2) {
            return result;
        }

        const auto& neighbors_1 = graph.getNeighbors(user_id_1);
        const auto& neighbors_2 = graph.getNeighbors(user_id_2);

        result.total_degree_1 = neighbors_1.size();
        result.total_degree_2 = neighbors_2.size();

        // Early exit: no neighbors means no mutual friends
        if (neighbors_1.empty() || neighbors_2.empty()) {
            return result;
        }

        // Optimize: Intersect with smaller set first
        const auto& smaller = neighbors_1.size() < neighbors_2.size() ? neighbors_1 : neighbors_2;
        const auto& larger = neighbors_1.size() < neighbors_2.size() ? neighbors_2 : neighbors_1;

        // Count intersection without creating intermediate set
        for (int neighbor : smaller) {
            if (larger.count(neighbor)) {
                result.mutual_ids.push_back(neighbor);
            }
        }

        // Calculate Jaccard similarity
        int union_size = neighbors_1.size() + neighbors_2.size() - result.mutual_ids.size();
        result.similarity_ratio = union_size == 0 ? 0.0 : (double)result.mutual_ids.size() / union_size;

        return result;
    }
};