# Social Graph Analysis & Visualization System

A comprehensive social network analysis platform featuring realistic dataset generation, high-performance graph algorithms, and interactive web-based visualization.

## 🎯 Project Overview

This project provides a complete solution for analyzing social networks:
- **Dataset Generator**: Creates realistic social media network data with temporal evolution
- **Backend Engine**: High-performance C++ server with graph analysis algorithms
- **Frontend Visualization**: Interactive web interface for exploring and analyzing networks

## ✨ Features

### Dataset Generation
- Realistic social network graphs (100-1000+ nodes)
- Hybrid clustering (geographic + interest-based)
- Temporal evolution with daily snapshots
- Friend and fan relationship dynamics
- Viral node growth modeling

### Graph Analysis Algorithms
- **Mutual Friends**: Find common connections between users
- **Friend Recommendations**: Multi-factor recommendation system
- **PageRank**: Calculate user importance scores
- **Community Detection**: Label propagation and modularity optimization
- **Influencer Ranking**: Composite scoring for top influencers
- **Centrality Metrics**: Degree, closeness, clustering coefficient
- **Shortest Path**: Bidirectional BFS with friendship scoring
- **User Search**: Trie-based autocomplete search
- **Friendship Score**: Multi-factor relationship strength calculator

### Interactive Visualization
- Real-time graph rendering with vis-network
- Node and edge interaction (hover, click, select)
- Search with autocomplete
- Date navigation for temporal analysis
- Feature buttons for algorithm execution
- Community color coding
- Path highlighting
- Node detail panels

## 🏗️ Project Structure

```
DSA_Project/
├── dataset/                 # Dataset generator (Python)
│   ├── dataset_generator/  # Core modules
│   ├── data/generated/     # Generated datasets
│   └── config.py           # Configuration
├── Backend/                # C++ API server
│   ├── Features/           # Algorithm implementations
│   ├── api_server.cpp      # HTTP server
│   └── Algorithm.hpp       # Unified interface
├── frontend/               # Web visualization
│   ├── js/                 # JavaScript modules
│   ├── css/                # Styling
│   └── index.html          # Main page
└── README.md               # This file
```

## 🚀 Quick Start

### Prerequisites

**For Dataset Generation**:
- Python 3.7+
- pip

**For Backend**:
- C++17 compiler (g++, clang++, or MSVC)
- Windows: Winsock2 library
- nlohmann/json.hpp (included)

**For Frontend**:
- Modern web browser (Chrome, Firefox, Safari, Edge)
- No build step required (pure HTML/CSS/JS)

### Installation

1. **Clone the repository**:
```bash
git clone <repository-url>
cd DSA_Project
```

2. **Generate dataset** (optional, pre-generated data included):
```bash
cd dataset
pip install -r requirements.txt
python generate_dataset.py
```

3. **Build backend**:
```bash
cd Backend

# Windows
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server.exe -lws2_32

# Linux/Mac
g++ -std=c++17 -O2 -I. api_server.cpp -o api_server -lpthread
```

4. **Start backend server**:
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

5. **Open frontend**:
- Open `frontend/index.html` in a web browser
- Or serve via HTTP server:
```bash
cd frontend
python -m http.server 8000
# Then open http://localhost:8000
```

## 📖 Usage

### Frontend Interface

1. **Search Users**: Type in the search bar to find users with autocomplete
2. **Navigate Dates**: Use ◀ ▶ buttons to switch between daily snapshots
3. **Explore Graph**: 
   - Click nodes to see details
   - Hover over nodes/edges for highlights
   - Zoom and pan to explore
4. **Run Algorithms**:
   - **Mutual Friends**: Find common connections
   - **Influencer Leaderboard**: See top influencers
   - **Community Detection**: Color nodes by community
   - **Shortest Path**: Find path between users (with friendship score)
   - **Recommendations**: Get friend suggestions

### Backend API

All endpoints return JSON:

- `GET /api/graph?date=YYYY-MM-DD` - Get graph data
- `GET /api/search?q=prefix` - Search users
- `GET /api/mutual-friends?user1=X&user2=Y` - Mutual friends
- `GET /api/recommendations?user_id=X&count=10` - Recommendations
- `GET /api/influencer-leaderboard?top_k=10` - Top influencers
- `GET /api/communities` - Community detection
- `GET /api/path?source=X&target=Y` - Shortest path
- `GET /api/dates` - Available dates

See `Backend/README.md` for complete API documentation.

## 🧪 Testing

### Backend Tests
```bash
cd Backend
g++ -std=c++17 test_features.cpp -o test_features.exe
./test_features.exe
```

### Frontend Testing
- Open browser developer console (F12)
- Check for JavaScript errors
- Test API connectivity via Network tab

## 📊 Performance

- **Graph Loading**: < 1 second for 1000 nodes
- **Query Response**: < 100ms for most algorithms
- **Visualization**: Smooth 60 FPS rendering
- **Memory Usage**: ~50MB for 1000 nodes

## 🛠️ Technology Stack

- **Dataset**: Python (NumPy, Pandas)
- **Backend**: C++17, HTTP Server, JSON
- **Frontend**: HTML5, CSS3, JavaScript (ES6+), vis-network
- **Data Format**: JSON

## 📚 Documentation

- **[Frontend README](frontend/README.md)**: Visualization details and interactions
- **[Backend README](Backend/README.md)**: Algorithms and API documentation
- **[Dataset README](dataset/README.md)**: Data generation algorithms and concepts

## 🎓 Educational Value

This project demonstrates:
- Graph data structures and algorithms
- Social network analysis techniques
- RESTful API design
- Interactive data visualization
- Temporal data modeling
- Performance optimization

## 🔧 Configuration

### Dataset Generation
Edit `dataset/config.py` to customize:
- Number of nodes
- Number of days
- Connection probabilities
- Relationship dynamics

### Backend
Modify algorithm parameters in respective header files:
- `Features/pagerank.hpp`: Damping factor, iterations
- `Features/community_detection.hpp`: Detection method, iterations
- `Features/friend_recommender.hpp`: Scoring weights

### Frontend
Edit `frontend/css/styles.css` for visual customization.

## 🐛 Troubleshooting

**Backend won't compile**:
- Ensure C++17 compiler is installed
- Check that nlohmann/json.hpp is in `Backend/nlohmann/`
- Windows: Link with `-lws2_32`

**Frontend shows no graph**:
- Verify backend is running on port 8080
- Check browser console for errors
- Ensure dataset files exist

**API returns 404**:
- Verify dataset paths are correct
- Check that date parameter matches available dates
- Ensure backend was compiled with latest code

## 📝 License

This project is part of a DSA (Data Structures and Algorithms) course project.

## 👥 Contributing

This is an educational project. For improvements:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## 🙏 Acknowledgments

- **vis-network**: Graph visualization library
- **nlohmann/json**: JSON library for C++
- **NetworkX**: Inspiration for graph algorithms

---

**Built with ❤️ for learning graph algorithms and social network analysis**

