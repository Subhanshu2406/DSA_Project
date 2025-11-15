# Social Network Graph Analysis System

A comprehensive Data Structures and Algorithms project that analyzes social network graphs using efficient algorithms and provides an interactive web-based visualization.

## Project Overview

This project implements a full-stack social network analysis system with:
- **C++ Backend**: High-performance graph algorithms and REST API server
- **Python Dataset Generator**: Realistic social graph data generation with temporal evolution
- **JavaScript Frontend**: Interactive graph visualization using vis-network

The system analyzes social networks with features like mutual friends, influencer ranking, community detection, shortest path finding, friend recommendations, and more.

## Features

### Graph Analysis Algorithms
-  **Mutual Friends**: Find common connections between users 
-  **Influencer Ranking**: PageRank-based leaderboard 
-  **Community Detection**: Label propagation algorithm 
-  **Shortest Path**: Bidirectional BFS path finding 
-  **Friend Recommendations**: Multi-factor scoring system 
-  **Friendship Score**: Relationship strength calculation
-  **User Search**: Trie-based autocomplete 
-  **Centrality Analysis**: Degree, closeness, clustering metrics

### Visualization Features
- Interactive graph with zoom, pan, hover effects
- Real-time node/edge highlighting
- Community color coding
- Path visualization
- Date navigation for temporal data
- Node detail panels

## Project Structure

```
DSA_Project/
├── Backend/                    # C++ backend implementation
│   ├── Features/               # Algorithm implementations
│   │   ├── mutual_friends.hpp
│   │   ├── pagerank.hpp
│   │   ├── community_detection.hpp
│   │   ├── influencer_ranking.hpp
│   │   ├── short_path.hpp
│   │   ├── friend_recommender.hpp
│   │   ├── friendshipscore.hpp
│   │   ├── user_search.hpp
│   │   └── centrality.hpp
│   ├── api_server.cpp          # REST API server
│   ├── Algorithm.hpp           # Unified algorithm interface
│   ├── graph_generator.hpp     # Graph data structure
│   └── README.md               # Backend documentation
├── frontend/                    # Web-based visualization
│   ├── index.html
│   ├── css/
│   │   └── styles.css
│   ├── js/
│   │   ├── main.js            # Graph initialization
│   │   ├── api.js             # API communication
│   │   ├── features.js        # Feature handlers
│   │   ├── search.js          # Search & autocomplete
│   │   └── utils.js           # Utilities
│   └── README.md              # Frontend documentation
├── dataset/                     # Python dataset generator
│   ├── dataset_generator/
│   │   ├── graph_generator.py
│   │   ├── clustering.py
│   │   ├── relationship_manager.py
│   │   ├── time_evolution.py
│   │   └── export.py
│   ├── data/generated/         # Generated dataset files
│   ├── config.py
│   ├── generate_dataset.py
│   └── README.md              # Dataset documentation
└── README.md                   # This file
```

## Quick Start

### Prerequisites
- **C++17 compiler** (g++ or MSVC)
- **Python 3.7+**
- **Modern web browser** (Chrome, Firefox, Edge, Safari)

### Step 1: Generate Dataset

```bash
cd dataset
pip install -r requirements.txt
python generate_dataset.py
```

This creates daily snapshots in `data/generated/YYYY-MM-DD/` directory.

### Step 2: Compile Backend

**Windows:**
```bash
cd Backend
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server.exe -lws2_32
```

**Linux/Mac:**
```bash
cd Backend
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

### Step 3: Run Backend Server

```bash
# Windows
./api_server.exe ../dataset/data/generated/2024-01-01/nodes.json \
                 ../dataset/data/generated/2024-01-01/edges.json \
                 ../dataset/data/generated/2024-01-01/metadata.json

# Linux/Mac
./api_server ../dataset/data/generated/2024-01-01/nodes.json \
             ../dataset/data/generated/2024-01-01/edges.json \
             ../dataset/data/generated/2024-01-01/metadata.json
```

Server runs on `http://localhost:8080`

### Step 4: Open Frontend

**Python HTTP Server**
```bash
cd frontend
python -m http.server 8000
# Open http://localhost:8000 in browser
```

### Backend Compilation

**Windows (MinGW/MSVC):**
```bash
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server.exe -lws2_32
```

**Linux:**
```bash
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

**Mac:**
```bash
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

**Flags:**
- `-std=c++17`: C++17 standard
- `-O2`: Optimization level 2
- `-I.`: Include current directory (for nlohmann/json.hpp)
- `-lws2_32` (Windows) / `-lpthread` (Linux/Mac): Socket libraries


## Technology Stack

**Backend:**
- C++17
- nlohmann/json (header-only)
- Windows Sockets (Winsock2) / POSIX sockets

**Frontend:**
- HTML5/CSS3/JavaScript (ES6+)
- vis-network (graph visualization)

**Dataset Generator:**
- Python 3
- Standard library (no external dependencies required)

## Algorithms Implemented

| Algorithm | Time Complexity | Space Complexity | File |
|-----------|----------------|------------------|------|
| Mutual Friends | O(min(deg1, deg2)) | O(min(deg1, deg2)) | `mutual_friends.hpp` |
| PageRank | O(E × I) | O(V) | `pagerank.hpp` |
| Community Detection | O(V × avg_degree × I) | O(V) | `community_detection.hpp` |
| Influencer Ranking | O(V log K + E × I) | O(V + K) | `influencer_ranking.hpp` |
| Shortest Path | O(V + E) | O(V) | `short_path.hpp` |
| Friend Recommendations | O(V × avg_degree²) | O(V) | `friend_recommender.hpp` |
| Friendship Score | O(avg_degree + M) | O(avg_degree) | `friendshipscore.hpp` |
| User Search (Trie) | O(P + R) | O(V × L) | `user_search.hpp` |
| Centrality | O(V + E) to O(V × (V + E)) | O(V) | `centrality.hpp` |

Where: V = nodes, E = edges, I = iterations, K = top K, deg = degree, P = prefix length, R = results, L = name length, M = mutual friends


## Configuration

### Dataset Configuration
Edit `dataset/config.py`:
- Number of nodes (default: 100)
- Number of days (default: 5)
- Connection probabilities
- Relationship change rates

### Backend Configuration
- API port: 8080 (hardcoded in `api_server.cpp`)
- Dataset paths: Passed as command-line arguments

### Frontend Configuration
- API base URL: `http://localhost:8080` (in `js/api.js`)


## Documentation

- **[Backend README](Backend/README.md)**: Algorithm implementations and API details
- **[Frontend README](frontend/README.md)**: Frontend architecture and usage
- **[Dataset README](dataset/README.md)**: Dataset generation methodology


## License

Educational project for Data Structures and Algorithms course.

## Author

Subhanshu Gupta (B24CS1072)<br>
Jimeet Shastri  (B24CS1034)<br>
Gokul Bansal    (B24CM1025)

---
