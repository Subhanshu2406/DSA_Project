// include/Backend/friend_recommender.hpp
/*
 * FRIEND RECOMMENDATION ALGORITHM
 * ===============================
 * Multi-factor scoring system for suggesting new friends.
 */

#pragma once

#include "../graph_generator.hpp"
#include "../algo_utils.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <string>

using namespace std;

/**
 * Result of friend recommendation algorithm.
 * Contains recommended user with explanation and scoring breakdown.
 */
struct FriendRecommendation {
    int recommended_user_id;       // Who to recommend
    string user_name;              // Display name
    double total_score;            // 0.0 to 100.0 (overall recommendation strength)
    int mutual_friends_count;      // How many friends in common
    int common_interests_count;    // How many shared interests
    double geographic_distance_km; // Distance in kilometers
    double community_similarity;   // 0.0 to 1.0 (are they from same community?)
    string recommendation_reason;  // Human-readable explanation
};

class FriendRecommender {
private:
    const SocialGraph& graph;

    /**
     * Score based on mutual friends (40% weight).
     * More mutual friends = higher score.
     */
    double score_mutual_friends(int mutual_count) const {
        return AlgoUtils::normalize_to_01(mutual_count, 100.0) * 40.0;
    }

    /**
     * Score based on common interests (25% weight).
     * More shared interests = higher score.
     */
    double score_common_interests(int interests_count) const {
        return AlgoUtils::normalize_to_01(interests_count, 10.0) * 25.0;
    }

    /**
     * Score based on geographic proximity (20% weight).
     * Closer distance = higher score (max 500km).
     */
    double score_geographic_proximity(double distance_km) const {
        double proximity = max(0.0, 1.0 - (distance_km / 500.0));
        return proximity * 20.0;
    }

    /**
     * Score based on community membership (15% weight).
     * Placeholder for now - will be enhanced with community detection.
     */
    double score_community_similarity(double similarity) const {
        return similarity * 15.0;
    }

public:
    explicit FriendRecommender(const SocialGraph& social_graph) 
        : graph(social_graph) {}

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
     * Space Complexity: O(V)
     * Returns: Vector of recommendations sorted by score (highest first)
     */
    vector<FriendRecommendation> get_recommendations(
        int target_user_id,
        int recommendation_count = 10) const 
    {
        const Node* target_user = graph.getNode(target_user_id);
        if (!target_user) return {};

        auto target_friends = graph.getFriends(target_user_id);
        map<int, FriendRecommendation> candidate_map;

        // Step 1: Find candidates (friends of friends)
        for (int friend_id : target_friends) {
            auto friend_connections = graph.getFriends(friend_id);
            for (int candidate_id : friend_connections) {
                // Skip if already friend or self
                if (candidate_id == target_user_id || target_friends.count(candidate_id)) {
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
                target_friends,
                graph.getFriends(candidate_id));
            recommendation.mutual_friends_count = mutual.size();
            double mutual_score = score_mutual_friends(recommendation.mutual_friends_count);

            // Common interests (25% weight)
            auto common_interests = AlgoUtils::find_common_items(
                target_user->interests,
                candidate->interests);
            recommendation.common_interests_count = common_interests.size();
            double interests_score = score_common_interests(recommendation.common_interests_count);

            // Geographic proximity (20% weight)
            double distance_km = AlgoUtils::calculate_haversine_distance(
                target_user->location.latitude,
                target_user->location.longitude,
                candidate->location.latitude,
                candidate->location.longitude);
            recommendation.geographic_distance_km = distance_km;
            double geo_score = score_geographic_proximity(distance_km);

            // Community similarity (15% weight)
            recommendation.community_similarity = 0.5; // Placeholder
            double community_score = score_community_similarity(recommendation.community_similarity);

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
};
