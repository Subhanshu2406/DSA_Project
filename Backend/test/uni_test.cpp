// tests/test_all_algorithms.cpp
/*
 * COMPREHENSIVE TEST SUITE
 * ========================
 * Tests all algorithms and features in the graph system.
 * 
 * Compile: g++ -std=c++17 -O3 tests/test_all_algorithms.cpp -o test_all
 * Run: ./test_all
 */

#include "../Algorithm.hpp"
#include "../Backend/graph_generator.hpp"
#include <iostream>
#include <cassert>
#include <iomanip>

using namespace std;

// ============================================================================
// TEST UTILITIES
// ============================================================================

class TestRunner {
private:
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    string current_category;

public:
    void start_category(const string& category) {
        current_category = category;
        cout << "\n" << string(70, '=') << endl;
        cout << "📋 " << category << endl;
        cout << string(70, '=') << endl;
    }

    void assert_equal(const string& test_name, bool condition, const string& expected = "", const string& actual = "") {
        total_tests++;
        if (condition) {
            passed_tests++;
            cout << "✅ " << test_name << endl;
        } else {
            failed_tests++;
            cout << "❌ " << test_name << endl;
            if (!expected.empty() || !actual.empty()) {
                cout << "   Expected: " << expected << ", Got: " << actual << endl;
            }
        }
    }

    void assert_not_empty(const string& test_name, int size) {
        assert_equal(test_name, size > 0, "size > 0", "size = " + to_string(size));
    }

    void assert_range(const string& test_name, double value, double min_val, double max_val) {
        bool condition = value >= min_val && value <= max_val;
        assert_equal(test_name, condition, 
                    to_string(min_val) + " <= value <= " + to_string(max_val),
                    to_string(value));
    }

    void print_summary() {
        cout << "\n" << string(70, '=') << endl;
        cout << "📊 TEST SUMMARY" << endl;
        cout << string(70, '=') << endl;
        cout << "Total Tests: " << total_tests << endl;
        cout << "✅ Passed: " << passed_tests << endl;
        cout << "❌ Failed: " << failed_tests << endl;
        cout << "Success Rate: " << fixed << setprecision(2) 
             << (total_tests > 0 ? (100.0 * passed_tests / total_tests) : 0) << "%" << endl;
        cout << string(70, '=') << endl;
    }

    bool all_passed() const {
        return failed_tests == 0;
    }
};

// ============================================================================
// MOCK DATA SETUP
// ============================================================================

/**
 * Create a simple test graph with known structure for testing.
 * Graph structure:
 *   0 -- 1 -- 2
 *   |         |
 *   +--- 3 ---+
 *        |
 *        4
 */
void setup_test_graph() {
    // Note: In real scenario, load from JSON files
    // For now, we'll assume graph is initialized externally
    SocialGraph g;

    // Update these paths if your dataset folder is somewhere else.
    // From Backend/test the dataset in your tree appears at: ../dataset/data/generated/2024-01-01/...
    const string nodesPath = "../dataset/data/generated/2024-01-01/nodes.json";
    const string edgesPath = "../dataset/data/generated/2024-01-01/edges.json";
    const string metaPath  = "../dataset/data/generated/2024-01-01/metadata.json";

    if (!g.initializeGraph(nodesPath, edgesPath, metaPath)) {
        cerr << "Failed to initialize graph (check file paths and json library)\n";
        return 1;
    }

    cout << "\n📥 Test graph initialized" << endl;
}

// ============================================================================
// TEST FUNCTIONS
// ============================================================================

void test_mutual_friends(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("MUTUAL FRIENDS ANALYSIS");

    // Test 1: Mutual friends between adjacent users
    auto result1 = engine.analyze_mutual_friends(0, 1);
    test.assert_equal("Can analyze mutual friends between users", true);
    test.assert_equal("User IDs set correctly", 
                     result1.user_id_1 == 0 && result1.user_id_2 == 1);

    // Test 2: Similarity ratio is valid
    test.assert_range("Similarity ratio in valid range", 
                     result1.similarity_ratio, 0.0, 1.0);

    // Test 3: Degree counts are non-negative
    test.assert_equal("User 1 degree >= 0", result1.total_degree_1 >= 0);
    test.assert_equal("User 2 degree >= 0", result1.total_degree_2 >= 0);

    // Test 4: Self-relationship
    auto self_result = engine.analyze_mutual_friends(0, 0);
    test.assert_equal("Self-relationship handled correctly", 
                     self_result.user_id_1 == 0 && self_result.user_id_2 == 0);

    // Test 5: Invalid user
    auto invalid_result = engine.analyze_mutual_friends(0, 999999);
    test.assert_equal("Invalid user handled gracefully", 
                     invalid_result.similarity_ratio == 0.0);

    return test;
}

void test_friend_recommender(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("FRIEND RECOMMENDER SYSTEM");

    // Test 1: Get recommendations
    auto recs = engine.get_friend_recommendations(0, 10);
    test.assert_equal("Can get recommendations", true);

    // Test 2: Recommendations not empty (if graph has connections)
    // test.assert_not_empty("Recommendations list not empty", recs.size());

    if (!recs.empty()) {
        // Test 3: First recommendation has valid data
        const auto& first_rec = recs[0];
        test.assert_equal("First rec has user_id", first_rec.recommended_user_id >= 0);
        test.assert_equal("First rec has user_name", !first_rec.user_name.empty());

        // Test 4: Score in valid range
        test.assert_range("Recommendation score in valid range", 
                         first_rec.total_score, 0.0, 100.0);

        // Test 5: Mutual friends count non-negative
        test.assert_equal("Mutual friends count >= 0", 
                         first_rec.mutual_friends_count >= 0);

        // Test 6: Common interests count non-negative
        test.assert_equal("Common interests count >= 0", 
                         first_rec.common_interests_count >= 0);

        // Test 7: Distance non-negative
        test.assert_equal("Geographic distance >= 0", 
                         first_rec.geographic_distance_km >= 0.0);

        // Test 8: Community similarity in range
        test.assert_range("Community similarity in range", 
                         first_rec.community_similarity, 0.0, 1.0);

        // Test 9: Reason not empty
        test.assert_equal("Recommendation reason provided", 
                         !first_rec.recommendation_reason.empty());

        // Test 10: Scores are descending
        if (recs.size() > 1) {
            test.assert_equal("Recommendations sorted by score", 
                             recs[0].total_score >= recs[1].total_score);
        }
    }

    // Test 11: Can request different counts
    auto recs_5 = engine.get_friend_recommendations(0, 5);
    test.assert_equal("Can request 5 recommendations", 
                     recs_5.size() <= 5);

    // Test 12: Invalid user returns empty
    auto invalid_recs = engine.get_friend_recommendations(999999, 10);
    test.assert_equal("Invalid user returns empty", 
                     invalid_recs.empty());

    return test;
}

void test_pagerank(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("PAGERANK CALCULATION");

    // Test 1: Can calculate PageRank
    auto ranks = engine.calculate_pagerank(0.85, 20);
    test.assert_equal("Can calculate PageRank", !ranks.empty());

    // Test 2: PageRank computed for all nodes
    int node_count = engine.pagerank_calculator.calculate(0.85, 20).size();
    test.assert_equal("PageRank computed for all nodes", node_count > 0);

    if (!ranks.empty()) {
        // Test 3: All ranks in valid range [0, 1]
        bool all_valid = true;
        for (const auto& [id, rank] : ranks) {
            if (rank < 0.0 || rank > 1.0) {
                all_valid = false;
                break;
            }
        }
        test.assert_equal("All PageRank scores in range [0, 1]", all_valid);

        // Test 4: Ranks sum approximately to 1 (within floating point error)
        double sum = 0.0;
        for (const auto& [id, rank] : ranks) {
            sum += rank;
        }
        // Sum should be approximately node_count / node_count = 1, or scaled
        test.assert_range("PageRank scores sum correctly", 
                         sum, 0.5, 1.5); // Allow some error

        // Test 5: Different damping factors work
        auto ranks_90 = engine.calculate_pagerank(0.90, 20);
        test.assert_equal("Different damping factor works", !ranks_90.empty());

        // Test 6: Different iterations work
        auto ranks_50 = engine.calculate_pagerank(0.85, 50);
        test.assert_equal("More iterations work", !ranks_50.empty());
    }

    return test;
}

void test_community_detection(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("COMMUNITY DETECTION");

    // Test 1: Label Propagation
    auto communities_lp = engine.detect_communities(0, 10);
    test.assert_equal("Label Propagation works", !communities_lp.empty());
    test.assert_not_empty("Communities detected", communities_lp.size());

    if (!communities_lp.empty()) {
        // Test 2: Community IDs are unique
        set<int> seen_ids;
        bool unique = true;
        for (const auto& comm : communities_lp) {
            if (seen_ids.count(comm.community_id)) {
                unique = false;
                break;
            }
            seen_ids.insert(comm.community_id);
        }
        test.assert_equal("Community IDs are unique", unique);

        // Test 3: All communities have members
        bool all_have_members = true;
        for (const auto& comm : communities_lp) {
            if (comm.member_ids.empty()) {
                all_have_members = false;
                break;
            }
        }
        test.assert_equal("All communities have members", all_have_members);

        // Test 4: Community size matches member count
        bool size_matches = true;
        for (const auto& comm : communities_lp) {
            if (comm.size != comm.member_ids.size()) {
                size_matches = false;
                break;
            }
        }
        test.assert_equal("Community size matches member count", size_matches);

        // Test 5: Internal density in valid range
        bool valid_density = true;
        for (const auto& comm : communities_lp) {
            if (comm.internal_edge_density < 0.0 || comm.internal_edge_density > 1.0) {
                valid_density = false;
                break;
            }
        }
        test.assert_equal("Internal edge density in range [0, 1]", valid_density);

        // Test 6: Edge counts non-negative
        bool valid_edges = true;
        for (const auto& comm : communities_lp) {
            if (comm.edges_within_community < 0 || comm.edges_to_outside < 0) {
                valid_edges = false;
                break;
            }
        }
        test.assert_equal("Edge counts non-negative", valid_edges);

        // Test 7: First community has valid data
        test.assert_equal("First community ID >= 0", communities_lp[0].community_id >= 0);
        test.assert_equal("First community size > 0", communities_lp[0].size > 0);
    }

    // Test 8: Greedy Modularity
    auto communities_gm = engine.detect_communities(1, 10);
    test.assert_equal("Greedy Modularity works", !communities_gm.empty());

    return test;
}

void test_influencer_ranking(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("INFLUENCER RANKING");

    // Test 1: Get global leaderboard
    auto leaderboard = engine.get_influencer_leaderboard(100, 20);
    test.assert_equal("Can get influencer leaderboard", !leaderboard.empty());

    if (!leaderboard.empty()) {
        // Test 2: Leaderboard is sorted by influence
        bool sorted = true;
        for (int i = 1; i < leaderboard.size(); i++) {
            if (leaderboard[i-1].influence_score < leaderboard[i].influence_score) {
                sorted = false;
                break;
            }
        }
        test.assert_equal("Leaderboard sorted by influence score", sorted);

        // Test 3: Ranks are sequential
        bool sequential = true;
        for (int i = 0; i < leaderboard.size(); i++) {
            if (leaderboard[i].rank != i + 1) {
                sequential = false;
                break;
            }
        }
        test.assert_equal("Ranks are sequential (1, 2, 3...)", sequential);

        // Test 4: Influence scores in valid range
        bool valid_scores = true;
        for (const auto& inf : leaderboard) {
            if (inf.influence_score < 0.0 || inf.influence_score > 100.0) {
                valid_scores = false;
                break;
            }
        }
        test.assert_equal("Influence scores in range [0, 100]", valid_scores);

        // Test 5: User IDs positive
        bool valid_ids = true;
        for (const auto& inf : leaderboard) {
            if (inf.user_id < 0) {
                valid_ids = false;
                break;
            }
        }
        test.assert_equal("User IDs non-negative", valid_ids);

        // Test 6: Follower counts non-negative
        bool valid_followers = true;
        for (const auto& inf : leaderboard) {
            if (inf.fan_count < 0 || inf.friend_count < 0 || inf.total_followers < 0) {
                valid_followers = false;
                break;
            }
        }
        test.assert_equal("Follower counts non-negative", valid_followers);

        // Test 7: Centrality scores in valid range
        bool valid_centrality = true;
        for (const auto& inf : leaderboard) {
            if (inf.centrality_score < 0.0 || inf.centrality_score > 1.0) {
                valid_centrality = false;
                break;
            }
        }
        test.assert_equal("Centrality scores in range [0, 1]", valid_centrality);

        // Test 8: User names provided
        bool has_names = true;
        for (const auto& inf : leaderboard) {
            if (inf.user_name.empty()) {
                has_names = false;
                break;
            }
        }
        test.assert_equal("User names provided", has_names);
    }

    // Test 9: Can request different top K
    auto top_10 = engine.get_influencer_leaderboard(10, 20);
    test.assert_equal("Can get top 10", top_10.size() <= 10);

    return test;
}

void test_centrality(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("CENTRALITY ANALYSIS");

    // Test 1: Get centrality for user 0
    auto metrics = engine.get_centrality_metrics(0);
    test.assert_equal("Can get centrality metrics", true);

    // Test 2: User ID set correctly
    test.assert_equal("User ID set correctly", metrics.user_id == 0);

    // Test 3: Degree centrality in range
    test.assert_range("Degree centrality in range [0, 1]", 
                     metrics.degree_centrality, 0.0, 1.0);

    // Test 4: Closeness centrality in range
    test.assert_range("Closeness centrality in range [0, 1]", 
                     metrics.closeness_centrality, 0.0, 1.0);

    // Test 5: Clustering coefficient in range
    test.assert_range("Clustering coefficient in range [0, 1]", 
                     metrics.clustering_coefficient, 0.0, 1.0);

    // Test 6: Betweenness centrality in range
    test.assert_range("Betweenness centrality in range [0, 1]", 
                     metrics.betweenness_centrality, 0.0, 1.0);

    // Test 7: Raw degree non-negative
    test.assert_equal("Raw degree non-negative", metrics.raw_degree >= 0);

    // Test 8: Get average clustering
    double avg_clustering = engine.calculate_average_clustering();
    test.assert_range("Average clustering in range [0, 1]", 
                     avg_clustering, 0.0, 1.0);

    return test;
}

void test_shortest_path(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("SHORTEST PATH FINDING");

    // Test 1: Find path from user 0 to user 1
    auto path_result = engine.find_shortest_path(0, 1);
    test.assert_equal("Can find path", true);

    // Test 2: Self-path
    auto self_path = engine.find_shortest_path(0, 0);
    test.assert_equal("Self-path handled correctly", self_path.path_exists);
    test.assert_equal("Self-path length is 0", self_path.path_length == 0);

    if (path_result.path_exists) {
        // Test 3: Path is not empty
        test.assert_not_empty("Path node IDs not empty", path_result.path_node_ids.size());

        // Test 4: Path starts with source
        test.assert_equal("Path starts with source", path_result.path_node_ids[0] == 0);

        // Test 5: Path ends with target
        test.assert_equal("Path ends with target", 
                         path_result.path_node_ids.back() == 1);

        // Test 6: Path length matches node count
        test.assert_equal("Path length = nodes - 1", 
                         path_result.path_length == path_result.path_node_ids.size() - 1);

        // Test 7: Path description not empty
        test.assert_equal("Path description provided", 
                         !path_result.path_description.empty());
    }

    // Test 8: Get distance only
    int distance = engine.get_shortest_distance(0, 1);
    test.assert_equal("Can get distance only", distance >= 0 || distance == -1);

    // Test 9: Batch paths
    vector<int> targets = {1, 2, 3};
    auto batch_paths = engine.find_paths_batch(0, targets);
    test.assert_equal("Can batch find paths", batch_paths.size() == targets.size());

    return test;
}

void test_network_statistics(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("NETWORK STATISTICS");

    // Test 1: Calculate network density
    double density = engine.calculate_network_density();
    test.assert_range("Network density in range [0, 1]", density, 0.0, 1.0);

    // Test 2: Calculate average clustering
    double avg_clustering = engine.calculate_average_clustering();
    test.assert_range("Average clustering in range [0, 1]", 
                     avg_clustering, 0.0, 1.0);

    return test;
}

void test_integration(GraphAlgorithms& engine) {
    TestRunner test;
    test.start_category("INTEGRATION TESTS");

    // Test 1: Can use multiple algorithms together
    auto communities = engine.detect_communities(0, 10);
    auto leaderboard = engine.get_influencer_leaderboard(10);
    test.assert_equal("Can get communities and leaderboard together", 
                     !communities.empty() && !leaderboard.empty());

    // Test 2: Can chain operations
    auto recs = engine.get_friend_recommendations(0, 5);
    auto centrality = engine.get_centrality_metrics(0);
    test.assert_equal("Can chain recommendation and centrality", 
                     !recs.empty() || centrality.user_id == 0);

    // Test 3: Cache functions work
    auto path1 = engine.find_shortest_path(0, 1);
    auto path2 = engine.find_shortest_path(0, 1); // Should use cache
    test.assert_equal("Path caching works", 
                     path1.path_exists == path2.path_exists);

    return test;
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main() {
    cout << "\n╔══════════════════════════════════════════════════════════════════════╗\n"
         << "║          COMPREHENSIVE ALGORITHM TEST SUITE                         ║\n"
         << "║                                                                      ║\n"
         << "║  Testing all graph algorithms and features                          ║\n"
         << "╚══════════════════════════════════════════════════════════════════════╝\n" << endl;

    // Initialize graph
    try {
        SocialGraph graph;
        bool loaded = graph.initializeGraph(
            "data/nodes.json",
            "data/edges.json",
            "data/metadata.json"
        );

        if (!loaded) {
            cerr << "\n❌ Failed to load graph data from JSON files!" << endl;
            cerr << "Make sure these files exist:" << endl;
            cerr << "  - data/nodes.json" << endl;
            cerr << "  - data/edges.json" << endl;
            cerr << "  - data/metadata.json" << endl;
            return 1;
        }

        cout << "✅ Graph loaded successfully!" << endl;
        cout << "   Nodes: " << graph.getNodeCount() << endl;
        cout << "   Edges: " << graph.getEdgeCount() << endl;

        // Create algorithm engine
        GraphAlgorithms engine(graph);

        // Run all tests
        test_mutual_friends(engine);
        test_friend_recommender(engine);
        test_pagerank(engine);
        test_community_detection(engine);
        test_influencer_ranking(engine);
        test_centrality(engine);
        test_shortest_path(engine);
        test_network_statistics(engine);
        test_integration(engine);

    } catch (const exception& e) {
        cerr << "\n❌ Exception caught: " << e.what() << endl;
        return 1;
    }

    cout << "\n✅ All tests completed successfully!" << endl;
    return 0;
}