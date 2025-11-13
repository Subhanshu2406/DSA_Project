// include/Backend/pagerank.hpp
/*
 * PAGERANK ALGORITHM
 * ==================
 * Calculates user importance scores based on network structure.
 */

#pragma once

#include "graph_generator.hpp"
#include <map>
#include <vector>

using namespace std;

class PageRankCalculator {
private:
    const SocialGraph& graph;

public:
    explicit PageRankCalculator(const SocialGraph& social_graph) 
        : graph(social_graph) {}

    /**
     * PAGERANK CALCULATION
     * 
     * Calculate importance score for each user using PageRank algorithm.
     * Models: user importance based on how many important users follow them.
     * 
     * Algorithm: Iterative PageRank with damping factor
     * Time Complexity: O(E * iterations)
     * Space Complexity: O(V)
     * 
     * Parameters:
     *   - damping_factor: probability of following a link (default 0.85)
     *   - iteration_count: number of iterations for convergence (default 20)
     * 
     * Returns: Map of user_id -> importance_score (0.0 to 1.0)
     */
    map<int, double> calculate(
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
};