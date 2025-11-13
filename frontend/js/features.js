// Feature button handlers

// cy is declared in main.js, we'll reference it from window

// Wait for DOM and Cytoscape to be ready
document.addEventListener('DOMContentLoaded', () => {
    setupFeatureHandlers();
    
    // Wait for Cytoscape to initialize
    // cy will be available via window.cy from main.js
});

function setupFeatureHandlers() {
    // Mutual Friends
    document.getElementById('btnMutualFriends').addEventListener('click', () => {
        document.getElementById('mutualFriendsModal').classList.add('show');
    });

    document.getElementById('findMutualFriends').addEventListener('click', async () => {
        const user1 = parseInt(document.getElementById('user1Input').value);
        const user2 = parseInt(document.getElementById('user2Input').value);
        
        if (!user1 || !user2) {
            alert('Please enter both user IDs');
            return;
        }

        const modal = document.getElementById('mutualFriendsModal');
        modal.classList.remove('show');

        try {
            const result = await getMutualFriends(user1, user2);
            displayMutualFriends(result);
        } catch (error) {
            console.error('Error getting mutual friends:', error);
            alert('Error finding mutual friends');
        }
    });

    // Influencer Leaderboard
    document.getElementById('btnLeaderboard').addEventListener('click', async () => {
        const panel = document.getElementById('featureResultsPanel');
        const title = document.getElementById('featureResultsTitle');
        const content = document.getElementById('featureResultsContent');
        
        panel.classList.remove('hidden');
        title.textContent = 'Influencer Leaderboard';
        showLoading(content);

        try {
            const leaderboard = await getInfluencerLeaderboard(10);
            displayLeaderboard(leaderboard);
        } catch (error) {
            console.error('Error getting leaderboard:', error);
            showError(content, 'Failed to load leaderboard');
        }
    });

    // Community Detection
    document.getElementById('btnCommunities').addEventListener('click', async () => {
        const panel = document.getElementById('featureResultsPanel');
        const title = document.getElementById('featureResultsTitle');
        const content = document.getElementById('featureResultsContent');
        
        panel.classList.remove('hidden');
        title.textContent = 'Community Detection';
        showLoading(content);

        try {
            const communities = await getCommunities();
            displayCommunities(communities);
            applyCommunityColors(communities);
        } catch (error) {
            console.error('Error getting communities:', error);
            showError(content, 'Failed to detect communities');
        }
    });

    // Shortest Path
    document.getElementById('btnShortestPath').addEventListener('click', () => {
        document.getElementById('shortestPathModal').classList.add('show');
    });

    document.getElementById('findPath').addEventListener('click', async () => {
        const source = parseInt(document.getElementById('sourceInput').value);
        const target = parseInt(document.getElementById('targetInput').value);
        
        if (!source || !target) {
            alert('Please enter both source and target user IDs');
            return;
        }

        const modal = document.getElementById('shortestPathModal');
        modal.classList.remove('show');

        try {
            const result = await getShortestPath(source, target);
            displayShortestPath(result, source, target);
        } catch (error) {
            console.error('Error finding path:', error);
            alert('Error finding shortest path');
        }
    });

    // Recommendations
    document.getElementById('btnRecommendations').addEventListener('click', () => {
        document.getElementById('recommendationsModal').classList.add('show');
    });

    document.getElementById('getRecommendations').addEventListener('click', async () => {
        const userId = parseInt(document.getElementById('recommendUserInput').value);
        const count = parseInt(document.getElementById('recommendCountInput').value) || 10;
        
        if (!userId) {
            alert('Please enter a user ID');
            return;
        }

        const modal = document.getElementById('recommendationsModal');
        modal.classList.remove('show');

        const panel = document.getElementById('featureResultsPanel');
        const title = document.getElementById('featureResultsTitle');
        const content = document.getElementById('featureResultsContent');
        
        panel.classList.remove('hidden');
        title.textContent = `Friend Recommendations for User ${userId}`;
        showLoading(content);

        try {
            const recommendations = await getRecommendations(userId, count);
            displayRecommendations(recommendations);
        } catch (error) {
            console.error('Error getting recommendations:', error);
            showError(content, 'Failed to load recommendations');
        }
    });

    // Reset View
    document.getElementById('btnReset').addEventListener('click', () => {
        if (window.resetView) resetView();
    });

    // Close modals
    document.querySelectorAll('.modal-close').forEach(closeBtn => {
        closeBtn.addEventListener('click', (e) => {
            e.target.closest('.modal').classList.remove('show');
        });
    });

    // Close modals on outside click
    document.querySelectorAll('.modal').forEach(modal => {
        modal.addEventListener('click', (e) => {
            if (e.target === modal) {
                modal.classList.remove('show');
            }
        });
    });

    // Close feature results panel
    document.getElementById('closeFeatureResults').addEventListener('click', () => {
        document.getElementById('featureResultsPanel').classList.add('hidden');
    });
}

// Display mutual friends
function displayMutualFriends(result) {
    const panel = document.getElementById('featureResultsPanel');
    const title = document.getElementById('featureResultsTitle');
    const content = document.getElementById('featureResultsContent');
    
    panel.classList.remove('hidden');
    title.textContent = `Mutual Friends: User ${result.user_id_1} & User ${result.user_id_2}`;
    
    let html = `
        <div class="node-detail-item">
            <div class="node-detail-label">Similarity Ratio</div>
            <div class="node-detail-value">${(result.similarity_ratio * 100).toFixed(2)}%</div>
        </div>
        <div class="node-detail-item">
            <div class="node-detail-label">Mutual Friends Count</div>
            <div class="node-detail-value">${result.mutual_ids.length}</div>
        </div>
        <div class="node-detail-item">
            <div class="node-detail-label">User ${result.user_id_1} Friends</div>
            <div class="node-detail-value">${result.total_degree_1}</div>
        </div>
        <div class="node-detail-item">
            <div class="node-detail-label">User ${result.user_id_2} Friends</div>
            <div class="node-detail-value">${result.total_degree_2}</div>
        </div>
    `;

    if (result.mutual_ids.length > 0) {
        html += `
            <div class="node-detail-item">
                <div class="node-detail-label">Mutual Friend IDs</div>
                <div class="node-detail-value">
                    <ul class="node-detail-list">
                        ${result.mutual_ids.map(id => `<li>User ${id}</li>`).join('')}
                    </ul>
                </div>
            </div>
        `;
    }

    content.innerHTML = html;

    // Highlight nodes
    if (window.clearHighlights) clearHighlights();
    if (window.cy && window.highlightNodes) {
        highlightNodes([result.user_id_1, result.user_id_2], 'selected');
        if (result.mutual_ids.length > 0) {
            highlightNodes(result.mutual_ids, 'mutual-friend');
        }
    }
}

// Display leaderboard
function displayLeaderboard(leaderboard) {
    const content = document.getElementById('featureResultsContent');
    
    let html = '<div class="leaderboard-container">';
    
    leaderboard.forEach(entry => {
        html += `
            <div class="leaderboard-item" data-user-id="${entry.user_id}">
                <div>
                    <span class="leaderboard-rank">#${entry.rank}</span>
                    <span class="leaderboard-name">${entry.user_name}</span>
                </div>
                <div class="leaderboard-stats">
                    Influence Score: ${entry.influence_score.toFixed(2)}<br>
                    Followers: ${entry.total_followers} | 
                    Fans: ${entry.fan_count} | 
                    Friends: ${entry.friend_count}<br>
                    PageRank: ${entry.pagerank_score.toFixed(6)}
                </div>
            </div>
        `;
    });
    
    html += '</div>';
    content.innerHTML = html;

    // Add click handlers
    content.querySelectorAll('.leaderboard-item').forEach(item => {
        item.addEventListener('click', () => {
            const userId = parseInt(item.dataset.userId);
            if (window.cy) {
                const node = window.cy.getElementById(userId.toString());
                if (node.length > 0) {
                    if (window.clearHighlights) clearHighlights();
                    node.addClass('selected');
                    window.cy.center(node);
                    window.cy.fit(node, 100);
                    if (window.showNodeDetails) showNodeDetails(userId);
                }
            }
        });
    });
}

// Display communities
function displayCommunities(communities) {
    const content = document.getElementById('featureResultsContent');
    
    let html = `
        <div class="node-detail-item">
            <div class="node-detail-label">Total Communities</div>
            <div class="node-detail-value">${communities.length}</div>
        </div>
        <div class="community-legend">
            <div class="node-detail-label">Community Legend</div>
    `;

    communities.forEach(comm => {
        html += `
            <div class="community-item">
                <div class="community-color" style="background-color: ${comm.color}"></div>
                <div class="community-info">
                    <div class="node-detail-value">Community ${comm.community_id}</div>
                    <div class="community-size">${comm.size} members | Density: ${(comm.internal_edge_density * 100).toFixed(1)}%</div>
                </div>
            </div>
        `;
    });

    html += '</div></div>';
    content.innerHTML = html;
    
    // Apply community colors to graph
    if (window.cy && window.applyCommunityColors) {
        applyCommunityColors(communities);
    }
}

// Display shortest path
function displayShortestPath(result, source, target) {
    const panel = document.getElementById('featureResultsPanel');
    const title = document.getElementById('featureResultsTitle');
    const content = document.getElementById('featureResultsContent');
    
    panel.classList.remove('hidden');
    title.textContent = `Shortest Path: User ${source} → User ${target}`;
    
    if (!result.path_exists) {
        content.innerHTML = '<div style="padding: 20px; text-align: center; color: #e74c3c;">No path found between these users</div>';
        return;
    }

    let html = `
        <div class="path-display">
            <div class="node-detail-label">Path Length</div>
            <div class="node-detail-value">${result.path_length} hops</div>
            <div class="node-detail-label" style="margin-top: 15px;">Path Description</div>
            <div class="node-detail-value">${result.path_description || 'N/A'}</div>
            <div class="node-detail-label" style="margin-top: 15px;">Path Nodes</div>
            <div class="path-nodes">
    `;

    result.path_node_ids.forEach((nodeId, index) => {
        html += `
            <span class="path-node" data-user-id="${nodeId}">User ${nodeId}</span>
            ${index < result.path_node_ids.length - 1 ? '<span class="path-arrow">→</span>' : ''}
        `;
    });

    html += '</div></div>';
    content.innerHTML = html;

    // Highlight path
    if (window.clearHighlights) clearHighlights();
    if (window.cy && window.highlightNodes) {
        highlightNodes(result.path_node_ids, 'selected');
        
        // Highlight edges in path
        for (let i = 0; i < result.path_node_ids.length - 1; i++) {
            const source = result.path_node_ids[i].toString();
            const target = result.path_node_ids[i + 1].toString();
            const edge = window.cy.edges(`[source = "${source}"][target = "${target}"]`);
            if (edge.length > 0) {
                edge.addClass('path-edge');
            }
        }
    }

    // Add click handlers to path nodes
    content.querySelectorAll('.path-node').forEach(node => {
        node.addEventListener('click', () => {
            const userId = parseInt(node.dataset.userId);
            showNodeDetails(userId);
        });
    });
}

// Display recommendations
function displayRecommendations(recommendations) {
    const content = document.getElementById('featureResultsContent');
    
    if (recommendations.length === 0) {
        content.innerHTML = '<div style="padding: 20px; text-align: center;">No recommendations found</div>';
        return;
    }

    let html = '<div class="recommendations-container">';
    
    recommendations.forEach(rec => {
        html += `
            <div class="recommendation-item" data-user-id="${rec.recommended_user_id}">
                <div class="recommendation-name">${rec.user_name} (ID: ${rec.recommended_user_id})</div>
                <div class="recommendation-score">Score: ${rec.total_score.toFixed(2)}/100</div>
                <div class="recommendation-reason">${rec.recommendation_reason}</div>
                <div class="leaderboard-stats" style="margin-top: 5px;">
                    Mutual Friends: ${rec.mutual_friends_count} | 
                    Common Interests: ${rec.common_interests_count} | 
                    Distance: ${rec.geographic_distance_km.toFixed(2)} km
                </div>
            </div>
        `;
    });
    
    html += '</div>';
    content.innerHTML = html;

    // Add click handlers
    content.querySelectorAll('.recommendation-item').forEach(item => {
        item.addEventListener('click', () => {
            const userId = parseInt(item.dataset.userId);
            if (window.cy) {
                const node = window.cy.getElementById(userId.toString());
                if (node.length > 0) {
                    if (window.clearHighlights) clearHighlights();
                    node.addClass('selected');
                    window.cy.center(node);
                    window.cy.fit(node, 100);
                    if (window.showNodeDetails) showNodeDetails(userId);
                }
            }
        });
    });
}

