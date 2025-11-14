# Frontend - Social Network Graph Visualizer

Interactive web-based visualization for the Social Network Graph Analysis System.

## Overview

This frontend provides a real-time, interactive visualization of social network graphs using `vis-network`. It connects to a C++ backend API server running on `http://localhost:8080` to display graph data and execute various graph analysis algorithms.

## How It Works

### Architecture

The frontend is a **single-page application (SPA)** built with vanilla JavaScript, HTML, and CSS. It uses a modular architecture:

```
frontend/
├── index.html          # Main HTML structure and UI layout
├── css/
│   └── styles.css      # All styling and visual design
└── js/
    ├── main.js         # Graph initialization & vis-network setup
    ├── api.js          # Backend API communication layer
    ├── features.js     # Feature button handlers and result display
    ├── search.js       # Search bar and autocomplete functionality
    └── utils.js        # Utility functions (node details, formatting, etc.)
```

### Core Components

#### 1. **Graph Visualization (`main.js`)**
- Initializes `vis-network` with nodes and edges from backend
- Handles user interactions: hover, click, zoom, pan
- Manages node/edge highlighting and styling
- Implements physics-based layout (Barnes-Hut algorithm)
- Updates graph when switching between dataset dates

#### 2. **API Communication (`api.js`)**
- Provides async functions to communicate with backend REST API
- Handles date parameter injection for multi-dataset support
- Error handling and response parsing
- Base URL: `http://localhost:8080`

#### 3. **Feature Handlers (`features.js`)**
- Mutual Friends: Finds and displays common connections
- Influencer Leaderboard: Shows top K influential users
- Community Detection: Colors nodes by community membership
- Shortest Path: Visualizes connection paths between users
- Friend Recommendations: Displays personalized suggestions
- All features update the graph visualization in real-time

#### 4. **Search & Autocomplete (`search.js`)**
- Trie-based prefix matching (backend handles search)
- Debounced input (300ms delay)
- Dropdown results with click-to-select
- Auto-focuses selected user in graph

#### 5. **Utilities (`utils.js`)**
- Node detail panel generation
- Interest name mapping (converts placeholders to real names)
- Number/date formatting
- Loading/error state management

### User Interface

**Toolbar:**
- Search bar with autocomplete
- Date navigation controls (prev/next day)
- Feature buttons (Mutual Friends, Leaderboard, Communities, etc.)
- Reset view button

**Main Graph Area:**
- Interactive vis-network visualization
- Zoom with mouse wheel
- Pan by dragging
- Hover to highlight connections
- Click to view node details

**Side Panels:**
- Node details panel (right side)
- Feature results panel (right side)
- Modals for input (mutual friends, shortest path, recommendations)

### Visual Features

**Node Styling:**
- Size based on degree (number of connections)
- Color by community (when communities detected)
- Labels hidden by default, shown on hover/click
- Highlighted when selected

**Edge Styling:**
- Translucent by default (opacity: 0.25)
- Bright highlight on hover (opacity: 1.0)
- Thicker edges for highlighted paths
- No arrows to reduce visual clutter

**Interaction Flow:**
1. User performs action (search, click feature button)
2. Frontend sends API request to backend
3. Backend processes request and returns JSON
4. Frontend updates visualization and displays results
5. User can interact with results (click nodes, view details)

### Date Navigation

The frontend supports multiple daily dataset snapshots:
- Fetches available dates from `/api/dates` endpoint
- Displays current date in toolbar
- Enables/disables navigation buttons based on available dates
- Reloads graph when date changes
- Maintains user's current view state

## Setup & Running

### Prerequisites
- Modern web browser (Chrome, Firefox, Edge, Safari)
- Backend API server running on `http://localhost:8080`

### Running the Frontend

**Option 1: Direct File Opening**
```
Simply open index.html in your browser
```

**Option 2: Local HTTP Server (Recommended)**

Python 3:
```bash
cd frontend
python -m http.server 8000
# Open http://localhost:8000
```

Node.js:
```bash
npm install -g http-server
cd frontend
http-server -p 8000
```

**Option 3: VS Code Live Server**
- Install "Live Server" extension
- Right-click `index.html` → "Open with Live Server"

### API Endpoints Used

| Endpoint | Method | Purpose |
|----------|--------|---------|
| `/api/graph?date=YYYY-MM-DD` | GET | Get graph nodes and edges |
| `/api/dates` | GET | Get available dataset dates |
| `/api/search?q=prefix` | GET | User search autocomplete |
| `/api/mutual-friends?user1=X&user2=Y` | GET | Find mutual friends |
| `/api/leaderboard?top_k=10` | GET | Get influencer leaderboard |
| `/api/communities` | GET | Detect communities |
| `/api/path?source=X&target=Y` | GET | Find shortest path |
| `/api/recommendations?user_id=X&count=10` | GET | Get friend recommendations |

## Browser Compatibility

- ✅ Chrome/Edge (recommended)
- ✅ Firefox
- ✅ Safari
- ✅ Opera

**Requirements:**
- ES6+ JavaScript support
- Fetch API
- CSS Grid/Flexbox support

## Troubleshooting

### Graph Not Loading
- **Check browser console** (F12) for errors
- **Verify backend is running** on port 8080
- **Check CORS** if using `file://` protocol (use HTTP server instead)
- **Verify dataset files exist** in backend directory

### API Errors
- Ensure backend server is running
- Check API endpoint URLs in `js/api.js`
- Verify network tab in browser DevTools
- Check backend console for errors

### Visualization Issues
- Clear browser cache (Ctrl+Shift+Delete)
- Check if vis-network CDN is loading
- Verify JavaScript console for errors
- Try different browser

### Date Navigation Disabled
- Backend must be recompiled with date navigation support
- Check `/api/dates` endpoint returns valid JSON
- Verify dataset files exist for multiple dates

## Development

### Modifying the Frontend

1. **Styling**: Edit `css/styles.css`
2. **Functionality**: Edit files in `js/` directory
3. **Structure**: Edit `index.html`
4. **No build step required** - just refresh browser

### Adding New Features

1. Add button in `index.html`
2. Add handler in `features.js`
3. Add API function in `api.js` (if new endpoint)
4. Update visualization in `main.js` (if needed)

## Dependencies

All dependencies loaded via CDN (no package manager needed):
- `vis-network.min.js` - Graph visualization library
- No npm/node_modules required

## File Structure Details

```
frontend/
├── index.html              # Main HTML structure
│   ├── Toolbar (search, buttons, date controls)
│   ├── Graph container (vis-network canvas)
│   └── Side panels (node details, feature results)
├── css/
│   └── styles.css          # Complete styling
│       ├── Layout (flexbox/grid)
│       ├── Components (buttons, modals, panels)
│       └── Graph styling
└── js/
    ├── main.js             # ~630 lines
    │   ├── Graph initialization
    │   ├── vis-network setup
    │   ├── Event handlers
    │   └── Visualization updates
    ├── api.js              # ~100 lines
    │   ├── API_BASE_URL
    │   ├── fetchAPI()
    │   └── Feature-specific API calls
    ├── features.js         # ~510 lines
    │   ├── Button handlers
    │   ├── Result display functions
    │   └── Graph highlighting
    ├── search.js           # ~130 lines
    │   ├── Search input handler
    │   ├── Autocomplete display
    │   └── User selection
    └── utils.js            # ~180 lines
        ├── Node detail HTML
        ├── Interest mapping
        └── Formatting utilities
```

## Performance Considerations

- **Graph size**: Optimized for 100-1000 nodes
- **Edge rendering**: Translucent edges reduce visual clutter
- **Physics simulation**: Barnes-Hut algorithm for efficient layout
- **Caching**: Backend caches algorithm results
- **Lazy loading**: Graph loads on page initialization

## Future Enhancements

- Export graph as image
- Graph statistics dashboard
- Filter nodes by attributes
- Timeline animation for date changes
- 3D graph visualization option

