#pragma once

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

#include "Backend/Utils.hpp"
#include "Backend/graph_generator.hpp"
#include <queue>
#include <stack>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>
#include <set>

using namespace std;
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


//  the inline keyword tells the compiler to make the function call 
// there itself rather than calling some other 


// ============================================================================
// MAIN ALGORITHMS CLASS
// ============================================================================

/**
 * GRAPH ALGORITHMS ENGINE
 * =======================
 * Central class containing all graph analysis algorithms.
 * 
 * Usage:
 *   SocialGraph graph;
 *   graph.initializeGraph(nodes_path, edges_path, metadata_path);
 *   GraphAlgorithms engine(graph);
 *   auto recommendations = engine.get_friend_recommendations(user_id, 10);
 */
class GraphAlgorithms {
private:
    // Reference to the graph (no copy, just reference)
    const SocialGraph& graph;

    // ========== PRIVATE HELPER METHODS ==========

    /**
     * Breadth-first search traversal starting from a node.
     * Used internally for path finding and graph exploration.
     */

    vector<int> bfs(int start_node_id) const {
        vector<int> visited_order;
        auto meta = graph.getMetadata();

        visited_order.reserve(meta.total_nodes);  // minor optimization

        unordered_set<int> visited;
        visited.reserve(meta.total_nodes);        // avoids rehashing

        queue<int> q;

        q.push(start_node_id);
        visited.insert(start_node_id);

        while (!q.empty()) {
            int u = q.front(); 
            q.pop();

            visited_order.push_back(u);

            for (int v : graph.getNeighbors(u)) {
                if (visited.find(v) == visited.end()) {
                    visited.insert(v);
                    q.push(v);
                }
            }
        }
        return visited_order;
    }


    /**
     * Depth-first search traversal - alternative to BFS.
     * Used for connected components analysis.
     */
    void depth_first_search_recursive(

        int current_node_id,
        unordered_set<int>& visited,
        vector<int>& result_path) const 
        {
            visited.insert(current_node_id);
            result_path.push_back(current_node_id);

            for (int neighbor_id : graph.getNeighbors(current_node_id)) {
                if (visited.find(neighbor_id) == visited.end()) {
                depth_first_search_recursive(neighbor_id, visited, result_path);
            }
        }   
    }

    vector<int> dfs(int start_node_id) const {
        vector<int> result_path;
        auto meta = graph.getMetadata();

        result_path.reserve(meta.total_nodes);

        unordered_set<int> visited;
        visited.reserve(meta.total_nodes);

        depth_first_search_recursive(start_node_id, visited, result_path);

        return result_path;
    }


    /**
     * Calculate distance between two nodes (number of hops).
     * Returns -1 if no path exists.
     */
    int calculate_shortest_distance(int source_id, int target_id) const {
        if (source_id == target_id) return 0;

        set<int> visited;
        queue<pair<int, int>> q; // (node_id, distance)
        
        q.push({source_id, 0});
        visited.insert(source_id);

        while (!q.empty()) {
            auto [current_id, distance] = q.front();
            q.pop();

            for (int neighbor_id : graph.getNeighbors(current_id)) {
                if (neighbor_id == target_id) {
                    return distance + 1;
                }

                if (visited.find(neighbor_id) == visited.end()) {
                    visited.insert(neighbor_id);
                    q.push({neighbor_id, distance + 1});
                }
            }
        }
        return -1; // No path found
    }

    /**
     * Get the actual path between two nodes.
     * Returns full path as list of node IDs.
     */
    vector<int> reconstruct_path(int source_id, int target_id) const {
        if (source_id == target_id) {
            return {source_id};
        }

        map<int, int> parent; // node_id -> previous_node_id
        set<int> visited;
        queue<int> q;

        q.push(source_id);
        visited.insert(source_id);
        parent[source_id] = -1;

        while (!q.empty()) {
            int current_id = q.front();
            q.pop();

            if (current_id == target_id) {
                // Reconstruct path
                vector<int> path;
                int node = target_id;
                while (node != -1) {
                    path.push_back(node);
                    node = parent[node];
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (int neighbor_id : graph.getNeighbors(current_id)) {
                if (visited.find(neighbor_id) == visited.end()) {
                    visited.insert(neighbor_id);
                    parent[neighbor_id] = current_id;
                    q.push(neighbor_id);
                }
            }
        }
        return {}; // No path
    }

    public:
    /**
     * Constructor: Create algorithm engine for a graph.
     * The graph must be initialized before creating this engine.
     */
    explicit GraphAlgorithms(const SocialGraph& social_graph) 
        : graph(social_graph) {}

    // ========== CORE ALGORITHMS ==========

    /**
     * MUTUAL FRIENDS ANALYZER
     * 
     * Find all common friends between two users.
     * Shows how connected two users are in the network.
     * 
     * Algorithm: intersection of neighbor sets
     * Time Complexity: O(min(deg1, deg2))
     * 
     * Returns: MutualFriendsResult with IDs and similarity ratio
     */
    MutualFriendsResult analyze_mutual_friends(int user_id_1, int user_id_2) const {
        const Node* node1 = graph.getNode(user_id_1);
        const Node* node2 = graph.getNode(user_id_2);
        
        MutualFriendsResult result;
        result.user_id_1 = user_id_1;
        result.user_id_2 = user_id_2;
        result.total_degree_1 = node1 ? graph.getDegree(user_id_1) : 0;
        result.total_degree_2 = node2 ? graph.getDegree(user_id_2) : 0;

        if (!node1 || !node2) {
            result.similarity_ratio = 0.0;
            return result;
        }

        auto neighbors_1 = graph.getNeighbors(user_id_1);
        auto neighbors_2 = graph.getNeighbors(user_id_2);

        auto mutual_set = AlgoUtils::set_intersection_of_two(neighbors_1, neighbors_2);
        result.mutual_ids = vector<int>(mutual_set.begin(), mutual_set.end());

        // Calculate Jaccard similarity
        result.similarity_ratio = AlgoUtils::jaccard_similarity(neighbors_1, neighbors_2);

        return result;
    }

    /**
     * FRIEND RECOMMENDER SYSTEM
     * 
     * Suggest new friends based on:
     * - Mutual friends (40% weight)
     * - Common interests (25% weight)
     * - Geographic proximity (20% weight)
     * - Community membership (15% weight)
     * 
     * Algorithm: 
     * 1. Find "friends of friends" (excluding already connected)
     * 2. Score each candidate based on above factors
     * 3. Return top K sorted by score
     * 
     * Time Complexity: O(V * avg_degree^2)
     * Returns: Vector of recommendations sorted by score (highest first)
     */
    vector<FriendRecommendation> get_friend_recommendations(
        int target_user_id,
        int recommendation_count = 10) const 
    {
        const Node* target_user = graph.getNode(target_user_id);
        if (!target_user) return {};

        auto target_neighbors = graph.getNeighbors(target_user_id);
        map<int, FriendRecommendation> candidate_map;

        // Step 1: Find candidates (friends of friends)
        for (int friend_id : target_neighbors) {
            auto friend_neighbors = graph.getNeighbors(friend_id);
            for (int candidate_id : friend_neighbors) {
                // Skip if already friend or self
                if (candidate_id == target_user_id || 
                    target_neighbors.count(candidate_id)) {
                    continue;
                }

                if (candidate_map.find(candidate_id) == candidate_map.end()) {
                    candidate_map[candidate_id] = FriendRecommendation();
                    candidate_map[candidate_id].recommended_user_id = candidate_id;
                }
            }
        }

        // Step 2: Score each candidate
        for (auto& [candidate_id, recommendation] : candidate_map) {
            const Node* candidate = graph.getNode(candidate_id);
            if (!candidate) continue;

            recommendation.user_name = candidate->name;

            // Mutual friends count (40% weight)
            auto mutual = AlgoUtils::set_intersection_of_two(
                target_neighbors,
                graph.getNeighbors(candidate_id));
            recommendation.mutual_friends_count = mutual.size();
            double mutual_score = AlgoUtils::normalize_to_01(
                recommendation.mutual_friends_count, 100.0) * 40.0;

            // Common interests (25% weight)
            auto common_interests = AlgoUtils::find_common_items(
                target_user->interests,
                candidate->interests);
            recommendation.common_interests_count = common_interests.size();
            double interests_score = AlgoUtils::normalize_to_01(
                recommendation.common_interests_count, 10.0) * 25.0;

            // Geographic proximity (20% weight)
            double distance_km = AlgoUtils::calculate_haversine_distance(
                target_user->location.latitude,
                target_user->location.longitude,
                candidate->location.latitude,
                candidate->location.longitude);
            recommendation.geographic_distance_km = distance_km;
            // Closer is better: max 500 km radius
            double geo_score = max(0.0, 1.0 - (distance_km / 500.0)) * 20.0;

            // Community similarity (15% weight) - to be enhanced with community detection
            recommendation.community_similarity = 0.5; // Placeholder
            double community_score = recommendation.community_similarity * 15.0;

            // Total score
            recommendation.total_score = mutual_score + interests_score + 
                                        geo_score + community_score;
            recommendation.recommendation_reason = 
                "Has " + to_string(recommendation.mutual_friends_count) + 
                " mutual friends and " +
                to_string(recommendation.common_interests_count) + 
                " shared interests";
        }

        // Step 3: Sort by score and return top K
        vector<FriendRecommendation> result;
        for (auto& [id, rec] : candidate_map) {
            result.push_back(rec);
        }
        sort(result.begin(), result.end(),
             [](const auto& a, const auto& b) { 
                 return a.total_score > b.total_score; 
             });

        if (result.size() > recommendation_count) {
            result.resize(recommendation_count);
        }
        return result;
    }

    /**
     * SHORTEST PATH FINDER
     * 
     * Find the shortest connection path between two users.
     * Shows how two users are related through the network.
     * 
     * Algorithm: Breadth-first search (BFS)
     * Time Complexity: O(V + E)
     * Returns: Path result with node IDs and human-readable description
     */
    PathFindResult find_shortest_path(int source_user_id, int target_user_id) const {
        PathFindResult result;

        if (source_user_id == target_user_id) {
            result.path_exists = true;
            result.path_node_ids = {source_user_id};
            result.path_length = 0;
            return result;
        }

        result.path_node_ids = reconstruct_path(source_user_id, target_user_id);
        result.path_exists = !result.path_node_ids.empty();
        result.path_length = result.path_node_ids.size() - 1;

        // Create human-readable description
        if (result.path_exists) {
            result.path_description = "";
            for (int i = 0; i < result.path_node_ids.size(); i++) {
                const Node* node = graph.getNode(result.path_node_ids[i]);
                if (node) {
                    result.path_description += node->name;
                    if (i < result.path_node_ids.size() - 1) {
                        result.path_description += " → ";
                    }
                }
            }
        }

        return result;
    }

    /**
     * CENTRALITY ANALYSIS
     * 
     * Calculate various centrality measures for a user.
     * Shows how important/central a user is in the network.
     * 
     * Metrics:
     * - Degree centrality: normalized number of connections
     * - Closeness: average distance to all other users
     * - Clustering coefficient: do my friends know each other?
     * - Betweenness: how often am I on shortest paths?
     * 
     * Time Complexity: O(V + E) per user
     */
    CentralityMetrics calculate_centrality_metrics(int user_id) const {
        CentralityMetrics metrics;
        metrics.user_id = user_id;

        int degree = graph.getDegree(user_id);
        int max_possible = graph.getNodeCount() - 1;
        metrics.raw_degree = degree;
        metrics.degree_centrality = max_possible > 0 ? 
            (double)degree / max_possible : 0.0;

        // Closeness centrality
        int total_distance = 0;
        int reachable_count = 0;
        for (const auto& [other_id, node] : graph.getNodes()) {
            if (other_id == user_id) continue;
            int dist = calculate_shortest_distance(user_id, other_id);
            if (dist >= 0) {
                total_distance += dist;
                reachable_count++;
            }
        }
        double avg_distance = reachable_count > 0 ? 
            (double)total_distance / reachable_count : 0.0;
        metrics.closeness_centrality = 1.0 / (1.0 + avg_distance);

        // Clustering coefficient
        auto neighbors = graph.getNeighbors(user_id);
        if (neighbors.size() < 2) {
            metrics.clustering_coefficient = 0.0;
        } else {
            int triangle_count = 0;
            int max_triangles = (neighbors.size() * (neighbors.size() - 1)) / 2;
            
            vector<int> neighbor_list(neighbors.begin(), neighbors.end());
            for (int i = 0; i < neighbor_list.size(); i++) {
                for (int j = i + 1; j < neighbor_list.size(); j++) {
                    auto neighbor_i_neighbors = graph.getNeighbors(neighbor_list[i]);
                    if (neighbor_i_neighbors.count(neighbor_list[j])) {
                        triangle_count++;
                    }
                }
            }
            metrics.clustering_coefficient = max_triangles > 0 ? 
                (double)triangle_count / max_triangles : 0.0;
        }

        metrics.betweenness_centrality = 0.0; // TODO: implement if needed
        return metrics;
    }

    /**
     * PAGERANK CALCULATION
     * 
     * Calculate importance score for each user using PageRank algorithm.
     * Models: user importance based on how many important users follow them.
     * 
     * Algorithm: Iterative PageRank
     * Time Complexity: O(E * iterations)
     * Returns: Map of user_id -> importance_score (0.0 to 1.0)
     */
    map<int, double> calculate_pagerank(
        double damping_factor = 0.85,
        int iteration_count = 20) const 
    {
        map<int, double> rank;
        const auto& all_nodes = graph.getNodes();
        int node_count = all_nodes.size();

        if (node_count == 0) return rank;

        // Initialize: all users start with equal importance
        double initial_rank = 1.0 / node_count;
        for (const auto& [id, node] : all_nodes) {
            rank[id] = initial_rank;
        }

        // Iteratively update ranks
        for (int iteration = 0; iteration < iteration_count; iteration++) {
            map<int, double> new_rank;

            // Base score: each node gets (1 - damping) / N
            for (const auto& [id, node] : all_nodes) {
                new_rank[id] = (1.0 - damping_factor) / node_count;
            }

            // Add contribution from neighbors
            for (const auto& [id, node] : all_nodes) {
                int out_degree = node.neighbors.size();
                if (out_degree > 0) {
                    double contribution = rank[id] / out_degree;
                    for (int neighbor_id : node.neighbors) {
                        new_rank[neighbor_id] += damping_factor * contribution;
                    }
                }
            }

            rank = new_rank;
        }

        return rank;
    }

    /**
     * NETWORK STATISTICS
     * 
     * Calculate global network metrics.
     */
    double calculate_network_density() const {
        int node_count = graph.getNodeCount();
        int edge_count = graph.getEdgeCount();
        if (node_count <= 1) return 0.0;
        double max_edges = (node_count * (node_count - 1)) / 2.0;
        return edge_count / max_edges;
    }

    double calculate_average_clustering_coefficient() const {
        double sum = 0.0;
        int count = 0;
        for (const auto& [id, node] : graph.getNodes()) {
            sum += calculate_centrality_metrics(id).clustering_coefficient;
            count++;
        }
        return count > 0 ? sum / count : 0.0;
    }

};
