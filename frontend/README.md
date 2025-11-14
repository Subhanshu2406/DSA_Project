# Frontend - Social Graph Visualization

## Overview

The frontend is an interactive web-based visualization system built with HTML, CSS, and JavaScript that provides a comprehensive interface for exploring and analyzing social network graphs. It uses **vis-network** for graph rendering and provides real-time interaction with the backend API.

## Technology Stack

- **HTML5/CSS3**: Structure and styling
- **JavaScript (ES6+)**: Core functionality and interactions
- **vis-network**: Graph visualization library (replaces Cytoscape.js for better performance with dense graphs)
- **Fetch API**: RESTful communication with backend

## Architecture

### File Structure

```
frontend/
├── index.html          # Main HTML structure
├── css/
│   └── styles.css     # All styling and responsive design
├── js/
│   ├── main.js        # Core graph initialization and vis-network setup
│   ├── api.js         # Backend API communication layer
│   ├── search.js      # User search with autocomplete
│   ├── features.js    # Feature button handlers (mutual friends, path, etc.)
│   └── utils.js       # Utility functions (interest mapping, formatting)
└── test_api.html      # API testing page
```

## Core Components

### 1. Graph Visualization (`main.js`)

#### Initialization
- Loads graph data from backend API (`/api/graph`)
- Converts API response format to vis-network DataSet format
- Initializes vis-network with optimized physics settings for dense graphs

#### Graph Configuration
- **Physics Engine**: Barnes-Hut algorithm with repulsion
  - `gravitationalConstant: -20000` - Strong repulsion to prevent overlap
  - `springLength: 200` - Optimal edge length
  - `springConstant: 0.01` - Edge stiffness
  - `avoidOverlap: 1` - Prevents node collision
  
- **Node Styling**:
  - Size based on degree (15-35px range)
  - Labels hidden by default (`font.size: 0`) to reduce clutter
  - Labels appear on hover/click
  - Color-coded by community (when community detection is applied)

- **Edge Styling**:
  - Translucent by default (`opacity: 0.25`) to reduce visual clutter
  - No arrows (reduces hairball effect)
  - Discrete smooth type for cleaner appearance
  - Width: 1.5px (2px for friend relationships)

#### Interaction Handlers

**Node Hover**:
- Shows node label
- Highlights connected edges in bright orange (`#f39c12`)
- Makes other edges more translucent (`opacity: 0.15`)
- Increases edge width to 4px for visibility

**Node Click**:
- Selects node
- Shows node details in left sidebar
- Displays node label
- Focuses camera on node with animation

**Edge Hover**:
- Highlights hovered edge in orange
- Increases opacity to 1.0
- Increases width to 4px

**Node Blur**:
- Hides node label
- Restores all edges to default translucent state

### 2. Search with Autocomplete (`search.js`)

#### Features
- **Real-time Search**: Debounced input (300ms delay)
- **Autocomplete**: Shows up to 10 matching users
- **Trie-based Backend**: Fast prefix matching via `/api/search` endpoint
- **User Selection**: Click or Enter key to select user
- **Visual Feedback**: 
  - Usernames in dark, bold text
  - User IDs in lighter gray
  - Hover effects with blue highlighting

#### Search Flow
1. User types in search input
2. After 300ms delay, sends request to `/api/search?q=prefix`
3. Backend returns matching users with IDs and names
4. Displays results in dropdown below search bar
5. On selection, highlights node and shows details

### 3. Feature Implementations (`features.js`)

#### Mutual Friends
- **Input**: Two user IDs via modal
- **Process**: Calls `/api/mutual-friends?user1=X&user2=Y`
- **Display**: 
  - Shows similarity ratio, mutual friends count
  - Lists all mutual friend IDs
  - Highlights both users and all mutual friends in graph
  - Color coding: selected users (blue), mutual friends (green)

#### Influencer Leaderboard
- **Trigger**: Click "Influencer Leaderboard" button
- **Process**: Calls `/api/influencer-leaderboard?top_k=10`
- **Display**:
  - Top 10 users ranked by influence score
  - Shows: rank, name, influence score, followers, fans, friends, PageRank
  - Clickable items that focus on selected user
- **Algorithm**: Combines PageRank, follower count, and fan count

#### Community Detection
- **Trigger**: Click "Community Detection" button
- **Process**: Calls `/api/communities`
- **Display**:
  - Total number of communities
  - Legend with community colors
  - Community size and density metrics
- **Visualization**:
  - Colors all nodes by community membership
  - Each community gets unique color from predefined palette
  - Updates graph in real-time

#### Shortest Path
- **Input**: Source and target user IDs via modal
- **Process**: Calls `/api/path?source=X&target=Y`
- **Display**:
  - Path length (number of hops)
  - Path description (e.g., "1 -> 5 -> 12")
  - **Friendship Score**: 
    - Prominently displayed with color coding
    - Green for friends (score 1.0-2.0)
    - Red for non-friends (score 2.0-3.0)
    - Includes explanation text
  - Highlights path nodes and edges
  - Special styling for source and target nodes
- **Algorithm**: Bidirectional BFS for optimal performance

#### Friend Recommendations
- **Input**: User ID and count via modal
- **Process**: Calls `/api/recommendations?user_id=X&count=10`
- **Display**:
  - Top recommendations with scores
  - Shows: name, total score, recommendation reason
  - Metrics: mutual friends, common interests, geographic distance
  - Clickable items to focus on recommended users
- **Algorithm**: Multi-factor scoring (mutual friends, interests, geography, community)

### 4. Date Navigation

#### Features
- **Dynamic Dataset Loading**: Switch between daily snapshots
- **Navigation Controls**: Previous/Next day buttons
- **Date Display**: Shows current dataset date
- **Auto-detection**: Fetches available dates from `/api/dates`
- **Graceful Fallback**: Disables navigation if endpoint unavailable

#### Implementation
- Fetches available dates on initialization
- Updates `currentDatasetDate` global variable
- Appends `?date=YYYY-MM-DD` to all API requests
- Re-initializes graph when date changes

### 5. Interest Mapping (`utils.js`)

Converts placeholder interests to real names:
- `interest_0` → Technology
- `interest_1` → Music
- `interest_2` → Sports
- ... (20 total categories)

Applied automatically when displaying node details.

## Visual Design

### Color Scheme
- **Primary**: `#3498db` (Blue) - Buttons, highlights
- **Secondary**: `#2c3e50` (Dark Blue) - Text, headers
- **Success**: `#27ae60` (Green) - Friends, positive indicators
- **Warning**: `#f39c12` (Orange) - Hover highlights
- **Error**: `#e74c3c` (Red) - Non-friends, errors
- **Background**: `#ecf0f1` (Light Gray) - Graph canvas

### Layout
- **Toolbar**: Fixed top bar with search, date controls, feature buttons
- **Graph Container**: Full-width, full-height visualization area
- **Sidebars**: 
  - Left: Node details (collapsible)
  - Right: Feature results (collapsible)

### Responsive Design
- Full-screen layout optimized for desktop
- Sidebars slide in/out smoothly
- Modals centered with backdrop
- Scrollable content areas

## Performance Optimizations

1. **Graph Rendering**:
   - Labels hidden by default (reduces rendering load)
   - Translucent edges (reduces visual complexity)
   - Physics stabilization before interaction

2. **API Calls**:
   - Debounced search (300ms)
   - Cached date information
   - Efficient data structures (vis DataSet)

3. **Memory Management**:
   - Graph context destroyed on date change
   - Event listeners properly cleaned up
   - No memory leaks in long-running sessions

## Browser Compatibility

- **Chrome/Edge**: Full support (recommended)
- **Firefox**: Full support
- **Safari**: Full support
- **IE11**: Not supported (uses modern JavaScript)

## Usage

1. **Start Backend**: Ensure API server is running on `http://localhost:8080`
2. **Open Frontend**: Open `index.html` in a web browser
3. **Interact**:
   - Type in search bar to find users
   - Click nodes to see details
   - Use feature buttons to run algorithms
   - Navigate dates to see temporal changes
   - Hover over nodes/edges for highlights

## Troubleshooting

**Graph not loading**:
- Check browser console for errors
- Verify backend is running
- Check API endpoint connectivity

**No nodes visible**:
- Check if dataset files exist
- Verify date parameter is correct
- Check network tab for API responses

**Features not working**:
- Ensure backend is compiled with latest code
- Check browser console for JavaScript errors
- Verify API endpoints are accessible

## Future Enhancements

- Export graph as image
- Save/load graph layouts
- Filter nodes by attributes
- Timeline animation for date changes
- 3D graph view option
- Custom color schemes

