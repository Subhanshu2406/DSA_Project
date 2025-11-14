# Social Graph Visualization Frontend

A web-based visualization tool for exploring social network graphs using vis-network.

## Features

- **Interactive Graph Visualization**: Zoom, pan, and explore the social network
- **User Search with Autocomplete**: Find users quickly with real-time search suggestions
- **Mutual Friends Analysis**: Find common connections between two users
- **Influencer Leaderboard**: View top influencers ranked by various metrics
- **Community Detection**: Visualize communities with color coding
- **Shortest Path Finding**: Discover connections between users
- **Friend Recommendations**: Get personalized friend suggestions

## Setup

### Prerequisites

1. **Backend API Server**: The C++ API server must be running
2. **Web Browser**: Modern browser with JavaScript enabled
3. **Local Web Server**: Required due to CORS (see below)

### Running the Backend API Server

```bash
cd Backend
g++ -std=c++17 -I. -I../dataset -o api_server api_server.cpp
./api_server ../dataset/data/generated/2024-01-01/nodes.json ../dataset/data/generated/2024-01-01/edges.json ../dataset/data/generated/2024-01-01/metadata.json 8080
```

The server will start on `http://localhost:8080`

### Running the Frontend

Due to browser CORS restrictions, you need to serve the frontend files through a web server:

#### Option 1: Python HTTP Server
```bash
cd frontend
python -m http.server 8000
```

Then open `http://localhost:8000` in your browser.

#### Option 2: Node.js HTTP Server
```bash
cd frontend
npx http-server -p 8000
```

#### Option 3: VS Code Live Server
If using VS Code, install the "Live Server" extension and right-click `index.html` → "Open with Live Server"

## Usage

1. **Start the backend API server** (see above)
2. **Start a local web server** for the frontend (see above)
3. **Open the application** in your browser
4. **Wait for the graph to load** (may take a moment for large graphs)

### Basic Interactions

- **Click on a node**: View detailed information in the left sidebar
- **Zoom**: Use mouse wheel or pinch gesture
- **Pan**: Click and drag the canvas
- **Search**: Type in the search bar to find users

### Feature Buttons

- **Mutual Friends**: Enter two user IDs to find their common friends
- **Influencer Leaderboard**: View top 10 most influential users
- **Community Detection**: Color-code nodes by their community membership
- **Shortest Path**: Find the connection path between two users
- **Recommendations**: Get friend suggestions for a user
- **Reset View**: Clear all highlights and reset the view

## File Structure

```
frontend/
├── index.html          # Main HTML page
├── css/
│   └── styles.css      # All styling
├── js/
│   ├── main.js         # vis-network initialization
│   ├── api.js          # API client functions
│   ├── search.js       # Search and autocomplete
│   ├── features.js     # Feature button handlers
│   └── utils.js        # Utility functions
└── README.md           # This file
```

## API Endpoints

The frontend communicates with the backend API server at `http://localhost:8080`:

- `GET /api/graph` - Get all nodes and edges
- `GET /api/node/:id` - Get node details
- `GET /api/search?q=query` - Search users
- `POST /api/mutual-friends` - Get mutual friends
- `GET /api/influencer-leaderboard?top=N` - Get leaderboard
- `GET /api/communities` - Get community detection results
- `GET /api/path?source=X&target=Y` - Get shortest path
- `GET /api/recommendations?user=X&count=N` - Get recommendations

## Troubleshooting

### Graph doesn't load
- Check that the API server is running on port 8080
- Check browser console for errors
- Verify CORS is enabled (API server includes CORS headers)

### Search not working
- Ensure the API server has loaded the graph data
- Check network tab for API requests

### Features not responding
- Wait for the graph to fully load
- Check browser console for JavaScript errors
- Verify all JS files are loaded (check Network tab)

## Performance Notes

- Large graphs (1000+ nodes) may take time to load and render
- Community detection and path finding can be slow for large graphs
- Consider using a subset of data for development/testing

## Browser Compatibility

Tested on:
- Chrome/Edge (latest)
- Firefox (latest)
- Safari (latest)

Requires modern browser with ES6+ support.

