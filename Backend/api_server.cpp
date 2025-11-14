#include "graph_generator.hpp"
#include "Algorithm.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdio>

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
namespace fs = std::filesystem;

static string urlDecode(const string& value) {
    string result;
    char ch;
    int i, ii;
    for (i = 0; i < static_cast<int>(value.length()); ++i) {
        if (static_cast<int>(value[i]) == 37 && i + 2 < static_cast<int>(value.length())) {
            sscanf(value.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i += 2;
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }
    return result;
}

class SimpleHTTPServer {
private:
    struct GraphContext {
        unique_ptr<SocialGraph> graph;
        unique_ptr<GraphAlgorithms> algorithms;
        string date;
    };

    string datasetRoot;
    string nodesFilename;
    string edgesFilename;
    string metadataFilename;
    string defaultDate;
    int port;
    bool running;
    unordered_map<string, shared_ptr<GraphContext>> graphCache;
    mutable mutex cacheMutex;

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

    string createHTTPResponse(const string& body,
                              const string& contentType = "application/json",
                              int statusCode = 200) {
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

    static string getQueryParameter(const string& query, const string& key) {
        string token = key + "=";
        size_t pos = query.find(token);
        if (pos == string::npos) return "";
        size_t start = pos + token.length();
        size_t end = query.find('&', start);
        string value = query.substr(start, end == string::npos ? string::npos : end - start);
        return urlDecode(value);
    }

    bool datasetExists(const string& date) const {
        fs::path datePath = fs::path(datasetRoot) / date;
        return fs::exists(datePath / nodesFilename) &&
               fs::exists(datePath / edgesFilename) &&
               fs::exists(datePath / metadataFilename);
    }

    vector<string> listAvailableDates() const {
        vector<string> dates;
        if (!fs::exists(datasetRoot)) {
            return dates;
        }
        for (const auto& entry : fs::directory_iterator(datasetRoot)) {
            if (!entry.is_directory()) continue;
            string date = entry.path().filename().string();
            fs::path nodesPath = entry.path() / nodesFilename;
            fs::path edgesPath = entry.path() / edgesFilename;
            fs::path metadataPath = entry.path() / metadataFilename;
            if (fs::exists(nodesPath) && fs::exists(edgesPath) && fs::exists(metadataPath)) {
                dates.push_back(date);
            }
        }
        sort(dates.begin(), dates.end());
        return dates;
    }

    shared_ptr<GraphContext> loadGraphContext(const string& date) {
        fs::path dateDir = fs::path(datasetRoot) / date;
        string nodesPath = (dateDir / nodesFilename).string();
        string edgesPath = (dateDir / edgesFilename).string();
        string metadataPath = (dateDir / metadataFilename).string();

        auto graph = make_unique<SocialGraph>();
        if (!graph->initializeGraph(nodesPath, edgesPath, metadataPath)) {
            throw runtime_error("Failed to load dataset for date " + date);
        }

        auto algorithms = make_unique<GraphAlgorithms>(*graph);
        auto context = make_shared<GraphContext>();
        context->graph = move(graph);
        context->algorithms = move(algorithms);
        context->date = date;
        return context;
    }

    shared_ptr<GraphContext> getGraphContext(const string& date) {
        {
            lock_guard<mutex> lock(cacheMutex);
            auto it = graphCache.find(date);
            if (it != graphCache.end()) {
                return it->second;
            }
        }

        if (!datasetExists(date)) {
            throw runtime_error("Dataset for date " + date + " not found");
        }

        auto context = loadGraphContext(date);
        {
            lock_guard<mutex> lock(cacheMutex);
            graphCache[date] = context;
        }
        return context;
    }

    string handleGetGraph(const SocialGraph& graph, const string& date) {
        json response;
        json nodesArray = json::array();
        json edgesArray = json::array();

        for (const auto& [id, node] : graph.getNodes()) {
            json nodeJson;
            nodeJson["data"]["id"] = to_string(node.user_id);
            nodeJson["data"]["label"] = node.name;
            nodeJson["data"]["user_id"] = node.user_id;
            nodeJson["data"]["name"] = node.name;
            nodeJson["data"]["degree"] = graph.getFriendCount(node.user_id);
            nodeJson["data"]["followers"] = graph.getFollowers(node.user_id).size();
            nodeJson["data"]["following"] = graph.getFollowing(node.user_id).size();
            nodeJson["data"]["region_id"] = node.region_id;
            nodeJson["data"]["interests"] = node.interests;
            nodeJson["data"]["location"] = {node.location.latitude, node.location.longitude};
            nodesArray.push_back(nodeJson);
        }

        int edgeId = 0;
        for (const auto& edge : graph.getEdges()) {
            if (graph.getNode(edge.source) && graph.getNode(edge.target)) {
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
            {"total_nodes", graph.getNodeCount()},
            {"total_edges", graph.getEdgeCount()},
            {"date", date}
        };

        return createHTTPResponse(response.dump());
    }

    string handleGetNode(const SocialGraph& graph, GraphAlgorithms& algorithms, const string& nodeId) {
        int id = stoi(nodeId);
        const Node* node = graph.getNode(id);

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
        response["friend_count"] = graph.getFriendCount(id);
        response["follower_count"] = graph.getFollowers(id).size();
        response["following_count"] = graph.getFollowing(id).size();

        auto metrics = algorithms.get_centrality_metrics(id);
        response["centrality"] = {
            {"degree_centrality", metrics.degree_centrality},
            {"closeness_centrality", metrics.closeness_centrality},
            {"clustering_coefficient", metrics.clustering_coefficient},
            {"raw_degree", metrics.raw_degree}
        };

        return createHTTPResponse(response.dump());
    }

    string handleSearch(GraphAlgorithms& algorithms, const string& query) {
        auto results = algorithms.search_users_with_names(query, 10);
        json response = json::array();

        for (const auto& [id, name] : results) {
            json item;
            item["user_id"] = id;
            item["name"] = name;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    string handleMutualFriends(GraphAlgorithms& algorithms, const string& body) {
        json request = json::parse(body);
        int user1 = request["user1"];
        int user2 = request["user2"];

        auto result = algorithms.analyze_mutual_friends(user1, user2);

        json response;
        response["user_id_1"] = result.user_id_1;
        response["user_id_2"] = result.user_id_2;
        response["mutual_ids"] = result.mutual_ids;
        response["similarity_ratio"] = result.similarity_ratio;
        response["total_degree_1"] = result.total_degree_1;
        response["total_degree_2"] = result.total_degree_2;

        return createHTTPResponse(response.dump());
    }

    string handleInfluencerLeaderboard(GraphAlgorithms& algorithms, int top) {
        auto leaderboard = algorithms.get_influencer_leaderboard(top, 20);
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

    string handleCommunities(GraphAlgorithms& algorithms) {
        auto communities = algorithms.detect_communities();
        json response = json::array();

        // Generate colors for communities
        vector<string> colors = {
            "#3498db", "#e74c3c", "#2ecc71", "#f39c12", "#9b59b6",
            "#1abc9c", "#e67e22", "#34495e", "#16a085", "#c0392b",
            "#27ae60", "#d35400", "#8e44ad", "#2980b9", "#f1c40f"
        };

        for (size_t i = 0; i < communities.size(); i++) {
            const auto& comm = communities[i];
            json item;
            item["community_id"] = comm.community_id;
            item["size"] = comm.size;
            item["member_ids"] = comm.member_ids;
            item["color"] = colors[i % colors.size()];
            item["label"] = "Community " + to_string(comm.community_id);
            item["internal_edge_density"] = comm.internal_edge_density;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    string handlePath(GraphAlgorithms& algorithms, int source, int target) {
        auto result = algorithms.find_shortest_path(source, target);
        
        // Calculate friendship score
        auto friendshipResult = algorithms.calculate_friendship_score(source, target);
        
        json response;
        response["path_exists"] = result.path_exists;
        response["path_length"] = result.path_length;
        response["path_node_ids"] = result.path_node_ids;
        response["path_description"] = result.path_description;
        response["friendship_score"] = friendshipResult.friendship_score;
        response["are_friends"] = friendshipResult.are_friends;
        response["friendship_explanation"] = friendshipResult.explanation;
        return createHTTPResponse(response.dump());
    }

    string handleRecommendations(GraphAlgorithms& algorithms, int userId, int count) {
        auto recommendations = algorithms.get_friend_recommendations(userId, count);
        json response = json::array();

        for (const auto& rec : recommendations) {
            json item;
            item["recommended_user_id"] = rec.recommended_user_id;
            item["user_name"] = rec.user_name;
            item["mutual_friends_count"] = rec.mutual_friends_count;
            item["common_interests_count"] = rec.common_interests_count;
            item["geographic_distance_km"] = rec.geographic_distance_km;
            item["community_similarity"] = rec.community_similarity;
            item["total_score"] = rec.total_score;
            item["recommendation_reason"] = rec.recommendation_reason;
            response.push_back(item);
        }

        return createHTTPResponse(response.dump());
    }

    string handleDatesEndpoint() {
        json response;
        response["default"] = defaultDate;
        response["available"] = listAvailableDates();
        return createHTTPResponse(response.dump());
    }

    void processRequest(int clientSocket) {
        string request = readRequest(clientSocket);
        if (request.empty()) {
            sendResponse(clientSocket, createHTTPResponse("Invalid request", "text/plain", 400));
            #ifdef _WIN32
            closesocket(clientSocket);
            #else
            close(clientSocket);
            #endif
            return;
        }

        stringstream requestStream(request);
        string requestLine;
        getline(requestStream, requestLine);

        string method, path, version;
        stringstream ss(requestLine);
        ss >> method >> path >> version;

        if (method == "OPTIONS") {
            sendResponse(clientSocket, handleOPTIONS());
            #ifdef _WIN32
            closesocket(clientSocket);
            #else
            close(clientSocket);
            #endif
            return;
        }

        string body;
        size_t headerEnd = request.find("\r\n\r\n");
        if (headerEnd != string::npos) {
            body = request.substr(headerEnd + 4);
        }

        string response;
        string basePath = path.substr(0, path.find('?'));
        string queryString = path.find('?') != string::npos ? path.substr(path.find('?') + 1) : "";

        try {
            if (basePath == "/api/dates") {
                response = handleDatesEndpoint();
            } else {
                string dateParam = getQueryParameter(queryString, "date");
                if (dateParam.empty()) {
                    dateParam = defaultDate;
                }

                auto context = getGraphContext(dateParam);
                const auto& graphRef = *context->graph;
                auto& algorithmsRef = *context->algorithms;

                if (basePath == "/api/graph") {
                    response = handleGetGraph(graphRef, dateParam);
                }
                else if (basePath.rfind("/api/node/", 0) == 0) {
                    string nodeId = basePath.substr(10);
                    response = handleGetNode(graphRef, algorithmsRef, nodeId);
                }
                else if (basePath == "/api/search") {
                    string query = getQueryParameter(queryString, "q");
                    response = handleSearch(algorithmsRef, query);
                }
                else if (basePath == "/api/mutual-friends") {
                    response = handleMutualFriends(algorithmsRef, body);
                }
                else if (basePath == "/api/influencer-leaderboard") {
                    int top = 10;
                    string topStr = getQueryParameter(queryString, "top");
                    if (!topStr.empty()) {
                        top = stoi(topStr);
                    }
                    response = handleInfluencerLeaderboard(algorithmsRef, top);
                }
                else if (basePath == "/api/communities") {
                    response = handleCommunities(algorithmsRef);
                }
                else if (basePath == "/api/path") {
                    string sourceStr = getQueryParameter(queryString, "source");
                    string targetStr = getQueryParameter(queryString, "target");
                    if (!sourceStr.empty() && !targetStr.empty()) {
                        response = handlePath(algorithmsRef, stoi(sourceStr), stoi(targetStr));
                    } else {
                        json error = {{"error", "Missing source or target parameter"}};
                        response = createHTTPResponse(error.dump(), "application/json", 400);
                    }
                }
                else if (basePath == "/api/recommendations") {
                    string userStr = getQueryParameter(queryString, "user");
                    string countStr = getQueryParameter(queryString, "count");
                    if (!userStr.empty()) {
                        int count = countStr.empty() ? 10 : stoi(countStr);
                        response = handleRecommendations(algorithmsRef, stoi(userStr), count);
                    } else {
                        json error = {{"error", "Missing user parameter"}};
                        response = createHTTPResponse(error.dump(), "application/json", 400);
                    }
                }
                else {
                    json error = {{"error", "Not found"}};
                    response = createHTTPResponse(error.dump(), "application/json", 404);
                }
            }
        } catch (const exception& ex) {
            json error = {{"error", ex.what()}};
            response = createHTTPResponse(error.dump(), "application/json", 400);
        }

        sendResponse(clientSocket, response);
        #ifdef _WIN32
        closesocket(clientSocket);
        #else
        close(clientSocket);
        #endif
    }

public:
    SimpleHTTPServer(const string& root,
                     const string& nodesFile,
                     const string& edgesFile,
                     const string& metadataFile,
                     const string& initialDate,
                     int p = 8080)
        : datasetRoot(root),
          nodesFilename(nodesFile),
          edgesFilename(edgesFile),
          metadataFilename(metadataFile),
          defaultDate(initialDate),
          port(p),
          running(false) {}

    bool initialize() {
        try {
            getGraphContext(defaultDate);
            return true;
        } catch (const exception& ex) {
            cerr << "Failed to load default dataset: " << ex.what() << endl;
            return false;
        }
    }

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

    fs::path nodesPathFs = fs::absolute(nodesPath);
    fs::path edgesPathFs = fs::absolute(edgesPath);
    fs::path metadataPathFs = fs::absolute(metadataPath);

    fs::path dateDir = nodesPathFs.parent_path();
    if (dateDir.empty()) {
        cerr << "Unable to determine dataset date directory from nodes path" << endl;
        return 1;
    }

    string initialDate = dateDir.filename().string();
    if (initialDate.empty()) {
        cerr << "Unable to determine initial dataset date" << endl;
        return 1;
    }

    fs::path datasetRoot = dateDir.parent_path();
    if (datasetRoot.empty()) {
        datasetRoot = fs::current_path();
    }

    string nodesFilename = nodesPathFs.filename().string();
    string edgesFilename = edgesPathFs.filename().string();
    string metadataFilename = metadataPathFs.filename().string();

    SimpleHTTPServer server(datasetRoot.string(),
                             nodesFilename,
                             edgesFilename,
                             metadataFilename,
                             initialDate,
                             port);

    if (!server.initialize()) {
        return 1;
    }

    server.start();
    return 0;
}

