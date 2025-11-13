// include/Backend/algorithms.hpp
/*
 * UNIFIED ALGORITHMS HEADER
 * ==========================
 * Integrates all individual algorithm classes into one convenient interface.
 * 
 * This file provides a single entry point to use all graph algorithms:
 * - Mutual Friends Analysis
 * - Friend Recommendations
 * - PageRank Calculation
 * - Community Detection
 * - Influencer Ranking
 * - Centrality Analysis
 * - Shortest Path Finding
 */

#pragma once

#include "graph_generator.hpp"
#include "mutual_friends.hpp"
#include "friend_recommender.hpp"
#include "pagerank.hpp"
// #include "community_detection.hpp"
// #include "influencer_ranking.hpp"
// #include "centrality.hpp"
#include "short_path.hpp"
#include <vector>

using namespace std;

// ============================================================================
// MAIN ALGORITHMS ENGINE - Unified Interface
// ============================================================================

/**
 * GRAPH ALGORITHMS ENGINE
 * =======================
 * Central class providing unified access to all graph analysis algorithms.
 * 
 * Usage:
 *   SocialGraph graph;
 *   graph.initializeGraph(nodes_path, edges_path, metadata_path);
 *   
 *   GraphAlgorithms engine(graph);
 *   
 *   // Use any algorithm
 *   auto recommendations = engine.friend_recommender.get_recommendations(user_id, 10);
 *   auto communities = engine.community_detector.detect_label_propagation();
 *   auto leaderboard = engine.influencer_ranker.get_leaderboard(100);
 */
class GraphAlgorithms {
public:
    // Individual algorithm components
    MutualFriendsAnalyzer mutual_friends_analyzer;
    FriendRecommender friend_recommender;
    PageRankCalculator pagerank_calculator;
    CommunityDetector community_detector;
    InfluencerRanker influencer_ranker;
    CentralityAnalyzer centrality_analyzer;
    OptimizedDistanceCalculator path_calculator;

private:
    const SocialGraph& graph;

public:
    /**
     * Constructor: Create unified algorithm engine for a graph.
     * Initializes all individual algorithm components.
     */
    explicit GraphAlgorithms(const SocialGraph& social_graph)
        : graph(social_graph),
          mutual_friends_analyzer(social_graph),
          friend_recommender(social_graph),
          pagerank_calculator(social_graph),
          community_detector(social_graph),
          influencer_ranker(social_graph),
          centrality_analyzer(social_graph),
          path_calculator(social_graph) {}

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

    /**
     * Get average clustering coefficient across entire network.
     */
    double calculate_average_clustering() const {
        return centrality_analyzer.calculate_average_clustering();
    }

    /**
     * CONVENIENCE METHODS - Shortcuts for common operations
     */

    // Mutual Friends
    MutualFriendsResult analyze_mutual_friends(int user1, int user2) const {
        return mutual_friends_analyzer.analyze(user1, user2);
    }

    // Friend Recommendations
    vector<FriendRecommendation> get_friend_recommendations(
        int user_id, int count = 10) const 
    {
        return friend_recommender.get_recommendations(user_id, count);
    }

    // PageRank
    map<int, double> calculate_pagerank(
        double damping = 0.85, int iterations = 20) const 
    {
        return pagerank_calculator.calculate(damping, iterations);
    }

    // Community Detection
    vector<Community> detect_communities(int method = 0, int iterations = 10) const {
        if (method == 0) {
            return community_detector.detect_label_propagation(iterations);
        } else {
            return community_detector.detect_greedy_modularity(iterations);
        }
    }

    // Influencer Leaderboard
    vector<InfluencerRank> get_influencer_leaderboard(
        int top_k = 100, int pagerank_iterations = 20) const 
    {
        return influencer_ranker.get_leaderboard(top_k, pagerank_iterations);
    }

    // Centrality
    CentralityMetrics get_centrality_metrics(int user_id) const {
        return centrality_analyzer.calculate_metrics(user_id);
    }

    // Shortest Path
    PathFindResult find_shortest_path(int source, int target) const {
        return path_calculator.find_path(source, target);
    }

    // Distance only
    int get_shortest_distance(int source, int target) const {
        return path_calculator.calculate_shortest_distance(source, target);
    }

    // Batch paths
    vector<PathFindResult> find_paths_batch(
        int source, const vector<int>& targets) const 
    {
        return path_calculator.find_paths_batch(source, targets);
    }
};

#endif // ALGORITHMS_HPP