#!/bin/bash

# Script to start the Social Network Analyzer Frontend
# This script starts both the backend API server and frontend web server

echo "=========================================="
echo "Social Network Analyzer - Starting..."
echo "=========================================="

# Get the project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$PROJECT_ROOT/Backend"
FRONTEND_DIR="$PROJECT_ROOT/frontend"
DATASET_DIR="$PROJECT_ROOT/dataset/data/generated/2024-01-01"

# Check if dataset files exist
if [ ! -f "$DATASET_DIR/nodes.json" ] || [ ! -f "$DATASET_DIR/edges.json" ] || [ ! -f "$DATASET_DIR/metadata.json" ]; then
    echo "Error: Dataset files not found at $DATASET_DIR"
    echo "Please ensure the dataset files exist."
    exit 1
fi

# Determine which backend executable to use (Linux or Windows)
BACKEND_EXE=""
if [ -f "$BACKEND_DIR/api_server" ]; then
    BACKEND_EXE="$BACKEND_DIR/api_server"
elif [ -f "$BACKEND_DIR/api_server.exe" ]; then
    # Check if it's actually a Linux binary (not Windows PE)
    if file "$BACKEND_DIR/api_server.exe" | grep -q "ELF"; then
        BACKEND_EXE="$BACKEND_DIR/api_server.exe"
    else
        echo "Warning: Found Windows executable, but we're on Linux."
        echo "Attempting to compile Linux version..."
        cd "$BACKEND_DIR"
        if g++ -std=c++17 -I. -I../dataset -o api_server api_server.cpp -pthread 2>/dev/null; then
            BACKEND_EXE="$BACKEND_DIR/api_server"
            echo "Successfully compiled Linux version!"
        else
            echo "Error: Failed to compile backend. Please compile manually:"
            echo "  cd Backend && g++ -std=c++17 -I. -I../dataset -o api_server api_server.cpp -pthread"
            exit 1
        fi
    fi
else
    echo "Backend executable not found. Attempting to compile..."
    cd "$BACKEND_DIR"
    if g++ -std=c++17 -I. -I../dataset -o api_server api_server.cpp -pthread 2>/dev/null; then
        BACKEND_EXE="$BACKEND_DIR/api_server"
        echo "Successfully compiled!"
    else
        echo "Error: Failed to compile backend. Please compile manually:"
        echo "  cd Backend && g++ -std=c++17 -I. -I../dataset -o api_server api_server.cpp -pthread"
        exit 1
    fi
fi

# Ensure backend executable has execute permissions
if [ ! -x "$BACKEND_EXE" ]; then
    echo "Making backend executable..."
    chmod +x "$BACKEND_EXE"
fi

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "Shutting down servers..."
    kill $BACKEND_PID $FRONTEND_PID 2>/dev/null
    exit
}

trap cleanup SIGINT SIGTERM

# Start backend server
echo "Starting backend API server on port 8080..."
cd "$BACKEND_DIR"
"$BACKEND_EXE" "$DATASET_DIR/nodes.json" "$DATASET_DIR/edges.json" "$DATASET_DIR/metadata.json" 8080 &
BACKEND_PID=$!

# Wait a moment for backend to start
sleep 2

# Check if backend started successfully
if ! kill -0 $BACKEND_PID 2>/dev/null; then
    echo "Error: Backend server failed to start"
    exit 1
fi

echo "Backend server started (PID: $BACKEND_PID)"

# Start frontend server
echo "Starting frontend web server on port 8000..."
cd "$FRONTEND_DIR"

# Check if Python 3 is available
if command -v python3 &> /dev/null; then
    python3 -m http.server 8000 &
    FRONTEND_PID=$!
    FRONTEND_TYPE="Python"
elif command -v python &> /dev/null; then
    python -m http.server 8000 &
    FRONTEND_PID=$!
    FRONTEND_TYPE="Python"
elif command -v npx &> /dev/null; then
    npx http-server -p 8000 &
    FRONTEND_PID=$!
    FRONTEND_TYPE="Node.js"
else
    echo "Error: Neither Python nor Node.js found. Please install one of them."
    kill $BACKEND_PID 2>/dev/null
    exit 1
fi

# Wait a moment for frontend to start
sleep 1

echo "Frontend server started using $FRONTEND_TYPE (PID: $FRONTEND_PID)"
echo ""
echo "=========================================="
echo "Servers are running!"
echo "=========================================="
echo "Backend API:  http://localhost:8080"
echo "Frontend:     http://localhost:8000"
echo ""
echo "Open http://localhost:8000 in your browser"
echo ""
echo "Press Ctrl+C to stop both servers"
echo "=========================================="

# Wait for user interrupt
wait

