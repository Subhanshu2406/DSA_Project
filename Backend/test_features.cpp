#include <iostream>
#include <iomanip>
#include "Algorithm.hpp"
#include "graph_generator.hpp"

using namespace std;

void print_separator(const string& title) {
    cout << "\n" << string(60, '=') << endl;
    cout << "  " << title << endl;
    cout << string(60, '=') << endl;
}

void test_user_search(const GraphAlgorithms& engine) {
    print_separator("TESTING USER SEARCH (AUTOCOMPLETE)");
    
    if (!engine.is_search_ready()) {
        cout << "❌ Search index not ready!" << endl;
        return;
    }
    
    cout << "✓ Search index is ready" << endl;
    
    // Test search
    vector<string> test_queries = {"A", "B", "User", "Test"};
    
    for (const auto& query : test_queries) {
        auto results = engine.search_users_with_names(query, 5);
        cout << "\nQuery: '" << query << "' -> Found " << results.size() << " results:" << endl;
        for (const auto& [id, name] : results) {
            cout << "  - ID: " << id << ", Name: " << name << endl;
        }
    }
}

void test_mutual_friends(const GraphAlgorithms& engine) {
    print_separator("TESTING MUTUAL FRIENDS");
    
    // Test with first few users
    vector<int> test_users = {0, 1, 2, 3, 4};
    
    for (size_t i = 0; i < test_users.size() - 1; i++) {
        int user1 = test_users[i];
        int user2 = test_users[i + 1];
        
        auto result = engine.analyze_mutual_friends(user1, user2);
        cout << "Users " << user1 << " and " << user2 << ":" << endl;
        cout << "  Mutual friends: " << result.mutual_ids.size() << endl;
        cout << "  Similarity ratio: " << fixed << setprecision(3) << result.similarity_ratio << endl;
        cout << "  User " << user1 << " has " << result.total_degree_1 << " friends" << endl;
        cout << "  User " << user2 << " has " << result.total_degree_2 << " friends" << endl;
    }
}

void test_friend_recommendations(const GraphAlgorithms& engine) {
    print_separator("TESTING FRIEND RECOMMENDATIONS");
    
    int test_user = 0;
    auto recommendations = engine.get_friend_recommendations(test_user, 5);
    
    cout << "Top 5 recommendations for user " << test_user << ":" << endl;
    for (const auto& rec : recommendations) {
        cout << "\n  User: " << rec.user_name << " (ID: " << rec.recommended_user_id << ")" << endl;
        cout << "    Score: " << fixed << setprecision(2) << rec.total_score << "/100" << endl;
        cout << "    Mutual friends: " << rec.mutual_friends_count << endl;
        cout << "    Common interests: " << rec.common_interests_count << endl;
        cout << "    Distance: " << fixed << setprecision(2) << rec.geographic_distance_km << " km" << endl;
        cout << "    Reason: " << rec.recommendation_reason << endl;
    }
}

void test_shortest_path(const GraphAlgorithms& engine) {
    print_separator("TESTING SHORTEST PATH");
    
    vector<pair<int, int>> test_pairs = {{0, 5}, {1, 10}, {2, 15}};
    
    for (const auto& [source, target] : test_pairs) {
        auto result = engine.find_shortest_path(source, target);
        cout << "Path from " << source << " to " << target << ":" << endl;
        if (result.path_exists) {
            cout << "  ✓ Path exists (length: " << result.path_length << ")" << endl;
            cout << "  Path: " << result.path_description << endl;
        } else {
            cout << "  ✗ No path found" << endl;
        }
    }
}

void test_community_detection(const GraphAlgorithms& engine) {
    print_separator("TESTING COMMUNITY DETECTION");
    
    auto communities = engine.detect_communities(0, 10); // Label propagation
    
    cout << "Found " << communities.size() << " communities:" << endl;
    for (size_t i = 0; i < min(5, (int)communities.size()); i++) {
        const auto& comm = communities[i];
        cout << "\n  Community " << comm.community_id << ":" << endl;
        cout << "    Size: " << comm.size << " members" << endl;
        cout << "    Internal density: " << fixed << setprecision(3) << comm.internal_edge_density << endl;
        cout << "    Edges within: " << comm.edges_within_community << endl;
        cout << "    Edges to outside: " << comm.edges_to_outside << endl;
    }
}

void test_pagerank(const GraphAlgorithms& engine) {
    print_separator("TESTING PAGERANK");
    
    auto pagerank = engine.calculate_pagerank(0.85, 10);
    
    // Get top 5 by pagerank
    vector<pair<int, double>> sorted_ranks;
    for (const auto& [id, score] : pagerank) {
        sorted_ranks.push_back({id, score});
    }
    sort(sorted_ranks.begin(), sorted_ranks.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });
    
    cout << "Top 5 users by PageRank:" << endl;
    for (size_t i = 0; i < min(5, (int)sorted_ranks.size()); i++) {
        cout << "  " << (i + 1) << ". User " << sorted_ranks[i].first 
             << " (score: " << fixed << setprecision(6) << sorted_ranks[i].second << ")" << endl;
    }
}

void test_influencer_ranking(const GraphAlgorithms& engine) {
    print_separator("TESTING INFLUENCER RANKING");
    
    auto leaderboard = engine.get_influencer_leaderboard(5, 10);
    
    cout << "Top 5 influencers:" << endl;
    for (const auto& entry : leaderboard) {
        cout << "\n  Rank " << entry.rank << ": " << entry.user_name 
             << " (ID: " << entry.user_id << ")" << endl;
        cout << "    Influence score: " << fixed << setprecision(2) << entry.influence_score << "/100" << endl;
        cout << "    Total followers: " << entry.total_followers << endl;
        cout << "    Fan count: " << entry.fan_count << endl;
        cout << "    Friend count: " << entry.friend_count << endl;
        cout << "    PageRank score: " << fixed << setprecision(6) << entry.pagerank_score << endl;
    }
}

void test_centrality(const GraphAlgorithms& engine) {
    print_separator("TESTING CENTRALITY ANALYSIS");
    
    vector<int> test_users = {0, 1, 2};
    
    for (int user_id : test_users) {
        auto metrics = engine.get_centrality_metrics(user_id);
        cout << "\nUser " << user_id << " centrality metrics:" << endl;
        cout << "  Degree centrality: " << fixed << setprecision(4) << metrics.degree_centrality << endl;
        cout << "  Closeness centrality: " << fixed << setprecision(4) << metrics.closeness_centrality << endl;
        cout << "  Clustering coefficient: " << fixed << setprecision(4) << metrics.clustering_coefficient << endl;
        cout << "  Raw degree: " << metrics.raw_degree << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "\n" << string(60, '=') << endl;
    cout << "  FEATURE TESTING SUITE" << endl;
    cout << string(60, '=') << endl;
    
    // Initialize graph
    SocialGraph graph;
    
    string nodes_path, edges_path, metadata_path;
    
    if (argc >= 4) {
        nodes_path = argv[1];
        edges_path = argv[2];
        metadata_path = argv[3];
    } else {
        // Default paths - adjust these to your dataset
        nodes_path = "dataset/data/generated/2024-01-01/nodes.json";
        edges_path = "dataset/data/generated/2024-01-01/edges.json";
        metadata_path = "dataset/data/generated/2024-01-01/metadata.json";
    }
    
    cout << "\nLoading graph from:" << endl;
    cout << "  Nodes: " << nodes_path << endl;
    cout << "  Edges: " << edges_path << endl;
    cout << "  Metadata: " << metadata_path << endl;
    
    if (!graph.initializeGraph(nodes_path, edges_path, metadata_path)) {
        cerr << "\n❌ ERROR: Failed to initialize graph!" << endl;
        cerr << "Usage: " << argv[0] << " [nodes.json] [edges.json] [metadata.json]" << endl;
        return 1;
    }
    
    cout << "\n✓ Graph loaded successfully!" << endl;
    graph.printStatistics();
    
    // Create algorithm engine
    GraphAlgorithms engine(graph);
    
    // Run all tests
    test_user_search(engine);
    test_mutual_friends(engine);
    test_friend_recommendations(engine);
    test_shortest_path(engine);
    test_community_detection(engine);
    test_pagerank(engine);
    test_influencer_ranking(engine);
    test_centrality(engine);
    
    // Network statistics
    print_separator("NETWORK STATISTICS");
    cout << "Network density: " << fixed << setprecision(6) 
         << engine.calculate_network_density() << endl;
    cout << "Average clustering coefficient: " << fixed << setprecision(6)
         << engine.calculate_average_clustering() << endl;
    
    cout << "\n" << string(60, '=') << endl;
    cout << "  ALL TESTS COMPLETE" << endl;
    cout << string(60, '=') << endl;
    
    return 0;
}

