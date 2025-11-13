// include/Backend/mutual_friends.hpp
/*
 * MUTUAL FRIENDS ALGORITHM
 * ========================
 * Analyzes common connections between two users.
 */

#pragma once

#include "../graph_generator.hpp"
#include "../algo_utils.hpp"
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
     * Algorithm: intersection of friend sets
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

        const auto friends_1 = graph.getFriends(user_id_1);
        const auto friends_2 = graph.getFriends(user_id_2);

        result.total_degree_1 = friends_1.size();
        result.total_degree_2 = friends_2.size();

        // Early exit: no friends means no mutual friends
        if (friends_1.empty() || friends_2.empty()) {
            return result;
        }

        // Optimize: Intersect with smaller set first
        const auto& smaller = friends_1.size() < friends_2.size() ? friends_1 : friends_2;
        const auto& larger = friends_1.size() <= friends_2.size() ? friends_2 : friends_1;

        // Count intersection without creating intermediate set
        for (int friend_id : smaller) {
            if (larger.count(friend_id)) {
                result.mutual_ids.push_back(friend_id);
            }
        }

        // Calculate Jaccard similarity
        int union_size = friends_1.size() + friends_2.size() - result.mutual_ids.size();
        result.similarity_ratio = union_size == 0 ? 0.0 : (double)result.mutual_ids.size() / union_size;

        return result;
    }
};
