// Main vis-network initialization and core logic

let network = null;
let nodes = null;
let edges = null;
let selectedNodes = [];
let communityColors = {};

// Initialize vis-network
async function initializeGraph() {
    try {
        console.log('Loading graph data...');
        // Load graph data
        const graphData = await getGraph();
        console.log('Graph data loaded:', graphData);
        console.log('Nodes count:', graphData.nodes ? graphData.nodes.length : 0);
        console.log('Edges count:', graphData.edges ? graphData.edges.length : 0);
        
        if (!graphData.nodes || graphData.nodes.length === 0) {
            throw new Error('No nodes found in graph data');
        }
        
        // Check if container exists
        const container = document.getElementById('cy');
        if (!container) {
            throw new Error('Graph container not found');
        }
        
        // Ensure container is visible
        container.style.width = '100%';
        container.style.height = '100%';
        container.style.minHeight = '400px';
        
        // Convert API format to vis-network format
        const visNodes = new vis.DataSet(
            graphData.nodes.map(node => ({
                id: node.data.id,
                label: node.data.label || node.data.name || node.data.user_id.toString(),
                user_id: node.data.user_id,
                name: node.data.name,
                degree: node.data.degree || 0,
                followers: node.data.followers || 0,
                following: node.data.following || 0,
                region_id: node.data.region_id,
                interests: node.data.interests || [],
                location: node.data.location || [],
                color: {
                    background: '#3498db',
                    border: '#2980b9',
                    highlight: {
                        background: '#e74c3c',
                        border: '#c0392b'
                    }
                },
                size: Math.max(15, Math.min(35, 15 + (node.data.degree || 0) * 0.2)),
                font: {
                    size: 0,  // Hide labels initially - show on hover/click
                    color: '#2c3e50',
                    face: 'Arial'
                },
                borderWidth: 2,
                borderWidthSelected: 4
            }))
        );
        
        const visEdges = new vis.DataSet(
            graphData.edges.map(edge => {
                const edgeColor = edge.data.relationship_type === 'friend' ? '#3498db' : '#e74c3c';
                return {
                    id: edge.data.id,
                    from: edge.data.source,
                    to: edge.data.target,
                    arrows: {
                        to: {
                            enabled: false  // Disabled to reduce clutter
                        }
                    },
                    color: {
                        color: edgeColor,
                        highlight: '#f39c12',
                        hover: '#f39c12',
                        opacity: 0.25  // Start with translucent edges
                    },
                    width: edge.data.relationship_type === 'friend' ? 2 : 1.5,
                    relationship_type: edge.data.relationship_type,
                    smooth: {
                        enabled: true,
                        type: 'discrete',
                        roundness: 0.5
                    }
                };
            })
        );
        
        nodes = visNodes;
        edges = visEdges;
        
        // Create data object
        const data = {
            nodes: visNodes,
            edges: visEdges
        };
        
        // Configure options - optimized for dense graphs
        const options = {
            nodes: {
                shape: 'dot',
                font: {
                    size: 0,  // Hide labels initially to reduce clutter
                    face: 'Arial',
                    color: '#2c3e50'
                },
                borderWidth: 2,
                shadow: true,
                size: 20
            },
            edges: {
                arrows: {
                    to: {
                        enabled: false  // Turn off arrows to reduce clutter
                    }
                },
                smooth: {
                    enabled: true,
                    type: 'discrete',
                    roundness: 0.5
                },
                shadow: false,
                width: 1.5
            },
            physics: {
                enabled: true,
                barnesHut: {
                    gravitationalConstant: -20000,  // Higher repulsion for better spacing
                    centralGravity: 0.1,
                    springLength: 200,  // Increased spring length
                    springConstant: 0.01,  // Lower spring constant
                    damping: 0.09,
                    avoidOverlap: 1  // Maximum overlap avoidance
                },
                stabilization: {
                    enabled: true,
                    iterations: 150,
                    fit: true
                }
            },
            interaction: {
                hover: true,
                tooltipDelay: 200,
                zoomView: true,
                dragView: true,
                selectConnectedEdges: true,
                hoverConnectedEdges: true
            },
            layout: {
                improvedLayout: true,
                hierarchical: {
                    enabled: false
                }
            }
        };
        
        // Initialize network
        network = new vis.Network(container, data, options);
        
        // Node click handler
        network.on('click', function(params) {
            if (params.nodes.length > 0) {
                const nodeId = params.nodes[0];
                const nodeData = visNodes.get(nodeId);
                if (nodeData && nodeData.user_id) {
                    // Show label for clicked node
                    visNodes.update({
                        id: nodeId,
                        font: {
                            size: 14,
                            face: 'Arial',
                            color: '#2c3e50'
                        }
                    });
                    showNodeDetails(nodeData.user_id);
                }
            } else if (params.edges.length > 0) {
                const edgeId = params.edges[0];
                const edgeData = visEdges.get(edgeId);
                console.log('Edge clicked:', edgeData);
            }
        });
        
        // Node hover handler - show label on hover and highlight connected edges
        network.on('hoverNode', function(params) {
            container.style.cursor = 'pointer';
            const nodeId = params.node;
            const node = visNodes.get(nodeId);
            if (node) {
                // Show label on hover
                visNodes.update({
                    id: nodeId,
                    font: {
                        size: 14,
                        face: 'Arial',
                        color: '#2c3e50'
                    }
                });
                
                // Highlight connected edges brightly
                const connectedEdges = network.getConnectedEdges(nodeId);
                const allEdges = edges.get();
                
                allEdges.forEach(edge => {
                    if (connectedEdges.includes(edge.id)) {
                        // Bright highlight for connected edges
                        edges.update({
                            id: edge.id,
                            color: {
                                color: '#f39c12',
                                highlight: '#f39c12',
                                hover: '#f39c12',
                                opacity: 1.0
                            },
                            width: 4
                        });
                    } else {
                        // Make other edges more translucent
                        const edgeColor = edge.relationship_type === 'friend' ? '#3498db' : '#e74c3c';
                        edges.update({
                            id: edge.id,
                            color: {
                                color: edgeColor,
                                highlight: '#f39c12',
                                hover: '#f39c12',
                                opacity: 0.15
                            },
                            width: edge.relationship_type === 'friend' ? 1.5 : 1
                        });
                    }
                });
            }
        });
        
        network.on('blurNode', function(params) {
            container.style.cursor = 'default';
            const nodeId = params.node;
            // Hide label when not hovering
            visNodes.update({
                id: nodeId,
                font: {
                    size: 0
                }
            });
            
            // Restore all edges to default translucent state
            const allEdges = edges.get();
            allEdges.forEach(edge => {
                const edgeColor = edge.relationship_type === 'friend' ? '#3498db' : '#e74c3c';
                edges.update({
                    id: edge.id,
                    color: {
                        color: edgeColor,
                        highlight: '#f39c12',
                        hover: '#f39c12',
                        opacity: 0.25
                    },
                    width: edge.relationship_type === 'friend' ? 2 : 1.5
                });
            });
        });
        
        // Edge hover handler - highlight edge when directly hovered
        network.on('hoverEdge', function(params) {
            const edgeId = params.edge;
            edges.update({
                id: edgeId,
                color: {
                    color: '#f39c12',
                    highlight: '#f39c12',
                    hover: '#f39c12',
                    opacity: 1.0
                },
                width: 4
            });
        });
        
        network.on('blurEdge', function(params) {
            const edgeId = params.edge;
            const edge = edges.get(edgeId);
            if (edge) {
                const edgeColor = edge.relationship_type === 'friend' ? '#3498db' : '#e74c3c';
                edges.update({
                    id: edgeId,
                    color: {
                        color: edgeColor,
                        highlight: '#f39c12',
                        hover: '#f39c12',
                        opacity: 0.25
                    },
                    width: edge.relationship_type === 'friend' ? 2 : 1.5
                });
            }
        });
        
        // Selection change handler
        network.on('select', function(params) {
            if (params.nodes.length > 0) {
                selectedNodes = params.nodes;
            }
        });
        
        // Stabilization complete handler
        network.on('stabilizationEnd', function() {
            console.log('Graph stabilization complete');
            network.fit({
                animation: {
                    duration: 1000,
                    easingFunction: 'easeInOutQuad'
                }
            });
        });
        
        // Expose network to window for other scripts
        window.network = network;
        window.nodes = nodes;
        window.edges = edges;
        
        console.log('vis-network initialized with', graphData.nodes.length, 'nodes and', graphData.edges.length, 'edges');
    } catch (error) {
        console.error('Error initializing graph:', error);
        const errorMsg = error.message || 'Unknown error';
        document.getElementById('cy').innerHTML = `
            <div style="padding: 20px; text-align: center; color: #e74c3c;">
                <h3>Error loading graph data</h3>
                <p>${errorMsg}</p>
                <p>Make sure the API server is running on http://localhost:8080</p>
                <p>Check the browser console for more details.</p>
            </div>
        `;
    }
}

// Show node details in sidebar
async function showNodeDetails(nodeId) {
    const panel = document.getElementById('nodeDetailsPanel');
    const content = document.getElementById('nodeDetailsContent');
    
    panel.classList.remove('hidden');
    showLoading(content);
    
    try {
        const nodeData = await getNodeDetails(nodeId);
        content.innerHTML = createNodeDetailHTML(nodeData);
    } catch (error) {
        showError(content, 'Failed to load node details');
    }
}

// Highlight nodes
function highlightNodes(nodeIds, className = 'highlighted') {
    if (!network || !nodes) return;
    
    const highlightColor = className === 'mutual-friend' ? '#9b59b6' : 
                          className === 'selected' ? '#e74c3c' : '#f39c12';
    
    nodeIds.forEach(id => {
        const node = nodes.get(id.toString());
        if (node) {
            nodes.update({
                id: id.toString(),
                color: {
                    background: highlightColor,
                    border: highlightColor === '#9b59b6' ? '#8e44ad' : 
                           highlightColor === '#e74c3c' ? '#c0392b' : '#e67e22',
                    highlight: {
                        background: highlightColor,
                        border: highlightColor === '#9b59b6' ? '#8e44ad' : 
                               highlightColor === '#e74c3c' ? '#c0392b' : '#e67e22'
                    }
                },
                borderWidth: 4
            });
        }
    });
}

// Highlight edges
function highlightEdges(edgeIds, className = 'highlighted') {
    if (!network || !edges) return;
    
    const highlightColor = className === 'path-edge' ? '#27ae60' : '#f39c12';
    
    edgeIds.forEach(id => {
        const edge = edges.get(id);
        if (edge) {
            edges.update({
                id: id,
                color: {
                    color: highlightColor,
                    highlight: highlightColor,
                    hover: highlightColor
                },
                width: 5
            });
        }
    });
}

// Apply community colors
function applyCommunityColors(communities) {
    if (!network || !nodes) return;
    
    communities.forEach(comm => {
        comm.member_ids.forEach(memberId => {
            const node = nodes.get(memberId.toString());
            if (node) {
                nodes.update({
                    id: memberId.toString(),
                    color: {
                        background: comm.color,
                        border: comm.color,
                        highlight: {
                            background: comm.color,
                            border: comm.color
                        }
                    },
                    font: {
                        size: 0  // Keep labels hidden for community colors
                    }
                });
            }
        });
        communityColors[comm.community_id] = comm.color;
    });
}

// Clear all highlights
function clearHighlights() {
    if (!network || !nodes || !edges) return;
    
    // Reset all nodes to default color
    const allNodes = nodes.get();
    allNodes.forEach(node => {
        nodes.update({
            id: node.id,
            color: {
                background: '#3498db',
                border: '#2980b9',
                highlight: {
                    background: '#e74c3c',
                    border: '#c0392b'
                }
            },
            borderWidth: 2,
            font: {
                size: 0  // Hide labels when clearing highlights
            }
        });
    });
    
    // Reset all edges to default translucent state
    const allEdges = edges.get();
    allEdges.forEach(edge => {
        const edgeColor = edge.relationship_type === 'friend' ? '#3498db' : '#e74c3c';
        edges.update({
            id: edge.id,
            color: {
                color: edgeColor,
                highlight: '#f39c12',
                hover: '#f39c12',
                opacity: 0.25
            },
            width: edge.relationship_type === 'friend' ? 2 : 1.5
        });
    });
    
    // Clear selection
    network.unselectAll();
}

// Reset view
function resetView() {
    if (!network) return;
    clearHighlights();
    network.fit({
        animation: {
            duration: 1000,
            easingFunction: 'easeInOutQuad'
        }
    });
    document.getElementById('nodeDetailsPanel').classList.add('hidden');
    document.getElementById('featureResultsPanel').classList.add('hidden');
}

// Close node details panel
document.getElementById('closeNodeDetails').addEventListener('click', () => {
    document.getElementById('nodeDetailsPanel').classList.add('hidden');
    if (network) {
        network.unselectAll();
    }
});

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    initializeGraph();
});

// Export functions for use in other modules
window.highlightNodes = highlightNodes;
window.highlightEdges = highlightEdges;
window.applyCommunityColors = applyCommunityColors;
window.clearHighlights = clearHighlights;
window.resetView = resetView;
window.showNodeDetails = showNodeDetails;
