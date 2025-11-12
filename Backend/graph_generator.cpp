// graph_generator.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>
#  include <nlohmann/json.hpp> // Fallback: hope compiler provides it
#endif
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>
using namespace std;

using json = nlohmann::json;

struct Location {
    double latitude;
    double longitude;
};

struct Node {
    int user_id;
     string name;
    Location location;
    int region_id;
     vector< string> interests;
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
    bool loadNodesFromJSON(const  string& filepath) {
        try {
             ifstream file(filepath);
            if (!file.is_open()) {
                 cerr << "Cannot open nodes file: " << filepath <<  endl;
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

             cout << "✓ Loaded " << nodes.size() << " nodes" <<  endl;
            return true;
        } catch (const  exception& e) {
             cerr << "Error loading nodes: " << e.what() <<  endl;
            return false;
        }
    }

    // Load edges from JSON file
    bool loadEdgesFromJSON(const  string& filepath) {
        try {
             ifstream file(filepath);
            if (!file.is_open()) {
                 cerr << "Cannot open edges file: " << filepath <<  endl;
                return false;
            }

            json edgesArray;
            file >> edgesArray;
            edges.clear();

            for (const auto& edgeJSON : edgesArray) {
                Edge edge;
                edge.source = edgeJSON["source"];
                edge.target = edgeJSON["target"];
                edge.relationship_type = edgeJSON["relationship_type"];
                edge.message_count = edgeJSON["message_count"];
                edge.last_interaction = edgeJSON.value("last_interaction", "");
                edge.distance = edgeJSON["distance"];
                edge.established_at = edgeJSON["established_at"];

                edges.push_back(edge);

                // Update adjacency list
                if (nodes.find(edge.source) != nodes.end() && 
                    nodes.find(edge.target) != nodes.end()) {
                    nodes[edge.source].neighbors.insert(edge.target);
                    nodes[edge.target].neighbors.insert(edge.source);
                }
            }

             cout << "✓ Loaded " << edges.size() << " edges" <<  endl;
            return true;
        } catch (const  exception& e) {
             cerr << "Error loading edges: " << e.what() <<  endl;
            return false;
        }
    }

    // Load metadata from JSON file
    bool loadMetadataFromJSON(const  string& filepath) {
        try {
             ifstream file(filepath);
            if (!file.is_open()) {
                 cerr << "Cannot open metadata file: " << filepath <<  endl;
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

             cout << "✓ Loaded metadata" <<  endl;
            return true;
        } catch (const  exception& e) {
             cerr << "Error loading metadata: " << e.what() <<  endl;
            return false;
        }
    }

    // Initialize graph from JSON files
    bool initializeGraph(const  string& nodesFile, 
                       const  string& edgesFile, 
                       const  string& metadataFile) {
         cout << "\n=== Initializing Graph ===" <<  endl;
        
        if (!loadNodesFromJSON(nodesFile)) return false;
        if (!loadEdgesFromJSON(edgesFile)) return false;
        if (!loadMetadataFromJSON(metadataFile)) return false;

        auto now =  chrono::system_clock::now();
        auto time =  chrono::system_clock::to_time_t(now);
        last_update =  ctime(&time);

        return true;
    }

    // Update graph with new data (delta updates)
    bool updateGraph(const  string& nodesFile, 
                    const  string& edgesFile, 
                    const  string& metadataFile) {
         cout << "\n=== Updating Graph ===" <<  endl;
        
        // Backup current state
        auto nodesBackup = nodes;
        auto edgesBackup = edges;

        // Load new data
        if (!loadNodesFromJSON(nodesFile)) {
            nodes = nodesBackup;
            edges = edgesBackup;
            return false;
        }

        if (!loadEdgesFromJSON(edgesFile)) {
            nodes = nodesBackup;
            edges = edgesBackup;
            return false;
        }

        if (!loadMetadataFromJSON(metadataFile)) {
            nodes = nodesBackup;
            edges = edgesBackup;
            return false;
        }

        auto now =  chrono::system_clock::now();
        auto time =  chrono::system_clock::to_time_t(now);
        last_update =  ctime(&time);

         cout << "✓ Graph updated successfully at " << last_update <<  endl;
        return true;
    }

    // Get node by user_id
    const Node* getNode(int user_id) const {
        auto it = nodes.find(user_id);
        return (it != nodes.end()) ? &it->second : nullptr;
    }

    // Get edges for a node
     vector<Edge> getEdgesForNode(int user_id) const {
         vector<Edge> result;
        for (const auto& edge : edges) {
            if (edge.source == user_id || edge.target == user_id) {
                result.push_back(edge);
            }
        }
        return result;
    }

    // Get neighbors of a node
     set<int> getNeighbors(int user_id) const {
        const auto* node = getNode(user_id);
        return node ? node->neighbors :  set<int>();
    }

    // Get degree of a node
    int getDegree(int user_id) const {
        const auto* node = getNode(user_id);
        return node ? node->neighbors.size() : 0;
    }

    // Get all nodes
    const  unordered_map<int, Node>& getNodes() const {
        return nodes;
    }

    // Get all edges
    const  vector<Edge>& getEdges() const {
        return edges;
    }

    // Get metadata
    const GraphMetadata& getMetadata() const {
        return metadata;
    }

    // Print graph statistics
    void printStatistics() const {
         cout << "\n=== Graph Statistics ===" <<  endl;
         cout << "Date: " << metadata.date <<  endl;
         cout << "Total Nodes: " << metadata.total_nodes <<  endl;
         cout << "Total Edges: " << metadata.total_edges <<  endl;
         cout << "Friend Relationships: " << metadata.friend_relationships <<  endl;
         cout << "Fan Relationships: " << metadata.fan_relationships <<  endl;
         cout << "Average Degree: " << metadata.average_degree <<  endl;
         cout << "Last Update: " << last_update;
         cout << "Actual Nodes Count: " << nodes.size() <<  endl;
         cout << "Actual Edges Count: " << edges.size() <<  endl;
    }

    // Save graph to binary format for faster loading
    bool serializeGraph(const  string& filepath) const {
        try {
             ofstream file(filepath,  ios::binary);
            if (!file.is_open()) {
                 cerr << "Cannot create serialized file: " << filepath <<  endl;
                return false;
            }

            // Serialize nodes
            size_t nodeCount = nodes.size();
            file.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));
            for (const auto& [id, node] : nodes) {
                file.write(reinterpret_cast<const char*>(&node.user_id), sizeof(node.user_id));
                size_t nameLen = node.name.length();
                file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
                file.write(node.name.c_str(), nameLen);
            }

            // Serialize edges
            size_t edgeCount = edges.size();
            file.write(reinterpret_cast<const char*>(&edgeCount), sizeof(edgeCount));
            for (const auto& edge : edges) {
                file.write(reinterpret_cast<const char*>(&edge.source), sizeof(edge.source));
                file.write(reinterpret_cast<const char*>(&edge.target), sizeof(edge.target));
            }

             cout << "✓ Graph serialized to " << filepath <<  endl;
            return true;
        } catch (const  exception& e) {
             cerr << "Error serializing graph: " << e.what() <<  endl;
            return false;
        }
    }
};

// ============== MAIN FUNCTION ==============
int main() {
    SocialGraph graph;

    // Initialize with JSON files
    if (!graph.initializeGraph(
        "data/nodes.json",
        "data/edges.json",
        "data/metadata.json")) {
         cerr << "Failed to initialize graph" <<  endl;
        return 1;
    }

    graph.printStatistics();

    // Example: Access node data
    const Node* sample = graph.getNode(0);
    if (sample) {
         cout << "\n=== Sample Node ===" <<  endl;
         cout << "Name: " << sample->name <<  endl;
         cout << "Region: " << sample->region_id <<  endl;
         cout << "Degree: " << graph.getDegree(sample->user_id) <<  endl;
    }

    // Simulate daily update
     cout << "\n=== Simulating Daily Update ===" <<  endl;
    if (graph.updateGraph(
        "data/nodes_new.json",
        "data/edges_new.json",
        "data/metadata_new.json")) {
        graph.printStatistics();
    }

    // Serialize for faster access
    graph.serializeGraph("graph.bin");

    return 0;
}