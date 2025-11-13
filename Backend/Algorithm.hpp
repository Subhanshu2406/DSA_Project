
// include/algorithms.hpp
/*
 * SOCIAL GRAPH ALGORITHMS LIBRARY
 * ================================
 * This file contains all graph analysis algorithms.
 * Every algorithm operates on a const reference to SocialGraph - no data duplication.
 * Results are returned as simple data structures for easy serialization to frontend.
 * 
 * Design Philosophy:
 * - Single responsibility per function
 * - Clear input/output types
 * - Extensive documentation
 * - No side effects
 * - Results designed for frontend visualization
 */

#pragma once

#include "Backend/graph_generator.hpp"
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

// ============================================================================
// RESULT DATA STRUCTURES - Used for returning algorithm outputs to frontend
// ============================================================================

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

/**
 * Result of friend recommendation algorithm.
 * Contains recommended user with explanation and scoring breakdown.
 */
struct FriendRecommendation {
    int recommended_user_id;    // Who to recommend
    string user_name;           // Display name
    double total_score;         // 0.0 to 100.0 (overall recommendation strength)
    int mutual_friends_count;   // How many friends in common
    int common_interests_count; // How many shared interests
    double geographic_distance_km; // Distance in kilometers
    double community_similarity;   // 0.0 to 1.0 (are they from same community?)
    string recommendation_reason;  // Human-readable explanation
};

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

/**
 * Result of influencer ranking algorithm.
 * An influencer has many followers and high engagement.
 */
struct InfluencerRank {
    int rank;                    // Position in leaderboard (1, 2, 3, ...)
    int user_id;                 // Who is this person
    string user_name;            // Display name
    double influence_score;      // 0.0 to 100.0 (composite score)
    int fan_count;               // Number of "fan" type relationships
    int friend_count;            // Number of "friend" type relationships
    double centrality_score;     // PageRank-like importance measure
    int total_followers;         // fan_count + friend_count
    vector<int> top_fan_ids;     // Top K fans/followers (for visualization)
};

/**
 * Result of centrality analysis.
 * Centrality measures how important a node is in the network.
 */
struct CentralityMetrics {
    int user_id;
    double degree_centrality;         // Normalized: (friends / max_possible)
    double closeness_centrality;      // How close to all other users (0.0 to 1.0)
    double clustering_coefficient;    // Do my friends know each other? (0.0 to 1.0)
    double betweenness_centrality;    // How often am I on paths between others?
    int raw_degree;                   // Actual number of connections
};

/**
 * Result of shortest path algorithm.
 * Shows how two users are connected through intermediate users.
 */
struct PathFindResult {
    bool path_exists;           // Whether connection found
    vector<int> path_node_ids;  // IDs of users in the path (source to target)
    int path_length;            // Number of hops (edges)
    string path_description;    // Human readable: "A -> B -> C"
};

/**
 * Result of community-based recommendations.
 * Uses community structure to find good friend suggestions.
 */
struct CommunityBasedRecommendation {
    int user_id;                 // User being recommended to
    int community_id;            // Which community they belong to
    vector<FriendRecommendation> same_community_recs;  // Users from their community
    vector<FriendRecommendation> adjacent_community_recs; // Users from neighboring communities
};

/**
 * Leaderboard entry for various rankings.
 * Used for showing top users by different metrics.
 */
struct LeaderboardEntry {
    int rank;              // 1st, 2nd, 3rd, etc.
    int user_id;
    string user_name;
    double score;          // Main metric value
    int secondary_metric;  // Context-dependent (followers, connections, etc.)
};

/**
 * Local community metrics for a specific user.
 * Shows statistics about user's immediate network.
 */
struct LocalCommunityMetrics {
    int user_id;
    int community_id;
    int community_size;
    int user_degree_in_community;      // Connections within community
    int user_degree_outside_community; // Connections outside community
    double local_clustering_coeff;     // Clustering within their community
    vector<int> top_influencers_in_community; // Top 5 influencers nearby
};

