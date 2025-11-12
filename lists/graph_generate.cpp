#include <bits/stdc++.h>
#include <string>
#include <vector>
using namespace std;

struct Edge{
    int dest;
    int num_messages;
    int num_mutuals;
    int frequency;      // 1=daily, 2=weekly, 3=monthly, 4=rarely freq of communication
    int relationship;   // 1=friend, 2=colleague, 3=family, 4=acquaintance lvl of relanship
};

// stores  id, name, 
struct Node{
    int id;
    string name;
    int followers;
    int following;
};

class SocialNetworkGraph {
private:
    int num_nodes;
    vector<vector<Edge>> adj_list;  // Adjacency list with edge attributes
    map<int, Node> nodes;           // Node attributes
    
public:
    // Constructor
    SocialNetworkGraph(int n) : num_nodes(n) {
        adj_list.resize(n + 1);  // +1 for 1-indexed nodes
    }
    
    // Load graph from edgelist.txt
    void loadEdgeList(string filename) {
        ifstream file(filename);
        int num_edges;
        file >> num_nodes >> num_edges;
        
        adj_list.resize(num_nodes + 1);
        
        for (int i = 0; i < num_edges; i++) {
            int src, dest, messages, mutuals;
            file >> src >> dest >> messages >> mutuals;
            
            // Add edge in both directions (undirected graph)
            Edge e1 = {dest, messages, mutuals, 0, 0};
            Edge e2 = {src, messages, mutuals, 0, 0};
            
            adj_list[src].push_back(e1);
            adj_list[dest].push_back(e2);
        }
        file.close();
        cout << "Loaded " << num_edges << " edges for " << num_nodes << " nodes\n";
    }
    
    // Load node attributes from nodes.txt
    void loadNodes(string filename) {
        ifstream file(filename);
        int n;
        file >> n;
        
        for (int i = 0; i < n; i++) {
            Node node;
            file >> node.id >> node.name >> node.followers >> node.following;
            nodes[node.id] = node;
        }
        file.close();
        cout << "Loaded " << n << " node attributes\n";
    }
    
    // Load edge attributes from edge_attributes.txt
    void loadEdgeAttributes(string filename) {
        ifstream file(filename);
        int num_edges;
        file >> num_edges;
        
        for (int i = 0; i < num_edges; i++) {
            int src, dest, messages, mutuals, freq, rel;
            file >> src >> dest >> messages >> mutuals >> freq >> rel;
            
            // Update edge attributes in adjacency list
            for (auto &edge : adj_list[src]) {
                if (edge.dest == dest) {
                    edge.frequency = freq;
                    edge.relationship = rel;
                }
            }
            for (auto &edge : adj_list[dest]) {
                if (edge.dest == src) {
                    edge.frequency = freq;
                    edge.relationship = rel;
                }
            }
        }
        file.close();
        cout << "Loaded edge attributes\n";
    }
};
