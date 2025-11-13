// Main Cytoscape.js initialization and core logic

let cy;
let selectedNodes = [];
let communityColors = {};

// Initialize Cytoscape
async function initializeCytoscape() {
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
        
        // Prepare elements array
        const elements = [];
        if (graphData.nodes) {
            elements.push(...graphData.nodes);
        }
        if (graphData.edges) {
            elements.push(...graphData.edges);
        }
        
        console.log('Total elements:', elements.length);
        
        // Check if container exists and has dimensions
        const container = document.getElementById('cy');
        if (!container) {
            throw new Error('Cytoscape container not found');
        }
        
        // Ensure container is visible
        container.style.width = '100%';
        container.style.height = '100%';
        container.style.minHeight = '400px';
        
        // Initialize Cytoscape
        cy = cytoscape({
            container: container,
            
            elements: elements,
            
            style: [
                {
                    selector: 'node',
                    style: {
                        'background-color': '#3498db',
                        'label': 'data(label)',
                        'width': 30,
                        'height': 30,
                        'font-size': '12px',
                        'text-valign': 'center',
                        'text-halign': 'center',
                        'text-outline-width': 2,
                        'text-outline-color': '#fff',
                        'color': '#2c3e50',
                        'overlay-padding': '6px',
                        'border-width': 2,
                        'border-color': '#fff'
                    }
                },
                {
                    selector: 'node[degree]',
                    style: {
                        'width': 'mapData(degree, 0, 100, 20, 60)',
                        'height': 'mapData(degree, 0, 100, 20, 60)'
                    }
                },
                {
                    selector: 'node.selected',
                    style: {
                        'background-color': '#e74c3c',
                        'border-width': 4,
                        'border-color': '#c0392b'
                    }
                },
                {
                    selector: 'node.highlighted',
                    style: {
                        'background-color': '#f39c12',
                        'border-width': 3,
                        'border-color': '#e67e22'
                    }
                },
                {
                    selector: 'node.mutual-friend',
                    style: {
                        'background-color': '#9b59b6',
                        'border-color': '#8e44ad'
                    }
                },
                {
                    selector: 'edge',
                    style: {
                        'width': 2,
                        'line-color': '#95a5a6',
                        'target-arrow-color': '#95a5a6',
                        'target-arrow-shape': 'triangle',
                        'curve-style': 'bezier',
                        'opacity': 0.6
                    }
                },
                {
                    selector: 'edge[relationship_type = "friend"]',
                    style: {
                        'line-color': '#3498db',
                        'target-arrow-color': '#3498db',
                        'width': 3
                    }
                },
                {
                    selector: 'edge[relationship_type = "fan"]',
                    style: {
                        'line-color': '#e74c3c',
                        'target-arrow-color': '#e74c3c',
                        'width': 2
                    }
                },
                {
                    selector: 'edge.highlighted',
                    style: {
                        'width': 4,
                        'opacity': 1,
                        'line-color': '#f39c12',
                        'target-arrow-color': '#f39c12'
                    }
                },
                {
                    selector: 'edge.path-edge',
                    style: {
                        'width': 5,
                        'opacity': 1,
                        'line-color': '#27ae60',
                        'target-arrow-color': '#27ae60'
                    }
                }
            ],
            
            layout: (() => {
                // Use simpler layout for very large graphs
                const nodeCount = graphData.nodes ? graphData.nodes.length : 0;
                if (nodeCount > 1000) {
                    console.log('Using grid layout for large graph');
                    return {
                        name: 'grid',
                        fit: true,
                        padding: 30,
                        animate: false
                    };
                } else if (nodeCount > 500) {
                    console.log('Using breadthfirst layout for medium graph');
                    return {
                        name: 'breadthfirst',
                        fit: true,
                        padding: 30,
                        animate: false,
                        directed: false
                    };
                } else {
                    console.log('Using COSE layout');
                    return {
                        name: 'cose',
                        idealEdgeLength: 100,
                        nodeOverlap: 20,
                        refresh: 20,
                        fit: true,
                        padding: 30,
                        randomize: true,
                        componentSpacing: 100,
                        nodeRepulsion: 4000,
                        edgeElasticity: 100,
                        nestingFactor: 5,
                        gravity: 0.25,
                        numIter: 1000,
                        initialTemp: 200,
                        coolingFactor: 0.95,
                        minTemp: 1.0,
                        animate: true,
                        animationDuration: 1000,
                        animationEasing: 'ease-out'
                    };
                }
            })()
        });

        // Node click handler
        cy.on('tap', 'node', async function(evt) {
            const node = evt.target;
            const nodeId = parseInt(node.id());
            
            // Clear previous selection
            cy.elements().removeClass('selected');
            node.addClass('selected');
            
            // Show node details
            if (window.showNodeDetails) {
                showNodeDetails(nodeId);
            }
        });

        // Edge click handler
        cy.on('tap', 'edge', function(evt) {
            const edge = evt.target;
            const sourceId = edge.source().id();
            const targetId = edge.target().id();
            const relationshipType = edge.data('relationship_type');
            
            // Show edge info in console or side panel
            console.log(`Edge: ${sourceId} -> ${targetId}, Type: ${relationshipType}`);
        });

        // Pan and zoom controls
        cy.userPanningEnabled(true);
        cy.boxSelectionEnabled(true);
        cy.zoomingEnabled(true);
        cy.minZoom(0.1);
        cy.maxZoom(2);

        console.log('Cytoscape initialized with', graphData.nodes.length, 'nodes and', graphData.edges.length, 'edges');
        
        // Expose cy to window for other scripts
        window.cy = cy;
        
        // Force a layout refresh
        setTimeout(() => {
            cy.fit();
            cy.center();
        }, 100);
    } catch (error) {
        console.error('Error initializing Cytoscape:', error);
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
    if (!cy) return;
    cy.elements().removeClass(className);
    nodeIds.forEach(id => {
        const node = cy.getElementById(id.toString());
        if (node.length > 0) {
            node.addClass(className);
        }
    });
}

// Highlight edges
function highlightEdges(edgeIds, className = 'highlighted') {
    if (!cy) return;
    cy.elements('edge').removeClass(className);
    edgeIds.forEach(id => {
        const edge = cy.getElementById(id);
        if (edge.length > 0) {
            edge.addClass(className);
        }
    });
}

// Apply community colors
function applyCommunityColors(communities) {
    if (!cy) return;
    communities.forEach(comm => {
        comm.member_ids.forEach(memberId => {
            const node = cy.getElementById(memberId.toString());
            if (node.length > 0) {
                node.style('background-color', comm.color);
                node.data('community', comm.community_id);
            }
        });
        communityColors[comm.community_id] = comm.color;
    });
}

// Clear all highlights
function clearHighlights() {
    if (!cy) return;
    cy.elements().removeClass('selected', 'highlighted', 'mutual-friend', 'path-edge');
    cy.elements('node').style('background-color', '#3498db');
}

// Reset view
function resetView() {
    if (!cy) return;
    clearHighlights();
    cy.fit();
    cy.center();
    document.getElementById('nodeDetailsPanel').classList.add('hidden');
    document.getElementById('featureResultsPanel').classList.add('hidden');
}

// Close node details panel
document.getElementById('closeNodeDetails').addEventListener('click', () => {
    document.getElementById('nodeDetailsPanel').classList.add('hidden');
    if (cy) {
        cy.elements().removeClass('selected');
    }
});

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    initializeCytoscape();
});

// Export functions for use in other modules
// cy is already assigned to window.cy in initializeCytoscape()
window.highlightNodes = highlightNodes;
window.highlightEdges = highlightEdges;
window.applyCommunityColors = applyCommunityColors;
window.clearHighlights = clearHighlights;
window.resetView = resetView;
window.showNodeDetails = showNodeDetails;

