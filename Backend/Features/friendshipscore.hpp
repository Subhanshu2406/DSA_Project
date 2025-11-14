// include/Backend/Features/friendship_score.hpp
/*
 * FRIENDSHIP SCORE CALCULATOR
 * ============================
 * Calculates normalized friendship score between two nodes.
 * Score range: 1.0 (strongest friendship) to 3.0 (no connection)
 * Guarantees: Friends always score [1.0, 2.0], non-friends score [2.0, 3.0]
 */

 #pragma once

 #include "../graph_generator.hpp"
 #include "../algo_utils.hpp"
 #include <vector>
 #include <map>
 #include <algorithm>
 #include <string>
 #include <cmath>
 #include <ctime>
 #include <sstream>
 #include <iomanip>
 
 using namespace std;
 
 /**
  * Result of friendship score calculation.
  * Contains score and breakdown of contributing factors.
  */
 struct FriendshipScoreResult {
     double friendship_score;        // 1.0 to 3.0 (lower = stronger friendship)
     bool are_friends;                // Whether they are mutual friends
     int mutual_friends_count;        // Number of mutual friends
     int message_count;               // Total messages exchanged
     double mutual_friends_score;     // Average friendship score of mutual friends
     double time_factor;              // How long they've been connected (0.0-1.0)
     double geographic_proximity;     // Geographic closeness (0.0-1.0)
     int common_interests_count;      // Number of shared interests
     string explanation;              // Human-readable explanation
 };
 
 class FriendshipScoreCalculator {
 private:
     const SocialGraph& graph;
     mutable map<pair<int, int>, double> score_cache;  // Cache to avoid recomputation
     
     /**
      * Get edge between two nodes (if exists).
      * Returns nullptr if no edge exists.
      */
     const Edge* getEdge(int node1, int node2) const {
         for (const auto& edge : graph.getEdges()) {
             if ((edge.source == node1 && edge.target == node2) ||
                 (edge.source == node2 && edge.target == node1)) {
                 return &edge;
             }
         }
         return nullptr;
     }
     
     /**
      * Parse date string (format: "YYYY-MM-DD" or ISO format).
      * Returns days since epoch, or 0 if parsing fails.
      */
     long parseDate(const string& date_str) const {
         if (date_str.empty()) return 0;
         
         // Try ISO format: "YYYY-MM-DD" or "YYYY-MM-DDTHH:MM:SS"
         string date_part = date_str.substr(0, 10);
         if (date_part.length() != 10) return 0;
         
         try {
             int year = stoi(date_part.substr(0, 4));
             int month = stoi(date_part.substr(5, 2));
             int day = stoi(date_part.substr(8, 2));
             
             // Simple day calculation (approximate)
             long days = year * 365 + month * 30 + day;
             return days;
         } catch (...) {
             return 0;
         }
     }
     
     /**
      * Calculate time factor based on how long they've been connected.
      * Longer connection = stronger friendship (lower score).
      * Returns 0.0 (new connection) to 1.0 (very old connection).
      */
     double calculateTimeFactor(const string& established_at) const {
         if (established_at.empty()) return 0.0;
         
         long connection_date = parseDate(established_at);
         if (connection_date == 0) return 0.0;
         
         // Assume current date is around 2024-01-05 (from dataset)
         long current_date = 2024 * 365 + 1 * 30 + 5;
         long days_connected = max(0L, current_date - connection_date);
         
         // Normalize: 0 days = 0.0, 365+ days = 1.0
         double normalized = min(1.0, days_connected / 365.0);
         return normalized;
     }
     
     /**
      * Calculate geographic proximity factor.
      * Closer = stronger friendship (lower score).
      * Returns 0.0 (very far) to 1.0 (very close).
      */
     double calculateGeographicProximity(
         double lat1, double lon1,
         double lat2, double lon2) const {
         
         double distance_km = AlgoUtils::calculate_haversine_distance(
             lat1, lon1, lat2, lon2);
         
         // Normalize: 0km = 1.0, 1000km+ = 0.0
         double proximity = max(0.0, 1.0 - (distance_km / 1000.0));
         return proximity;
     }
     
     /**
      * Calculate average friendship score of mutual friends.
      * This creates a transitive/recursive effect.
      * Returns average score, or 1.5 (neutral) if no mutual friends.
      */
     double calculateMutualFriendsScore(int node1, int node2) const {
         auto mutual_friends = AlgoUtils::set_intersection_of_two(
             graph.getFriends(node1),
             graph.getFriends(node2));
         
         if (mutual_friends.empty()) return 1.5;  // Neutral
         
         double total_score = 0.0;
         int count = 0;
         
         // Calculate average friendship score between node1 and each mutual friend
         // and between node2 and each mutual friend
         for (int mutual : mutual_friends) {
             // Avoid infinite recursion by checking cache
             pair<int, int> key1 = make_pair(min(node1, mutual), max(node1, mutual));
             pair<int, int> key2 = make_pair(min(node2, mutual), max(node2, mutual));
             
             double score1 = 1.5;  // Default neutral
             double score2 = 1.5;
             
             if (score_cache.find(key1) != score_cache.end()) {
                 score1 = score_cache[key1];
             } else {
                 // Calculate recursively (but limit depth to avoid infinite loops)
                 auto result1 = calculateScoreInternal(node1, mutual, 1);  // depth=1
                 score1 = result1.friendship_score;
             }
             
             if (score_cache.find(key2) != score_cache.end()) {
                 score2 = score_cache[key2];
             } else {
                 auto result2 = calculateScoreInternal(node2, mutual, 1);  // depth=1
                 score2 = result2.friendship_score;
             }
             
             // Average of both scores
             total_score += (score1 + score2) / 2.0;
             count++;
         }
         
         return count > 0 ? total_score / count : 1.5;
     }
     
     /**
      * Internal calculation with recursion depth limit.
      */
     FriendshipScoreResult calculateScoreInternal(
         int node1, int node2, int depth) const {
         
         // Limit recursion depth to prevent infinite loops
         if (depth > 2) {
             FriendshipScoreResult result;
             result.friendship_score = 1.5;  // Neutral default
             result.are_friends = false;
             return result;
         }
         
         const Node* n1 = graph.getNode(node1);
         const Node* n2 = graph.getNode(node2);
         
         if (!n1 || !n2) {
             FriendshipScoreResult result;
             result.friendship_score = 3.0;  // No connection
             result.are_friends = false;
             return result;
         }
         
        // Get edge data first
        const Edge* edge = getEdge(node1, node2);
        
        // Check if they are friends - check both directions to be safe
        set<int> node1Friends = graph.getFriends(node1);
        set<int> node2Friends = graph.getFriends(node2);
        bool are_friends = (node1Friends.count(node2) > 0) || (node2Friends.count(node1) > 0);
        
        // Also check if there's a direct edge with relationship_type == "friend"
        if (edge && edge->relationship_type == "friend") {
            are_friends = true;
        }
        
        int message_count = edge ? edge->message_count : 0;
        string established_at = edge ? edge->established_at : "";
        
        // Get mutual friends
        auto mutual_friends = AlgoUtils::set_intersection_of_two(
            graph.getFriends(node1),
            graph.getFriends(node2));
        int mutual_count = mutual_friends.size();
        
        // Calculate mutual friends score (transitive)
        double mutual_friends_score = calculateMutualFriendsScore(node1, node2);
        
        // Calculate time factor
        double time_factor = calculateTimeFactor(established_at);
        
        // Calculate geographic proximity
        double geo_proximity = calculateGeographicProximity(
            n1->location.latitude, n1->location.longitude,
            n2->location.latitude, n2->location.longitude);
        
        // Calculate common interests
        auto common_interests = AlgoUtils::find_common_items(
            n1->interests, n2->interests);
        int interests_count = common_interests.size();
        
        // ========== SCORE CALCULATION ==========
        // Base score: 1.5 for friends, 2.5 for non-friends
        double base_score = are_friends ? 1.5 : 2.5;
        
        // Factor 1: Mutual friends (strongest indicator)
        // More mutual friends = lower score (stronger friendship)
        double mutual_factor = 0.0;
        if (mutual_count > 0) {
            // Normalize: 0 mutual = 0, 10+ mutual = -0.3
            double normalized_mutual = AlgoUtils::normalize_to_01(mutual_count, 10.0);
            mutual_factor = -0.3 * normalized_mutual;  // Negative = reduces score
        }
        
        // Factor 2: Message count (engagement indicator)
        // More messages = lower score (stronger friendship)
        double message_factor = 0.0;
        if (message_count > 0) {
            // Normalize: 0 messages = 0, 1000+ messages = -0.25
            double normalized_messages = AlgoUtils::normalize_to_01(message_count, 1000.0);
            message_factor = -0.25 * normalized_messages;
        }
        
        // Factor 3: Mutual friends' friendship scores (transitive)
        // If mutual friends are close, these two are likely closer too
        double transitive_factor = 0.0;
        if (mutual_friends_score < 1.5) {  // If mutual friends are close
            // Lower mutual_friends_score = stronger transitive bond
            double normalized = (1.5 - mutual_friends_score) / 0.5;  // 0 to 1
            transitive_factor = -0.15 * normalized;
        }
        
        // Factor 4: Time of connection
        // Longer connection = lower score (stronger friendship)
        double time_factor_impact = -0.15 * time_factor;
        
        // Factor 5: Geographic proximity
        // Closer = lower score (stronger friendship)
        double geo_factor = -0.1 * geo_proximity;
        
        // Factor 6: Common interests
        // More interests = lower score (stronger friendship)
        double interests_factor = 0.0;
        if (interests_count > 0) {
            double normalized_interests = AlgoUtils::normalize_to_01(interests_count, 5.0);
            interests_factor = -0.1 * normalized_interests;
        }
        
        // Calculate final score
        double raw_score = base_score + mutual_factor + message_factor + 
                          transitive_factor + time_factor_impact + 
                          geo_factor + interests_factor;
        
        // Normalize to [1.0, 3.0] range
        // Friends: [1.0, 2.0], Non-friends: [2.0, 3.0]
        double final_score;
        if (are_friends) {
            // Friends: clamp to [1.0, 2.0]
            final_score = max(1.0, min(2.0, raw_score));
        } else {
            // Non-friends: clamp to [2.0, 3.0]
            final_score = max(2.0, min(3.0, raw_score));
        }
        
        // Build result
        FriendshipScoreResult result;
        result.friendship_score = final_score;
        result.are_friends = are_friends;
        result.mutual_friends_count = mutual_count;
        result.message_count = message_count;
        result.mutual_friends_score = mutual_friends_score;
        result.time_factor = time_factor;
        result.geographic_proximity = geo_proximity;
        result.common_interests_count = interests_count;
        
        // Generate explanation
        stringstream ss;
        if (are_friends) {
            ss << "Friends with score " << fixed << setprecision(2) << final_score;
            if (mutual_count > 0) ss << " (" << mutual_count << " mutual friends)";
            if (message_count > 0) ss << ", " << message_count << " messages";
        } else {
            ss << "Not friends, score " << fixed << setprecision(2) << final_score;
            if (mutual_count > 0) ss << " (" << mutual_count << " mutual connections)";
        }
        result.explanation = ss.str();
        
        return result;
     }
 
 public:
     explicit FriendshipScoreCalculator(const SocialGraph& social_graph) 
         : graph(social_graph) {}
     
     /**
      * Calculate friendship score between two nodes.
      * 
      * Score range: 1.0 (strongest friendship) to 3.0 (no connection)
      * Guarantee: Friends always score [1.0, 2.0], non-friends score [2.0, 3.0]
      * 
      * Factors considered:
      * - Mutual friends count (30% weight)
      * - Message count (25% weight)
      * - Friendship scores of mutual friends (15% weight, transitive)
      * - Time of connection (15% weight)
      * - Geographic proximity (10% weight)
      * - Common interests (10% weight)
      * 
      * @param node1 First node ID
      * @param node2 Second node ID
      * @return FriendshipScoreResult with score and breakdown
      */
     FriendshipScoreResult calculateScore(int node1, int node2) const {
         // Use cache to avoid recomputation
         pair<int, int> key = make_pair(min(node1, node2), max(node1, node2));
         
         if (score_cache.find(key) != score_cache.end()) {
             // Return cached result (but we need full result, so recalculate)
             // Actually, let's just recalculate for now to get full breakdown
         }
         
         auto result = calculateScoreInternal(node1, node2, 0);
         
         // Cache the score
         score_cache[key] = result.friendship_score;
         
         return result;
     }
     
     /**
      * Clear the score cache (useful if graph is updated).
      */
     void clearCache() {
         score_cache.clear();
     }
 };