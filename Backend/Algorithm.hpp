#pragma once

#include "graph_generator.hpp"
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

/**
 * Algorithm class for social graph operations
 */
class GraphAlgorithms {
public:
    /**
     * Find mutual friends between two users
     * 
     * Mutual friends are users who have a "friend" relationship (mutual follow)
     * with both user1 and user2.
     * 
     * @param graph Reference to the SocialGraph
     * @param user1_id First user's ID
     * @param user2_id Second user's ID
     * @return Vector of user IDs who are mutual friends of both users
     */
    static vector<int> findMutualFriends(const SocialGraph& graph, int user1_id, int user2_id) {
        vector<int> mutualFriends;
        
        // Check if both users exist
        const Node* node1 = graph.getNode(user1_id);
        const Node* node2 = graph.getNode(user2_id);
        
        if (!node1 || !node2) {
            return mutualFriends; // Return empty if either user doesn't exist
        }
        
        // Get neighbors of both users
        set<int> neighbors1 = graph.getNeighbors(user1_id);
        set<int> neighbors2 = graph.getNeighbors(user2_id);
        
        // Find intersection (common neighbors)
        set<int> commonNeighbors;
        set_intersection(
            neighbors1.begin(), neighbors1.end(),
            neighbors2.begin(), neighbors2.end(),
            inserter(commonNeighbors, commonNeighbors.begin())
        );
        
        // Filter to only include mutual friends (both relationships must be "friend" type)
        for (int friend_id : commonNeighbors) {
            if (areFriends(graph, user1_id, friend_id) && 
                areFriends(graph, user2_id, friend_id)) {
                mutualFriends.push_back(friend_id);
            }
        }
        
        return mutualFriends;
    }
    
    /**
     * Check if two users are friends (have a mutual "friend" relationship)
     * 
     * A friend relationship exists when there are edges in BOTH directions
     * between the two users (bidirectional connection).
     * 
     * @param graph Reference to the SocialGraph
     * @param user1_id First user's ID
     * @param user2_id Second user's ID
     * @return True if they have a "friend" relationship (bidirectional), false otherwise
     */
    static bool areFriends(const SocialGraph& graph, int user1_id, int user2_id) {
        vector<Edge> edges = graph.getEdgesForNode(user1_id);
        
        bool hasEdge1to2 = false;
        bool hasEdge2to1 = false;
        
        for (const auto& edge : edges) {
            if (edge.source == user1_id && edge.target == user2_id) {
                hasEdge1to2 = true;
            } else if (edge.source == user2_id && edge.target == user1_id) {
                hasEdge2to1 = true;
            }
        }
        
        // Friend relationship requires edges in both directions
        return hasEdge1to2 && hasEdge2to1;
    }
    
    /**
     * Get the count of mutual friends between two users
     * 
     * @param graph Reference to the SocialGraph
     * @param user1_id First user's ID
     * @param user2_id Second user's ID
     * @return Number of mutual friends
     */
    static int getMutualFriendsCount(const SocialGraph& graph, int user1_id, int user2_id) {
        return findMutualFriends(graph, user1_id, user2_id).size();
    }
};
