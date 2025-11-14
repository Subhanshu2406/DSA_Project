# Quick Start Guide - Social Network Analyzer Frontend

## How to Run the Frontend

### Step 1: Start the Backend API Server

The backend must be running before you can use the frontend. Open a terminal and run:

```bash
cd /home/jimeet/Desktop/DSA_project/Backend
./api_server.exe ../dataset/data/generated/2024-01-01/nodes.json ../dataset/data/generated/2024-01-01/edges.json ../dataset/data/generated/2024-01-01/metadata.json 8080
```

**Note:** The backend will start on port 8080. Keep this terminal open while using the frontend.

### Step 2: Start the Frontend Web Server

Open a **new terminal** and run one of these options:

#### Option A: Python HTTP Server (Recommended)
```bash
cd /home/jimeet/Desktop/DSA_project/frontend
python3 -m http.server 8000
```

#### Option B: Node.js HTTP Server
```bash
cd /home/jimeet/Desktop/DSA_project/frontend
npx http-server -p 8000
```

### Step 3: Open in Browser

Once both servers are running:
1. Open your web browser
2. Navigate to: `http://localhost:8000`
3. The graph visualization should load automatically

## What You'll See

- **Interactive Graph**: Visual representation of the social network
- **Search Bar**: Type to search for users
- **Feature Buttons**:
  - **Mutual Friends**: Find common friends between two users
  - **Influencer Leaderboard**: View top influencers
  - **Community Detection**: Color-code nodes by communities
  - **Shortest Path**: Find connections between users
  - **Recommendations**: Get friend suggestions
  - **Reset View**: Clear highlights and reset

## Troubleshooting

### "Error loading graph data"
- Make sure the backend API server is running on port 8080
- Check the backend terminal for any error messages
- Verify the dataset files exist at the specified path

### "CORS error" or blank page
- Make sure you're accessing via `http://localhost:8000` (not `file://`)
- The frontend must be served through a web server (not opened directly as a file)

### Graph doesn't load
- Check browser console (F12) for errors
- Verify the API server is responding: visit `http://localhost:8080/api/graph` in your browser

## Using Different Datasets

To use a different date's dataset, change the paths when starting the backend:

```bash
./api_server.exe ../dataset/data/generated/2024-01-02/nodes.json ../dataset/data/generated/2024-01-02/edges.json ../dataset/data/generated/2024-01-02/metadata.json 8080
```

## Stopping the Servers

- **Backend**: Press `Ctrl+C` in the backend terminal
- **Frontend**: Press `Ctrl+C` in the frontend terminal

