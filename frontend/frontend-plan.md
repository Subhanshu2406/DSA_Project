# Frontend Visualization with Cytoscape.js

## Architecture Overview

- **Backend API Server**: C++ HTTP server exposing REST endpoints for all graph algorithms
- **Frontend**: Single-page web application using Cytoscape.js for graph visualization
- **Communication**: JSON over HTTP (REST API)

## Implementation Plan

### Phase 1: Backend API Server (`Backend/api_server.cpp`)

Create HTTP server that:

- Loads graph data on startup
- Exposes REST endpoints:
  - `GET /api/graph` - Returns all nodes and edges as JSON
  - `GET /api/node/:id` - Returns node details with attributes
  - `GET /api/search?q=prefix` - Autocomplete search (uses user_search)
  - `POST /api/mutual-friends` - `{user1, user2}` returns mutual friends IDs
  - `GET /api/influencer-leaderboard?top=10` - Returns top influencers
  - `GET /api/communities` - Returns community detection results with color mapping
  - `GET /api/path?source=X&target=Y` - Returns shortest path
  - `GET /api/recommendations?user=X&count=10` - Friend recommendations
- Uses `nlohmann/json` for JSON serialization (already in project)
- Uses lightweight HTTP library (e.g., `cpp-httplib` or similar)

### Phase 2: Frontend Structure (`frontend/`)

```
frontend/
├── index.html          # Main HTML page
├── css/
│   └── styles.css     # Styling for UI components
├── js/
│   ├── main.js        # Main Cytoscape.js initialization
│   ├── api.js         # API client functions
│   ├── search.js      # Search bar with autocomplete
│   ├── features.js    # Feature button handlers
│   └── utils.js       # Helper functions
└── lib/               # External libraries (Cytoscape.js, etc.)
    └── cytoscape.min.js
```

### Phase 3: Core Visualization (`frontend/js/main.js`)

- Initialize Cytoscape instance with:
  - Layout: `cose` or `breadthfirst` for better visualization
  - Node styling: size based on degree, color by community
  - Edge styling: different colors for "friend" vs "fan" relationships
- Implement zoom controls (Cytoscape built-in)
- Node click handler: show attributes in side panel
- Edge click handler: show relationship details

### Phase 4: Search with Autocomplete (`frontend/js/search.js`)

- Search input with dropdown suggestions
- Debounced API calls to `/api/search?q=prefix`
- Display suggestions as user types
- On selection: highlight node, center view, show details
- Integrate with `UserSearchIndex` from backend

### Phase 5: Feature Buttons (`frontend/js/features.js`)

**Mutual Friends:**

- Button triggers modal/dialog asking for second user ID
- Alternative: use search to select both users
- Call `/api/mutual-friends` with both user IDs
- Highlight mutual friend nodes with distinct color
- Highlight edges connecting the three users

**Influencer Leaderboard:**

- Button shows sidebar panel with top 10 influencers
- Display: rank, name, influence score, follower count
- Clicking entry highlights that node in graph
- Nodes sized by influence score (larger = more influential)

**Community Detection:**

- Button triggers community detection algorithm
- Call `/api/communities` endpoint
- Assign unique color to each community
- Color-code all nodes by their community
- Show legend mapping community IDs to colors
- Option to toggle community colors on/off

**Additional Features:**

- **Shortest Path**: Select two nodes, highlight path between them
- **Friend Recommendations**: Show recommended connections for selected user
- **Reset View**: Clear all highlights, reset to default view

### Phase 6: UI Components (`frontend/index.html`, `frontend/css/styles.css`)

- Top toolbar: Search bar, feature buttons
- Left sidebar: Node details panel (shown on click)
- Right sidebar: Feature results (leaderboard, communities, etc.)
- Main area: Cytoscape.js canvas (full viewport)
- Responsive layout with CSS Grid/Flexbox

### Phase 7: Data Format & API Response Structure

**Node JSON format for Cytoscape:**

```json
{
  "data": {
    "id": "123",
    "label": "User Name",
    "user_id": 123,
    "name": "User Name",
    "degree": 45,
    "community": 2,
    "influence_score": 75.5
  },
  "position": {"x": 100, "y": 200}
}
```

**Edge JSON format:**

```json
{
  "data": {
    "id": "e1",
    "source": "123",
    "target": "456",
    "relationship_type": "friend",
    "weight": 1
  }
}
```

### Phase 8: Enhancements & Polish

- Loading indicators for async operations
- Error handling and user feedback
- Performance optimization for large graphs (lazy loading, clustering)
- Export functionality (PNG/SVG of current view)
- Keyboard shortcuts for common actions
- Tooltips on hover for nodes/edges

## Technical Decisions

- **HTTP Library**: Use `cpp-httplib` (header-only, lightweight) or `crow` framework
- **Cytoscape Layout**: Start with `cose` (compound spring embedder) for organic layout
- **Color Scheme**: Use distinct color palette (e.g., ColorBrewer) for communities
- **Node Sizing**: Scale by degree or influence score (configurable)
- **Edge Filtering**: Toggle between showing all edges vs only friend relationships

## Files to Create

1. `Backend/api_server.cpp` - HTTP server with REST endpoints
2. `Backend/api_server.hpp` - API server class definition (optional)
3. `frontend/index.html` - Main HTML page
4. `frontend/css/styles.css` - All styling
5. `frontend/js/main.js` - Cytoscape initialization and core logic
6. `frontend/js/api.js` - API client functions
7. `frontend/js/search.js` - Search and autocomplete
8. `frontend/js/features.js` - Feature button handlers
9. `frontend/js/utils.js` - Utility functions
10. `frontend/README.md` - Setup and usage instructions

## Dependencies

- **Backend**: HTTP server library (cpp-httplib or similar)
- **Frontend**: Cytoscape.js (CDN or local copy)
- **Optional**: jQuery for easier DOM manipulation (or vanilla JS)

## Testing Strategy

- Test API endpoints independently with curl/Postman
- Test frontend with mock data first
- Integration testing with real graph data
- Performance testing with large graphs (1000+ nodes)