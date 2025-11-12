#pragma once

#include "graph_generator.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

/**
 * User Search Index for autocomplete functionality
 * Uses Trie data structure for efficient prefix matching
 */
class UserSearchIndex {
private:
    struct TrieNode {
        unordered_map<char, TrieNode*> children;
        vector<int> user_ids;  // User IDs matching this prefix
        
        ~TrieNode() {
            for (auto& pair : children) {
                delete pair.second;
            }
        }
    };
    
    TrieNode* root;
    const SocialGraph* graph;
    bool isBuilt;
    
    // Helper to convert string to lowercase
    string toLower(const string& str) const {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Insert a user name into the trie
    void insert(const string& name, int user_id) {
        string lowerName = toLower(name);
        TrieNode* current = root;
        
        for (char c : lowerName) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];
            
            // Store user_id at each node (allows prefix matching)
            // Limit to avoid storing too many IDs at root
            if (current->user_ids.size() < 100) {
                current->user_ids.push_back(user_id);
            }
        }
    }
    
    // Get all user IDs from a trie node and its children
    void getAllUserIds(TrieNode* node, vector<int>& result, int limit) const {
        if (!node || result.size() >= limit) return;
        
        // Add user IDs from current node
        for (int user_id : node->user_ids) {
            if (result.size() >= limit) break;
            // Avoid duplicates
            if (find(result.begin(), result.end(), user_id) == result.end()) {
                result.push_back(user_id);
            }
        }
        
        // Recursively search children
        for (const auto& pair : node->children) {
            if (result.size() >= limit) break;
            getAllUserIds(pair.second, result, limit);
        }
    }

public:
    UserSearchIndex() : root(new TrieNode()), graph(nullptr), isBuilt(false) {}
    
    ~UserSearchIndex() {
        delete root;
    }
    
    // Build the search index from the graph
    void buildIndex(const SocialGraph& graph) {
        this->graph = &graph;
        root = new TrieNode();  // Reset
        isBuilt = false;
        
        const auto& nodes = graph.getNodes();
        for (const auto& pair : nodes) {
            const Node& node = pair.second;
            insert(node.name, node.user_id);
        }
        
        isBuilt = true;
    }
    
    /**
     * Search for users matching the given prefix
     * 
     * @param prefix Search query (prefix)
     * @param limit Maximum number of results to return (default: 10)
     * @return Vector of user IDs matching the prefix, sorted by relevance
     */
    vector<int> search(const string& prefix, int limit = 10) const {
        vector<int> results;
        
        if (!isBuilt || prefix.empty()) {
            return results;
        }
        
        string lowerPrefix = toLower(prefix);
        TrieNode* current = root;
        
        // Navigate to the node representing the prefix
        for (char c : lowerPrefix) {
            if (current->children.find(c) == current->children.end()) {
                return results;  // No matches
            }
            current = current->children[c];
        }
        
        // Collect all user IDs from this node and its children
        getAllUserIds(current, results, limit);
        
        // Sort by name for consistent ordering
        sort(results.begin(), results.end(), [this](int a, int b) {
            const Node* nodeA = graph->getNode(a);
            const Node* nodeB = graph->getNode(b);
            if (!nodeA || !nodeB) return false;
            return toLower(nodeA->name) < toLower(nodeB->name);
        });
        
        return results;
    }
    
    /**
     * Search and return user information
     * 
     * @param prefix Search query
     * @param limit Maximum results
     * @return Vector of pairs (user_id, name) sorted by relevance
     */
    vector<pair<int, string>> searchWithNames(const string& prefix, int limit = 10) const {
        vector<int> user_ids = search(prefix, limit);
        vector<pair<int, string>> results;
        
        for (int user_id : user_ids) {
            const Node* node = graph->getNode(user_id);
            if (node) {
                results.push_back({user_id, node->name});
            }
        }
        
        return results;
    }
    
    /**
     * Check if the index is built and ready
     */
    bool isReady() const {
        return isBuilt;
    }
    
    /**
     * Get total number of indexed users
     */
    int getIndexedUserCount() const {
        if (!isBuilt) return 0;
        return graph->getNodeCount();
    }
};

