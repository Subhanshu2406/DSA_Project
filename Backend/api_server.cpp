#include "graph_generator.hpp"
#include "Algorithm.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

// Simple HTTP server using standard library
// For production, consider using cpp-httplib or similar
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

using json = nlohmann::json;
using namespace std;

class SimpleHTTPServer {
private:
    SocialGraph* graph;
    GraphAlgorithms* algorithms;
    int port;
    bool running;

    string readRequest(int clientSocket) {
        string request;
        char buffer[4096] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            request = string(buffer, bytesRead);
        }
        return request;
    }

    void sendResponse(int clientSocket, const string& response) {
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    string createHTTPResponse(const string& body, const string& contentType = "application/json", int statusCode = 200) {
        stringstream ss;
        ss << "HTTP/1.1 " << statusCode << " OK\r\n";
        ss << "Content-Type: " << contentType << "\r\n";
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type\r\n";
        ss << "Content-Length: " << body.length() << "\r\n";
        ss << "Connection: close\r\n\r\n";
        ss << body;
        return ss.str();
    }

    string handleOPTIONS() {
        return createHTTPResponse("", "text/plain", 200);
    }

    string handleGetGraph() {
        json response;
        json nodesArray = json::array();
        json edgesArray = json::array();

        // Convert nodes to Cytoscape format
        for (const auto& [id, node] : graph->getNodes()) {
            json nodeJson;
            nodeJson["data"]["id"] = to_string(node.user_id);
            nodeJson["data"]["label"] = node.name;
            nodeJson["data"]["user_id"] = node.user_id;
            nodeJson["data"]["name"] = node.name;
            nodeJson["data"]["degree"] = graph->getFriendCount(node.user_id);
            nodeJson["data"]["followers"] = graph->getFollowers(node.user_id).size();
            nodeJson["data"]["following"] = graph->getFollowing(node.user_id).size();
            nodeJson["data"]["region_id"] = node.region_id;
            nodeJson["data"]["interests"] = node.interests;
            nodeJson["data"]["location"] = {node.location.latitude, node.location.longitude};
            // Add position for initial layout (will be overridden by layout algorithm)
            nodeJson["position"] = json::object();
            nodesArray.push_back(nodeJson);
        }

        // Convert edges to Cytoscape format
        int edgeId = 0;
        for (const auto& edge : graph->getEdges()) {
            // Only add edge if both source and target nodes exist
            if (graph->getNode(edge.source) && graph->getNode(edge.target)) {
                json edgeJson;
                edgeJson["data"]["id"] = "e" + to_string(edgeId++);
                edgeJson["data"]["source"] = to_string(edge.source);
                edgeJson["data"]["target"] = to_string(edge.target);
                edgeJson["data"]["relationship_type"] = edge.relationship_type;
                edgeJson["data"]["weight"] = 1;
                edgesArray.push_back(edgeJson);
            }
        }

        response["nodes"] = nodesArray;
        response["edges"] = edgesArray;
        response["metadata"] = {
            {"total_nodes", graph->getNodeCount()},
            {"total_edges", graph->getEdgeCount()}
        };

        return createHTTPResponse(response.dump());
    }

    string handleGetNode(const string& nodeId) {
        int id = stoi(nodeId);
        const Node* node = graph->getNode(id);
        
        if (!node) {
            json error = {{"error", "Node not found"}};
            return createHTTPResponse(error.dump(), "application/json", 404);
        }

        json response;
        response["user_id"] = node->user_id;
        response["name"] = node->name;
        response["location"] = {node->location.latitude, node->location.longitude};
        response["region_id"] = node->region_id;
        response["interests"] = node->interests;
        response["created_at"] = node->created_at;
        response["friend_count"] = graph->getFriendCount(id);
        response["follower_count"] = graph->getFollowers(id).size();
        response["following_count"] = graph->getFollowing(id).size();

        // Get centrality metrics
        auto metrics = algorithms->get_centrality_metrics(id);
        response["centrality"] = {
            {"degree_centrality", metrics.degree_centrality},
            {"closeness_centrality", metrics.closeness_centrality},
            {"clustering_coefficient", metrics.clustering_coefficient},
            {"raw_degree", metrics.raw_degree}
        };

        return createHTTPResponse(response.dump());
    }

    string handleSearch(const string& query) {
        auto results = algorithms->search_users_with_names(query, 10);
        json response = json::array();
        
        for (const auto& [id, name] : results) {
            json item;
            item["user_id"] = id;
            item["name"] = name;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    string handleMutualFriends(const string& body) {
        json request = json::parse(body);
        int user1 = request["user1"];
        int user2 = request["user2"];

        auto result = algorithms->analyze_mutual_friends(user1, user2);
        
        json response;
        response["user_id_1"] = result.user_id_1;
        response["user_id_2"] = result.user_id_2;
        response["mutual_ids"] = result.mutual_ids;
        response["similarity_ratio"] = result.similarity_ratio;
        response["total_degree_1"] = result.total_degree_1;
        response["total_degree_2"] = result.total_degree_2;

        return createHTTPResponse(response.dump());
    }

    string handleInfluencerLeaderboard(int top) {
        auto leaderboard = algorithms->get_influencer_leaderboard(top, 20);
        json response = json::array();

        for (const auto& entry : leaderboard) {
            json item;
            item["rank"] = entry.rank;
            item["user_id"] = entry.user_id;
            item["user_name"] = entry.user_name;
            item["influence_score"] = entry.influence_score;
            item["fan_count"] = entry.fan_count;
            item["friend_count"] = entry.friend_count;
            item["pagerank_score"] = entry.pagerank_score;
            item["total_followers"] = entry.total_followers;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    string handleCommunities() {
        auto communities = algorithms->detect_communities(0, 10);
        json response = json::array();

        // Generate colors for communities
        vector<string> colors = {
            "#FF6B6B", "#4ECDC4", "#45B7D1", "#FFA07A", "#98D8C8",
            "#F7DC6F", "#BB8FCE", "#85C1E2", "#F8B739", "#52BE80",
            "#EC7063", "#5DADE2", "#F4D03F", "#AF7AC5", "#85C1E9"
        };

        for (size_t i = 0; i < communities.size(); i++) {
            const auto& comm = communities[i];
            json commJson;
            commJson["community_id"] = comm.community_id;
            commJson["member_ids"] = comm.member_ids;
            commJson["size"] = comm.size;
            commJson["internal_edge_density"] = comm.internal_edge_density;
            commJson["modularity_score"] = comm.modularity_score;
            commJson["edges_within_community"] = comm.edges_within_community;
            commJson["edges_to_outside"] = comm.edges_to_outside;
            commJson["color"] = colors[i % colors.size()];
            response.push_back(commJson);
        }

        return createHTTPResponse(response.dump());
    }

    string handlePath(int source, int target) {
        auto result = algorithms->find_shortest_path(source, target);
        
        json response;
        response["path_exists"] = result.path_exists;
        response["path_node_ids"] = result.path_node_ids;
        response["path_length"] = result.path_length;
        response["path_description"] = result.path_description;

        return createHTTPResponse(response.dump());
    }

    string handleRecommendations(int userId, int count) {
        auto recommendations = algorithms->get_friend_recommendations(userId, count);
        json response = json::array();

        for (const auto& rec : recommendations) {
            json item;
            item["recommended_user_id"] = rec.recommended_user_id;
            item["user_name"] = rec.user_name;
            item["total_score"] = rec.total_score;
            item["mutual_friends_count"] = rec.mutual_friends_count;
            item["common_interests_count"] = rec.common_interests_count;
            item["geographic_distance_km"] = rec.geographic_distance_km;
            item["community_similarity"] = rec.community_similarity;
            item["recommendation_reason"] = rec.recommendation_reason;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    void processRequest(int clientSocket) {
        string request = readRequest(clientSocket);
        
        if (request.empty()) {
            #ifdef _WIN32
            closesocket(clientSocket);
            #else
            close(clientSocket);
            #endif
            return;
        }

        stringstream ss(request);
        string method, path, protocol;
        ss >> method >> path >> protocol;

        // Handle CORS preflight
        if (method == "OPTIONS") {
            sendResponse(clientSocket, handleOPTIONS());
            #ifdef _WIN32
            closesocket(clientSocket);
            #else
            close(clientSocket);
            #endif
            return;
        }

        string response;

        // Parse query parameters
        size_t queryPos = path.find('?');
        string basePath = path;
        string queryString = "";
        if (queryPos != string::npos) {
            basePath = path.substr(0, queryPos);
            queryString = path.substr(queryPos + 1);
        }

        // Route requests
        if (basePath == "/api/graph") {
            response = handleGetGraph();
        }
        else if (basePath.find("/api/node/") == 0) {
            string nodeId = basePath.substr(10);
            response = handleGetNode(nodeId);
        }
        else if (basePath == "/api/search") {
            // Parse query parameter
            size_t qPos = queryString.find("q=");
            string query = "";
            if (qPos != string::npos) {
                query = queryString.substr(qPos + 2);
                size_t ampPos = query.find('&');
                if (ampPos != string::npos) {
                    query = query.substr(0, ampPos);
                }
                // URL decode (simple)
                size_t plusPos;
                while ((plusPos = query.find('+')) != string::npos) {
                    query.replace(plusPos, 1, " ");
                }
            }
            response = handleSearch(query);
        }
        else if (basePath == "/api/mutual-friends" && method == "POST") {
            // Get body
            size_t bodyPos = request.find("\r\n\r\n");
            string body = "";
            if (bodyPos != string::npos) {
                body = request.substr(bodyPos + 4);
            }
            response = handleMutualFriends(body);
        }
        else if (basePath == "/api/influencer-leaderboard") {
            int top = 10;
            size_t topPos = queryString.find("top=");
            if (topPos != string::npos) {
                string topStr = queryString.substr(topPos + 4);
                size_t ampPos = topStr.find('&');
                if (ampPos != string::npos) {
                    topStr = topStr.substr(0, ampPos);
                }
                top = stoi(topStr);
            }
            response = handleInfluencerLeaderboard(top);
        }
        else if (basePath == "/api/communities") {
            response = handleCommunities();
        }
        else if (basePath == "/api/path") {
            int source = -1, target = -1;
            size_t srcPos = queryString.find("source=");
            size_t tgtPos = queryString.find("target=");
            if (srcPos != string::npos) {
                string srcStr = queryString.substr(srcPos + 7);
                size_t ampPos = srcStr.find('&');
                if (ampPos != string::npos) {
                    srcStr = srcStr.substr(0, ampPos);
                }
                source = stoi(srcStr);
            }
            if (tgtPos != string::npos) {
                string tgtStr = queryString.substr(tgtPos + 7);
                size_t ampPos = tgtStr.find('&');
                if (ampPos != string::npos) {
                    tgtStr = tgtStr.substr(0, ampPos);
                }
                target = stoi(tgtStr);
            }
            if (source >= 0 && target >= 0) {
                response = handlePath(source, target);
            } else {
                json error = {{"error", "Missing source or target parameter"}};
                response = createHTTPResponse(error.dump(), "application/json", 400);
            }
        }
        else if (basePath == "/api/recommendations") {
            int userId = -1, count = 10;
            size_t userPos = queryString.find("user=");
            size_t countPos = queryString.find("count=");
            if (userPos != string::npos) {
                string userStr = queryString.substr(userPos + 5);
                size_t ampPos = userStr.find('&');
                if (ampPos != string::npos) {
                    userStr = userStr.substr(0, ampPos);
                }
                userId = stoi(userStr);
            }
            if (countPos != string::npos) {
                string countStr = queryString.substr(countPos + 6);
                size_t ampPos = countStr.find('&');
                if (ampPos != string::npos) {
                    countStr = countStr.substr(0, ampPos);
                }
                count = stoi(countStr);
            }
            if (userId >= 0) {
                response = handleRecommendations(userId, count);
            } else {
                json error = {{"error", "Missing user parameter"}};
                response = createHTTPResponse(error.dump(), "application/json", 400);
            }
        }
        else {
            json error = {{"error", "Not found"}};
            response = createHTTPResponse(error.dump(), "application/json", 404);
        }

        sendResponse(clientSocket, response);
        #ifdef _WIN32
        closesocket(clientSocket);
        #else
        close(clientSocket);
        #endif
    }

public:
    SimpleHTTPServer(SocialGraph* g, GraphAlgorithms* alg, int p = 8080) 
        : graph(g), algorithms(alg), port(p), running(false) {}

    void start() {
        #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif

        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            cerr << "Error creating socket" << endl;
            return;
        }

        #ifdef _WIN32
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
        #else
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        #endif

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            cerr << "Error binding to port " << port << endl;
            #ifdef _WIN32
            closesocket(serverSocket);
            WSACleanup();
            #else
            close(serverSocket);
            #endif
            return;
        }

        if (listen(serverSocket, 10) < 0) {
            cerr << "Error listening on socket" << endl;
            #ifdef _WIN32
            closesocket(serverSocket);
            WSACleanup();
            #else
            close(serverSocket);
            #endif
            return;
        }

        running = true;
        cout << "HTTP Server started on port " << port << endl;
        cout << "API endpoints available at http://localhost:" << port << "/api/" << endl;

        while (running) {
            sockaddr_in clientAddr;
            #ifdef _WIN32
            int clientAddrLen = sizeof(clientAddr);
            #else
            socklen_t clientAddrLen = sizeof(clientAddr);
            #endif
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

            if (clientSocket >= 0) {
                thread clientThread(&SimpleHTTPServer::processRequest, this, clientSocket);
                clientThread.detach();
            }
        }

        #ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
        #else
        close(serverSocket);
        #endif
    }

    void stop() {
        running = false;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <nodes.json> <edges.json> <metadata.json> [port]" << endl;
        return 1;
    }

    string nodesPath = argv[1];
    string edgesPath = argv[2];
    string metadataPath = argv[3];
    int port = 8080;
    
    if (argc >= 5) {
        port = stoi(argv[4]);
    }

    SocialGraph graph;
    if (!graph.initializeGraph(nodesPath, edgesPath, metadataPath)) {
        cerr << "Failed to initialize graph" << endl;
        return 1;
    }

    GraphAlgorithms algorithms(graph);
    cout << "Graph loaded successfully!" << endl;
    graph.printStatistics();

    SimpleHTTPServer server(&graph, &algorithms, port);
    server.start();

    return 0;
}
