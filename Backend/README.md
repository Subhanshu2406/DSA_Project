# Backend - Social Graph Analysis Engine

## Overview

The backend is a high-performance C++ HTTP server that provides RESTful API endpoints for social network graph analysis. It implements a comprehensive suite of graph algorithms optimized for large-scale social networks with real-time query capabilities.

## Technology Stack

- **C++17**: Core implementation language
- **HTTP Server**: Custom lightweight server using Windows Sockets (Winsock2)
- **JSON**: nlohmann/json library for data serialization
- **Graph Algorithms**: Custom implementations optimized for social networks

## Architecture

### File Structure

```
Backend/
├── api_server.cpp          # HTTP server and API endpoints
├── Algorithm.hpp           # Unified algorithms interface
├── graph_generator.hpp     # Graph data structures and loading
├── algo_utils.hpp          # Algorithm utilities
├── Features/
│   ├── mutual_friends.hpp      # Mutual friends analysis
│   ├── friend_recommender.hpp  # Friend recommendation system
│   ├── pagerank.hpp            # PageRank calculation
│   ├── community_detection.hpp # Community detection algorithms
│   ├── influencer_ranking.hpp  # Influencer ranking system
│   ├── centrality.hpp          # Centrality metrics
│   ├── short_path.hpp          # Shortest path finding
│   ├── user_search.hpp         # Autocomplete search (Trie)
│   └── friendshipscore.hpp    # Friendship score calculator
└── nlohmann/
    └── json.hpp            # JSON library
```

## Features & Algorithms

### 1. Mutual Friends Analysis

**Endpoint**: `GET /api/mutual-friends?user1=X&user2=Y`

**Algorithm**: Set Intersection
- Finds common friends between two users
- Calculates similarity ratio (Jaccard similarity)
- Returns mutual friend IDs and statistics

**Time Complexity**: O(min(|friends1|, |friends2|))
**Space Complexity**: O(min(|friends1|, |friends2|))

**Implementation**: `Features/mutual_friends.hpp`

---

### 2. Friend Recommendation System

**Endpoint**: `GET /api/recommendations?user_id=X&count=10`

**Algorithm**: Multi-Factor Scoring
- **Mutual Friends (40% weight)**: More mutual friends = higher score
- **Common Interests (25% weight)**: Shared interests boost recommendation
- **Geographic Proximity (20% weight)**: Closer users get higher scores
- **Community Similarity (15% weight)**: Same community = better match

**Process**:
1. Find "friends of friends" (excluding existing connections)
2. Score each candidate using weighted factors
3. Return top K sorted by total score

**Time Complexity**: O(V × avg_degree²)
**Space Complexity**: O(V)

**Implementation**: `Features/friend_recommender.hpp`

---

### 3. PageRank Calculation

**Endpoint**: `GET /api/pagerank?damping=0.85&iterations=20`

**Algorithm**: Iterative PageRank with Damping Factor
- Models user importance based on incoming links
- Uses damping factor (0.85) to handle dead ends
- Iterative convergence (typically 20 iterations)

**Formula**: 
```
PR(u) = (1-d)/N + d × Σ(PR(v) / out_degree(v))
```
where:
- `d` = damping factor (0.85)
- `N` = total nodes
- Sum over all nodes `v` that link to `u`

**Time Complexity**: O(E × iterations)
**Space Complexity**: O(V)

**Implementation**: `Features/pagerank.hpp`

---

### 4. Community Detection

**Endpoint**: `GET /api/communities?method=0&iterations=10`

**Algorithms**:

#### a) Label Propagation (Default)
- Each node adopts label of most frequent neighbor
- Iterative process until convergence
- Fast and effective for social networks

**Time Complexity**: O(V × avg_degree × iterations)
**Space Complexity**: O(V)

#### b) Greedy Modularity Optimization
- Maximizes modularity score
- Merges communities greedily
- More accurate but slower

**Time Complexity**: O(V² × log(V))
**Space Complexity**: O(V)

**Returns**: 
- Community IDs
- Member lists
- Internal edge density
- Modularity scores

**Implementation**: `Features/community_detection.hpp`

---

### 5. Influencer Ranking

**Endpoint**: `GET /api/influencer-leaderboard?top_k=10`

**Algorithm**: Composite Scoring
- **PageRank Score**: Network importance (40% weight)
- **Follower Count**: Direct influence (30% weight)
- **Fan Count**: One-way follows (20% weight)
- **Friend Count**: Mutual connections (10% weight)

**Process**:
1. Calculate PageRank for all users
2. Combine with follower/fan/friend counts
3. Normalize and weight factors
4. Sort and return top K

**Time Complexity**: O(E × pagerank_iterations + V × log(V))
**Space Complexity**: O(V)

**Implementation**: `Features/influencer_ranking.hpp`

---

### 6. Centrality Analysis

**Endpoint**: `GET /api/centrality?user_id=X`

**Metrics**:

#### Degree Centrality
- Number of direct connections
- Normalized by (V-1)

#### Closeness Centrality
- Average distance to all other nodes
- Higher = more central in network

#### Clustering Coefficient
- Probability that neighbors are connected
- Measures local clustering

**Time Complexity**: O(V + E) for degree, O(V × (V + E)) for closeness
**Space Complexity**: O(V)

**Implementation**: `Features/centrality.hpp`

---

### 7. Shortest Path Finding

**Endpoint**: `GET /api/path?source=X&target=Y`

**Algorithm**: Bidirectional BFS
- Searches from both source and target simultaneously
- Meets in the middle for optimal performance
- Handles directed graphs (friends/followers)

**Optimizations**:
- Caching for repeated queries
- Early termination when path found
- Path reconstruction from meeting point

**Time Complexity**: O(V + E) average case
**Space Complexity**: O(V)

**Returns**:
- Path existence boolean
- Path node IDs
- Path length (hops)
- Path description string
- **Friendship Score** (see below)

**Implementation**: `Features/short_path.hpp`

---

### 8. User Search (Autocomplete)

**Endpoint**: `GET /api/search?q=prefix`

**Algorithm**: Trie Data Structure
- Fast prefix matching
- Returns up to 10 matching users
- Case-insensitive search

**Data Structure**:
- Trie with user IDs and names
- Built once on graph load
- O(m) search time where m = prefix length

**Time Complexity**: O(m) per query
**Space Complexity**: O(V × avg_name_length)

**Implementation**: `Features/user_search.hpp`

---

### 9. Friendship Score Calculator

**Endpoint**: Integrated into `/api/path` response

**Algorithm**: Multi-Factor Scoring
- **Base Score**: 1.5 (friends) or 2.5 (non-friends)
- **Mutual Friends Factor**: -0.3 max (more mutual = lower score)
- **Message Count Factor**: -0.25 max (more messages = lower score)
- **Transitive Factor**: -0.15 max (mutual friends' closeness)
- **Time Factor**: -0.15 max (longer connection = lower score)
- **Geographic Factor**: -0.1 max (closer = lower score)
- **Interests Factor**: -0.1 max (more shared = lower score)

**Score Range**:
- Friends: 1.0 to 2.0 (lower = stronger friendship)
- Non-friends: 2.0 to 3.0 (lower = closer connection)

**Time Complexity**: O(avg_degree²) per calculation
**Space Complexity**: O(1)

**Implementation**: `Features/friendshipscore.hpp`

---

## API Endpoints

### Graph Data
- `GET /api/graph?date=YYYY-MM-DD` - Get graph data (nodes, edges, metadata)

### Search
- `GET /api/search?q=prefix` - Autocomplete user search

### Analysis Features
- `GET /api/mutual-friends?user1=X&user2=Y` - Mutual friends analysis
- `GET /api/recommendations?user_id=X&count=10` - Friend recommendations
- `GET /api/pagerank?damping=0.85&iterations=20` - PageRank scores
- `GET /api/communities?method=0&iterations=10` - Community detection
- `GET /api/influencer-leaderboard?top_k=10` - Influencer ranking
- `GET /api/centrality?user_id=X` - Centrality metrics
- `GET /api/path?source=X&target=Y` - Shortest path with friendship score

### Dataset Management
- `GET /api/dates` - List available dataset dates and default date
- `GET /api/node?user_id=X` - Get single node details

## Data Structures

### Node
```cpp
struct Node {
    int user_id;
    string name;
    Location location;
    int region_id;
    vector<string> interests;
    string created_at;
    set<int> friends;      // Mutual relationships
    set<int> followers;    // Incoming edges
    set<int> following;    // Outgoing edges
};
```

### Edge
```cpp
struct Edge {
    int source;
    int target;
    string relationship_type;  // "friend" or "fan"
    int message_count;
    string last_interaction;
    double distance;
    string established_at;
};
```

## Building & Running

### Prerequisites
- C++17 compatible compiler (g++, clang++, MSVC)
- Windows: Winsock2 library
- nlohmann/json.hpp (included in repository)

### Compilation

**Windows (MinGW/MSVC)**:
```bash
cd Backend
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server.exe -lws2_32
```

**Linux/Mac**:
```bash
cd Backend
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

### Running

```bash
./api_server.exe <nodes_path> <edges_path> <metadata_path>
```

Example:
```bash
./api_server.exe ../dataset/data/generated/2024-01-01/nodes.json \
                 ../dataset/data/generated/2024-01-01/edges.json \
                 ../dataset/data/generated/2024-01-01/metadata.json
```

Server runs on `http://localhost:8080` by default.

## Performance Characteristics

- **Graph Loading**: O(V + E) - Linear time
- **Query Response**: Most queries < 100ms for 1000 nodes
- **Memory Usage**: ~50MB for 1000 nodes, 10000 edges
- **Concurrent Requests**: Single-threaded (can be extended)

## Design Patterns

1. **Unified Interface**: `GraphAlgorithms` class provides single entry point
2. **Separation of Concerns**: Each algorithm in separate header file
3. **Const Correctness**: Algorithms don't modify graph
4. **RAII**: Automatic resource management
5. **Cache Optimization**: Friendship scores and paths cached

## Error Handling

- Invalid user IDs return 404
- Missing files return 500
- Malformed JSON returns 400
- All errors include descriptive messages

## Testing

Run test suite:
```bash
cd Backend
g++ -std=c++17 test_features.cpp -o test_features.exe
./test_features.exe
```

## Future Enhancements

- Multi-threading for concurrent requests
- Graph database backend (Neo4j integration)
- Real-time graph updates
- GraphQL API option
- Performance profiling tools
- Extended caching strategies

