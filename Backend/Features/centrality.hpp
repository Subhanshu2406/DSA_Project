// include/Backend/centrality.hpp
/*
 * CENTRALITY ANALYSIS ALGORITHM
 * =============================
 * Measures how important/central a node is in the network.
 */

#pragma once

#include "graph_generator.hpp"
#include "short_path.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

/**
 * Result of centrality analysis.
 * Centrality measures how important a node is in the network.
 */
struct CentralityMetrics {
    int user_id;
    double degree_centrality;      // Normalized: (friends / max_possible)
    double closeness_centrality;   // How close to all other users (0.0 to 1.0)
    double clustering_coefficient; // Do my friends know each other? (0.0 to 1.0)
    double betweenness_centrality; // How often am I on paths between others?
    int raw_degree;                // Actual number of connections
};

class CentralityAnalyzer {
private:
    const SocialGraph& graph;
    mutable OptimizedDistanceCalculator path_calculator;

    /**
     * Count triangles in friend neighborhood (for clustering coefficient).
     * Optimized: only checks when nodes have sufficient connections.
     */
    int count_triangles_in_friend_neighborhood(const set<int>& friend_ids) const {
        if (friend_ids.size() < 2) return 0;

        int triangle_count = 0;
        vector<int> friend_list(friend_ids.begin(), friend_ids.end());

        // Only check pairs in the smaller direction for optimization
        for (int i = 0; i < friend_list.size(); i++) {
            auto friends_of_i = graph.getFriends(friend_list[i]);

            if (friends_of_i.empty()) continue;

            for (int j = i + 1; j < friend_list.size(); j++) {
                if (friends_of_i.count(friend_list[j])) {
                    triangle_count++;
                }
            }
        }
        return triangle_count;
    }

public:
    explicit CentralityAnalyzer(const SocialGraph& social_graph) 
        : graph(social_graph), path_calculator(social_graph) {}

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
     * Space Complexity: O(V)
     * 
     * Returns: CentralityMetrics with all measures
     */
    CentralityMetrics calculate_metrics(int user_id) const {
        CentralityMetrics metrics;
        metrics.user_id = user_id;

        // Degree centrality
        int degree = graph.getFriendCount(user_id);
        int max_possible = graph.getNodeCount() - 1;
        metrics.raw_degree = degree;
        metrics.degree_centrality = max_possible > 0 ? 
            (double)degree / max_possible : 0.0;

        // Closeness centrality: average distance to all reachable users
        int total_distance = 0;
        int reachable_count = 0;

        for (const auto& [other_id, node] : graph.getNodes()) {
            if (other_id == user_id) continue;
            int dist = path_calculator.calculate_shortest_distance(user_id, other_id);
            if (dist >= 0) {
                total_distance += dist;
                reachable_count++;
            }
        }

        double avg_distance = reachable_count > 0 ? 
            (double)total_distance / reachable_count : 0.0;
        metrics.closeness_centrality = 1.0 / (1.0 + avg_distance);

        // Clustering coefficient: do my friends know each other?
        auto friends = graph.getFriends(user_id);
        if (friends.size() < 2) {
            metrics.clustering_coefficient = 0.0;
        } else {
            int triangle_count = count_triangles_in_friend_neighborhood(friends);
            int max_triangles = (friends.size() * (friends.size() - 1)) / 2;

            metrics.clustering_coefficient = max_triangles > 0 ? 
                (double)triangle_count / max_triangles : 0.0;
        }

        metrics.betweenness_centrality = 0.0; // TODO: implement if needed
        return metrics;
    }

    /**
     * Calculate centrality for all users.
     * Returns vector of metrics for each user.
     */
    vector<CentralityMetrics> calculate_all_users() const {
        vector<CentralityMetrics> results;

        for (const auto& [id, node] : graph.getNodes()) {
            results.push_back(calculate_metrics(id));
        }

        return results;
    }

    /**
     * Calculate average clustering coefficient across all users.
     * Shows how much the network tends to form clusters/cliques.
     * 
     * Time Complexity: O(V * avg_degree^2)
     * Space Complexity: O(1)
     */
    double calculate_average_clustering() const {
        double sum = 0.0;
        int count = 0;

        for (const auto& [id, node] : graph.getNodes()) {
            sum += calculate_metrics(id).clustering_coefficient;
            count++;
        }

        return count > 0 ? sum / count : 0.0;
    }

    /**
     * Clear path cache to free up memory.
     */
    void clear_cache() const {
        path_calculator.clear_cache();
    }

    /**
     * Get cache statistics.
     */
    void print_cache_stats() const {
        path_calculator.print_cache_stats();
    }
};
