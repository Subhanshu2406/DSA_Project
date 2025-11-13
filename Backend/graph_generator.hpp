// include/graph_generator.h
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>

using json = nlohmann::json;
using namespace std;

// ============== DATA STRUCTURES ==============
struct Location {
    double latitude;
    double longitude;
};

struct Node {
    int user_id;
    string name;
    Location location;
    int region_id;
    vector<string> interests;
    string created_at;
    set<int> neighbors;  // Adjacency list
};

struct Edge {
    int source;
    int target;
    string relationship_type;
    int message_count;
    string last_interaction;
    double distance;
    string established_at;
};

struct GraphMetadata {
    string date;
    int total_nodes;
    int total_edges;
    int friend_relationships;
    int fan_relationships;
    double average_degree;
};

// ============== GRAPH CLASS ==============
class SocialGraph {
private:
    unordered_map<int, Node> nodes;
    vector<Edge> edges;
    GraphMetadata metadata;
    string last_update;

public:
    SocialGraph() = default;

    // Load nodes from JSON file
    bool loadNodesFromJSON(const string& filepath) {
        try {
            ifstream file(filepath);
            if (!file.is_open()) {
                cerr << "Cannot open nodes file: " << filepath << endl;
                return false;
            }

            json nodesArray;
            file >> nodesArray;

            for (const auto& nodeJSON : nodesArray) {
                Node node;
                node.user_id = nodeJSON["user_id"];
                node.name = nodeJSON["name"];
                node.location.latitude = nodeJSON["location"][0];
                node.location.longitude = nodeJSON["location"][1];
                node.region_id = nodeJSON["region_id"];

                for (const auto& interest : nodeJSON["interests"]) {
                    node.interests.push_back(interest);
                }

                node.created_at = nodeJSON["created_at"];
                nodes[node.user_id] = node;
            }

            cout << "✓ Loaded " << nodes.size() << " nodes" << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error loading nodes: " << e.what() << endl;
            return false;
        }
    }

    // Load edges from JSON file
    bool loadEdgesFromJSON(const string& filepath) {
        try {
            ifstream file(filepath);
            if (!file.is_open()) {
                cerr << "Cannot open edges file: " << filepath << endl;
                return false;
            }

            json edgesArray;
            file >> edgesArray;
            edges.clear();

            for (const auto& edgeJSON : edgesArray) {
                Edge edge;

                edge.source = edgeJSON.value("source", -1);
                edge.target = edgeJSON.value("target", -1);
                edge.relationship_type = edgeJSON.value("relationship_type", "");
                edge.message_count     = edgeJSON.value("message_count", 0);

                // Handle null values for string fields
                edge.last_interaction  = edgeJSON.contains("last_interaction") && !edgeJSON["last_interaction"].is_null() 
                                         ? edgeJSON["last_interaction"].get<string>() : "";
                edge.distance          = edgeJSON.value("distance", 0.0);
                edge.established_at    = edgeJSON.value("established_at", "");
                
                // edge.relationship_type =
                //     edgeJSON.contains("relationship_type") && edgeJSON["relationship_type"].is_string()
                //     ? edgeJSON["relationship_type"].get<string>()
                //     : "";

                edge.last_interaction =
                    edgeJSON.contains("last_interaction") && edgeJSON["last_interaction"].is_string()
                    ? edgeJSON["last_interaction"].get<string>()
                    : "";

                //  edge.established_at =
                //     edgeJSON.contains("established_at") && edgeJSON["established_at"].is_string()
                //     ? edgeJSON["established_at"].get<string>()
                //     : "";

                edges.push_back(edge);

                // Update adjacency list - only if both nodes exist
                if (nodes.find(edge.source) != nodes.end() && 
                    nodes.find(edge.target) != nodes.end()) {
                    nodes[edge.source].neighbors.insert(edge.target);
                    nodes[edge.target].neighbors.insert(edge.source);
                }
            }

            cout << "✓ Loaded " << edges.size() << " edges" << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error loading edges: " << e.what() << endl;
            return false;
        }
    }

    // Load metadata from JSON file
    bool loadMetadataFromJSON(const string& filepath) {
        try {
            ifstream file(filepath);
            if (!file.is_open()) {
                cerr << "Cannot open metadata file: " << filepath << endl;
                return false;
            }

            json metadataJSON;
            file >> metadataJSON;

            metadata.date = metadataJSON["date"];
            metadata.total_nodes = metadataJSON["total_nodes"];
            metadata.total_edges = metadataJSON["total_edges"];
            metadata.friend_relationships = metadataJSON["friend_relationships"];
            metadata.fan_relationships = metadataJSON["fan_relationships"];
            metadata.average_degree = metadataJSON["average_degree"];

            cout << "✓ Loaded metadata" << endl;
            return true;
        } catch (const exception& e) {
            cerr << "Error loading metadata: " << e.what() << endl;
            return false;
        }
    }

    // ========== MAIN PUBLIC METHODS FOR EXTERNAL USE ==========

    // Initialize graph from JSON files (CALL THIS FIRST FROM FRONTEND)
    bool initializeGraph(const string& nodesFile, 
                        const string& edgesFile, 
                        const string& metadataFile) {
        cout << "\n=== Initializing Graph ===" << endl;
        
        if (!loadNodesFromJSON(nodesFile)) return false;
        if (!loadEdgesFromJSON(edgesFile)) return false;
        if (!loadMetadataFromJSON(metadataFile)) return false;

        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        last_update = ctime(&time);

        cout << "✓ Graph initialized successfully" << endl;
        return true;
    }

    // Update graph with new data (CALL THIS DAILY FROM FRONTEND)
    // How it works:
    // 1. Stores backup of current state in case update fails
    // 2. Clears adjacency lists in all nodes (neighbors set)
    // 3. Loads new nodes - NEW NODES are added, EXISTING nodes are updated
    // 4. Loads new edges - rebuilds all adjacency relationships
    // 5. If any step fails, restores from backup (transaction-like behavior)
    // This way: if dataset adds new users or modifies existing ones, graph adapts
    bool updateGraph(const string& nodesFile, 
                    const string& edgesFile, 
                    const string& metadataFile) {
        cout << "\n=== Updating Graph ===" << endl;
        
        // Backup current state
        auto nodesBackup = nodes;
        auto edgesBackup = edges;
        auto metadataBackup = metadata;

        try {
            // Clear neighbors but keep nodes - for fresh rebuild
            for (auto& [id, node] : nodes) {
                node.neighbors.clear();
            }

            // Load new data
            if (!loadNodesFromJSON(nodesFile)) {
                nodes = nodesBackup;
                edges = edgesBackup;
                metadata = metadataBackup;
                cout << "✗ Failed to load nodes, reverting to previous state" << endl;
                return false;
            }

            if (!loadEdgesFromJSON(edgesFile)) {
                nodes = nodesBackup;
                edges = edgesBackup;
                metadata = metadataBackup;
                cout << "✗ Failed to load edges, reverting to previous state" << endl;
                return false;
            }

            if (!loadMetadataFromJSON(metadataFile)) {
                nodes = nodesBackup;
                edges = edgesBackup;
                metadata = metadataBackup;
                cout << "✗ Failed to load metadata, reverting to previous state" << endl;
                return false;
            }

            auto now = chrono::system_clock::now();
            auto time = chrono::system_clock::to_time_t(now);
            last_update = ctime(&time);

            cout << "✓ Graph updated successfully" << endl;
            return true;
        } catch (const exception& e) {
            nodes = nodesBackup;
            edges = edgesBackup;
            metadata = metadataBackup;
            cerr << "✗ Exception during update: " << e.what() << endl;
            return false;
        }
    }

    // ========== ACCESSOR METHODS FOR ALGORITHMS ==========

    const Node* getNode(int user_id) const {
        auto it = nodes.find(user_id);
        return (it != nodes.end()) ? &it->second : nullptr;
    }

    vector<Edge> getEdgesForNode(int user_id) const {
        vector<Edge> result;
        for (const auto& edge : edges) {
            if (edge.source == user_id || edge.target == user_id) {
                result.push_back(edge);
            }
        }
        return result;
    }

    set<int> getNeighbors(int user_id) const {
        const auto* node = getNode(user_id);
        return node ? node->neighbors : set<int>();
    }

    int getDegree(int user_id) const {
        const auto* node = getNode(user_id);
        return node ? node->neighbors.size() : 0;
    }

    const unordered_map<int, Node>& getNodes() const {
        return nodes;
    }

    const vector<Edge>& getEdges() const {
        return edges;
    }

    const GraphMetadata& getMetadata() const {
        return metadata;
    }

    int getNodeCount() const {
        return nodes.size();
    }

    int getEdgeCount() const {
        return edges.size();
    }

    // Print graph statistics
    void printStatistics() const {
        cout << "\n=== Graph Statistics ===" << endl;
        cout << "Date: " << metadata.date << endl;
        cout << "Total Nodes: " << metadata.total_nodes << endl;
        cout << "Total Edges: " << metadata.total_edges << endl;
        cout << "Friend Relationships: " << metadata.friend_relationships << endl;
        cout << "Fan Relationships: " << metadata.fan_relationships << endl;
        cout << "Average Degree: " << metadata.average_degree << endl;
        cout << "Last Update: " << last_update;
    }
};