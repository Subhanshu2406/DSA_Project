# Backend - Graph Algorithms & API Server

High-performance C++ backend implementing graph algorithms and REST API for social network analysis.

## Overview

The backend provides a comprehensive set of graph algorithms optimized for performance, along with an HTTP server that exposes these algorithms via REST API endpoints. All algorithms are implemented in C++17 for maximum efficiency.

## Architecture

```
Backend/
├── api_server.cpp          # HTTP server and API endpoints
├── Algorithm.hpp          # Unified algorithm interface
├── graph_generator.hpp     # Graph data structure
├── algo_utils.hpp          # Algorithm utilities
├── Features/               # Individual algorithm implementations
│   ├── mutual_friends.hpp
│   ├── pagerank.hpp
│   ├── community_detection.hpp
│   ├── influencer_ranking.hpp
│   ├── short_path.hpp
│   ├── friend_recommender.hpp
│   ├── friendshipscore.hpp
│   ├── user_search.hpp
│   └── centrality.hpp
└── nlohmann/
    └── json.hpp            # JSON library
```

## Graph Data Structure

### SocialGraph Class
- **Nodes**: Users with attributes (ID, name, location, interests, friends, followers, following)
- **Edges**: Relationships with attributes (source, target, type, messages, distance, timestamps)
- **Storage**: `unordered_map<int, Node>` for O(1) node lookup
- **Relationships**: Sets for friends/followers/following (O(1) membership check)

## Algorithms Implemented

### 1. Mutual Friends Analysis
**File**: `Features/mutual_friends.hpp`  
**Class**: `MutualFriendsAnalyzer`

**How it works:**
1. Get friend sets for both users
2. Find intersection of the two sets (common friends)
3. Calculate Jaccard similarity: `|intersection| / |union|`
4. Return mutual friend IDs and similarity ratio

**Time Complexity**: O(min(deg1, deg2))  
**Space Complexity**: O(min(deg1, deg2))

**Algorithm**: Set intersection using optimized iteration through smaller set

---

### 2. PageRank
**File**: `Features/pagerank.hpp`  
**Class**: `PageRankCalculator`

**How it works:**
1. Initialize all nodes with equal rank (1/N)
2. Iteratively update ranks:
   - Each node distributes its rank to nodes it follows
   - Damping factor (0.85) models random jumps
   - Dangling nodes (no outgoing edges) distribute evenly
3. Repeat for specified iterations (default: 20)
4. Converges to steady-state importance scores

**Time Complexity**: O(E × iterations)  
**Space Complexity**: O(V)

**Algorithm**: Iterative PageRank with damping factor

---

### 3. Community Detection
**File**: `Features/community_detection.hpp`  
**Class**: `CommunityDetector`

**Method 1: Label Propagation**
1. Initialize: Each node gets its own ID as community label
2. Iterate:
   - For each node, count labels of its friends
   - Assign most frequent friend's label to this node
3. Repeat until convergence or max iterations
4. Group nodes with same label into communities

**Time Complexity**: O(V × avg_degree × iterations)  
**Space Complexity**: O(V)

**Method 2: Greedy Modularity (Louvain-like)**
- Optimizes modularity score
- More accurate but slower
- Time: O(E × iterations)

---

### 4. Influencer Ranking
**File**: `Features/influencer_ranking.hpp`  
**Class**: `InfluencerRanker`

**How it works:**
1. Calculate PageRank scores for all users
2. For each user, compute influence score:
   - Follower count (30% weight)
   - Fan count - one-way followers (35% weight)
   - Friend count - mutual connections (15% weight)
   - PageRank score (25% weight)
3. Use min-heap to maintain top K efficiently
4. Sort and return top K with ranks

**Time Complexity**: O(V × log(K) + E × I)  
**Space Complexity**: O(V + K)

**Algorithm**: PageRank + weighted scoring + heap-based top-K selection

---

### 5. Shortest Path Finding
**File**: `Features/short_path.hpp`  
**Class**: `OptimizedDistanceCalculator`

**How it works:**
1. Check if nodes are directly connected (friends/following)
2. If not, use **Bidirectional BFS**:
   - Start BFS from source (forward)
   - Start BFS from target (backward)
   - When paths meet, reconstruct full path
3. Fallback to unidirectional BFS if bidirectional fails
4. Cache results for repeated queries

**Time Complexity**: O(V + E) average case  
**Space Complexity**: O(V)

**Algorithm**: Bidirectional Breadth-First Search with path reconstruction

---

### 6. Friend Recommendations
**File**: `Features/friend_recommender.hpp`  
**Class**: `FriendRecommender`

**How it works:**
1. Find candidates: "friends of friends" (excluding existing friends)
2. Score each candidate using weighted factors:
   - Mutual friends count (40% weight)
   - Common interests (25% weight)
   - Geographic proximity (20% weight)
   - Community similarity (15% weight)
3. Sort by total score
4. Return top K recommendations with explanations

**Time Complexity**: O(V × avg_degree²)  
**Space Complexity**: O(V)

**Algorithm**: Friends-of-friends expansion + multi-factor scoring

---

### 7. Friendship Score
**File**: `Features/friendshipscore.hpp`  
**Class**: `FriendshipScoreCalculator`

**How it works:**
1. Check if nodes are friends (mutual connection or friend edge)
2. Calculate base score: 1.5 for friends, 2.5 for non-friends
3. Apply weighted factors (reduce score = stronger friendship):
   - Mutual friends count (30% weight)
   - Message count (25% weight)
   - Transitive: friendship scores of mutual friends (15% weight)
   - Time of connection (15% weight)
   - Geographic proximity (10% weight)
   - Common interests (10% weight)
4. Normalize to [1.0, 3.0] range
   - Friends: [1.0, 2.0]
   - Non-friends: [2.0, 3.0]

**Time Complexity**: O(avg_degree + M) where M = mutual friends  
**Space Complexity**: O(avg_degree)

**Algorithm**: Multi-factor weighted scoring with normalization

---

### 8. User Search (Autocomplete)
**File**: `Features/user_search.hpp`  
**Class**: `UserSearchIndex`

**How it works:**
1. **Build phase**: Insert all user names into Trie
   - Each character creates a node
   - Store user IDs at each node (allows prefix matching)
2. **Search phase**:
   - Navigate Trie following prefix characters
   - If prefix path exists, collect all user IDs from that node and children
   - Sort results alphabetically
   - Return top K matches

**Time Complexity**: 
- Build: O(V × L) where L = average name length
- Search: O(P + R) where P = prefix length, R = results

**Space Complexity**: O(V × L)

**Algorithm**: Trie (Prefix Tree) data structure

---

### 9. Centrality Analysis
**File**: `Features/centrality.hpp`  
**Class**: `CentralityAnalyzer`

**Metrics calculated:**
- **Degree Centrality**: Number of connections / (V-1)
- **Closeness Centrality**: Average distance to all other nodes
- **Clustering Coefficient**: How connected a node's neighbors are

**Time Complexity**: O(V + E) for degree, O(V × (V + E)) for closeness  
**Space Complexity**: O(V)

---

## API Server

**File**: `api_server.cpp`  
**Class**: `SimpleHTTPServer`

### Endpoints

| Endpoint | Method | Parameters | Returns |
|----------|--------|------------|---------|
| `/api/graph` | GET | `?date=YYYY-MM-DD` | Graph nodes and edges |
| `/api/dates` | GET | - | Available dates and default |
| `/api/search` | GET | `?q=prefix` | User search results |
| `/api/mutual-friends` | GET | `?user1=X&user2=Y` | Mutual friends result |
| `/api/leaderboard` | GET | `?top_k=10` | Influencer rankings |
| `/api/communities` | GET | - | Community detection results |
| `/api/path` | GET | `?source=X&target=Y` | Shortest path result |
| `/api/recommendations` | GET | `?user_id=X&count=10` | Friend recommendations |

### Features
- **Multi-dataset support**: Load different daily snapshots via `date` parameter
- **Graph caching**: Caches graph context per date
- **JSON responses**: All responses in JSON format
- **Error handling**: Proper HTTP status codes
- **CORS support**: Allows frontend cross-origin requests

## Compilation

### Windows
```bash
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server.exe -lws2_32
```

### Linux/Mac
```bash
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

**Flags:**
- `-std=c++17`: C++17 standard
- `-O2`: Optimization level 2
- `-I.`: Include current directory (for nlohmann/json.hpp)
- `-lws2_32` (Windows) / `-lpthread` (Linux): Socket libraries

## Running the Server

```bash
./api_server.exe <nodes.json> <edges.json> <metadata.json>
```

Example:
```bash
./api_server.exe ../dataset/data/generated/2024-01-01/nodes.json \
                 ../dataset/data/generated/2024-01-01/edges.json \
                 ../dataset/data/generated/2024-01-01/metadata.json
```

Server runs on `http://localhost:8080`

## Data Structures Used

- **`unordered_map`**: O(1) average lookup for nodes
- **`set`**: O(log n) insertion, O(1) membership check for friends/followers
- **`vector`**: Dynamic arrays for paths, results
- **`priority_queue`**: Min-heap for top-K selection
- **`queue`**: BFS traversal
- **Trie**: Prefix tree for autocomplete

## Performance Optimizations

1. **Caching**: Algorithm results cached to avoid recomputation
2. **Set operations**: Efficient set intersection using smaller set iteration
3. **Bidirectional BFS**: Reduces search space for path finding
4. **Heap-based top-K**: O(V log K) instead of O(V log V) for leaderboard
5. **Early termination**: Algorithms stop when convergence detected
6. **Graph caching**: Per-date graph contexts cached in memory

## Testing

Test files in `test/` directory:
- `test_features.cpp`: Feature algorithm tests
- `uni_test.cpp`: Unit tests
- `test2.cpp`: Additional tests

Compile and run:
```bash
g++ -std=c++17 test_features.cpp -o test_features.exe
./test_features.exe
```

## Dependencies

- **C++17** compiler (g++ or MSVC)
- **nlohmann/json**: Header-only JSON library (included)
- **Windows Sockets** (Windows) or **POSIX sockets** (Linux/Mac)

## Algorithm Complexity Summary

| Algorithm | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Mutual Friends | O(min(deg1, deg2)) | O(min(deg1, deg2)) |
| PageRank | O(E × I) | O(V) |
| Community Detection | O(V × avg_degree × I) | O(V) |
| Influencer Ranking | O(V log K + E × I) | O(V + K) |
| Shortest Path | O(V + E) | O(V) |
| Friend Recommendations | O(V × avg_degree²) | O(V) |
| Friendship Score | O(avg_degree + M) | O(avg_degree) |
| User Search | O(P + R) | O(V × L) |
| Centrality | O(V + E) to O(V × (V + E)) | O(V) |

Where:
- V = number of nodes
- E = number of edges
- I = iterations
- K = top K results
- deg = degree (connections)
- P = prefix length
- R = results count
- L = average name length
- M = mutual friends count

