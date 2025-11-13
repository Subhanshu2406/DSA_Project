#pragma once

#include "../graph_generator.hpp"
#include "pagerank.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <cmath>

using namespace std;

struct InfluencerRank {
    int rank = 0;
    int user_id = -1;
    string user_name;
    double influence_score = 0.0;
    int fan_count = 0;
    int friend_count = 0;
    double pagerank_score = 0.0;
    int total_followers = 0;
    vector<int> top_fan_ids;
};

class InfluencerRanker {
private:
    const SocialGraph& graph;
    PageRankCalculator pagerank_calculator;

    static double clamp_score(double value, double max_value) {
        if (max_value <= 0.0) return 0.0;
        return min(1.0, value / max_value);
    }

public:
    explicit InfluencerRanker(const SocialGraph& social_graph)
        : graph(social_graph), pagerank_calculator(social_graph) {}

    vector<InfluencerRank> get_leaderboard(
    int top_k = 10,
    int pagerank_iterations = 20) const
{
    if (graph.getNodeCount() == 0 || top_k <= 0)
        return {};

    auto pagerank_scores = pagerank_calculator.calculate(0.85, pagerank_iterations);

    // Min-heap of size <= top_k
    auto cmp = [](const InfluencerRank& a, const InfluencerRank& b) {
        return a.influence_score > b.influence_score;   // min-heap
    };

    // using priority_queue to always keep the top K(required) so time complexity doesn't get sky-rocketed
    priority_queue<
        InfluencerRank,
        vector<InfluencerRank>,
        decltype(cmp)
    > min_heap(cmp);

    for (const auto& [user_id, node] : graph.getNodes()) {
        InfluencerRank entry;
        entry.user_id = user_id;
        entry.user_name = node.name;

        auto followers = graph.getFollowers(user_id);
        auto friends   = graph.getFriends(user_id);

        entry.friend_count     = friends.size();
        entry.total_followers  = followers.size();

        for (int follower_id : followers) {
            if (!friends.count(follower_id)) {
                entry.fan_count++;
                if (entry.top_fan_ids.size() < 5)
                    entry.top_fan_ids.push_back(follower_id);
            }
        }

        entry.pagerank_score = pagerank_scores.count(user_id)
                                ? pagerank_scores.at(user_id)
                                : 0.0;

        double follower_component = clamp_score(entry.total_followers, 1000.0) * 30.0;
        double fan_component      = clamp_score(entry.fan_count,      500.0) * 35.0;
        double friend_component   = clamp_score(entry.friend_count,   500.0) * 15.0;
        double pagerank_component = min(25.0, entry.pagerank_score * 100.0 * 0.25);

        entry.influence_score = follower_component +
                                fan_component +
                                friend_component +
                                pagerank_component;

        // Push to heap
        if ((int)min_heap.size() < top_k) {
            min_heap.push(entry);
        } else if (entry.influence_score > min_heap.top().influence_score) {
            min_heap.pop();
            min_heap.push(entry);
        }
    }

    // Extract top K from heap (they are unsorted)
    vector<InfluencerRank> leaderboard;
    leaderboard.reserve(top_k);
    while (!min_heap.empty()) {
        leaderboard.push_back(min_heap.top());
        min_heap.pop();
    }

    // Now sort only top K (small cost)
    sort(leaderboard.begin(), leaderboard.end(),
         [](const InfluencerRank& a, const InfluencerRank& b) {
             if (a.influence_score != b.influence_score)
                 return a.influence_score > b.influence_score;
             return a.user_id < b.user_id;
         });

    // Assign ranks
        for (int i = 0; i < leaderboard.size(); i++)
            leaderboard[i].rank = i + 1;

        return leaderboard;
    }

};
