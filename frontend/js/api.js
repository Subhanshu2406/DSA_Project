// API client functions

const API_BASE_URL = 'http://localhost:8080';

function appendDate(endpoint, includeDate = true) {
    if (!includeDate) return endpoint;
    const date = window.currentDatasetDate;
    if (!date) return endpoint;
    const separator = endpoint.includes('?') ? '&' : '?';
    return `${endpoint}${separator}date=${encodeURIComponent(date)}`;
}

async function fetchAPI(endpoint, options = {}, includeDate = true) {
    try {
        const url = `${API_BASE_URL}${appendDate(endpoint, includeDate)}`;
        const response = await fetch(url, {
            ...options,
            headers: {
                'Content-Type': 'application/json',
                ...options.headers
            }
        });

        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        return await response.json();
    } catch (error) {
        console.error('API Error:', error);
        throw error;
    }
}

// Get dataset dates
async function getAvailableDates() {
    return await fetchAPI('/api/dates', {}, false);
}

// Get full graph data
async function getGraph() {
    return await fetchAPI('/api/graph');
}

// Get node details
async function getNodeDetails(nodeId) {
    return await fetchAPI(`/api/node/${nodeId}`);
}

// Search users
async function searchUsers(query) {
    if (!query || query.trim().length === 0) {
        return [];
    }
    return await fetchAPI(`/api/search?q=${encodeURIComponent(query)}`);
}

// Get mutual friends
async function getMutualFriends(user1, user2) {
    return await fetchAPI('/api/mutual-friends', {
        method: 'POST',
        body: JSON.stringify({ user1, user2 })
    });
}

// Get influencer leaderboard
async function getInfluencerLeaderboard(top = 10) {
    return await fetchAPI(`/api/influencer-leaderboard?top=${top}`);
}

// Get communities
async function getCommunities() {
    return await fetchAPI('/api/communities');
}

// Get shortest path
async function getShortestPath(source, target) {
    return await fetchAPI(`/api/path?source=${source}&target=${target}`);
}

// Get recommendations
async function getRecommendations(userId, count = 10) {
    return await fetchAPI(`/api/recommendations?user=${userId}&count=${count}`);
}

// Export functions
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        getAvailableDates,
        getGraph,
        getNodeDetails,
        searchUsers,
        getMutualFriends,
        getInfluencerLeaderboard,
        getCommunities,
        getShortestPath,
        getRecommendations
    };
}

