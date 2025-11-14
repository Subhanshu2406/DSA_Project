// Utility functions

// API_BASE_URL is defined in api.js

// Interest mapping: Convert placeholder interests to real names
const INTEREST_MAPPING = {
    'interest_0': 'Technology',
    'interest_1': 'Music',
    'interest_2': 'Sports',
    'interest_3': 'Travel',
    'interest_4': 'Food & Cooking',
    'interest_5': 'Photography',
    'interest_6': 'Fitness',
    'interest_7': 'Art & Design',
    'interest_8': 'Gaming',
    'interest_9': 'Movies & TV',
    'interest_10': 'Reading',
    'interest_11': 'Fashion',
    'interest_12': 'Nature & Outdoors',
    'interest_13': 'Science',
    'interest_14': 'Business',
    'interest_15': 'Education',
    'interest_16': 'Health & Wellness',
    'interest_17': 'Comedy & Humor',
    'interest_18': 'Politics',
    'interest_19': 'DIY & Crafts'
};

// Convert interest placeholder to real name
function getInterestName(interest) {
    if (typeof interest !== 'string') return interest;
    return INTEREST_MAPPING[interest] || interest;
}

// Convert array of interests to real names
function mapInterests(interests) {
    if (!Array.isArray(interests)) return interests;
    return interests.map(getInterestName);
}

// Debounce function for search
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// Format number with commas
function formatNumber(num) {
    return num.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

// Format date string
function formatDate(dateString) {
    if (!dateString) return 'N/A';
    try {
        const date = new Date(dateString);
        return date.toLocaleDateString();
    } catch (e) {
        return dateString;
    }
}

// Show loading indicator
function showLoading(element) {
    element.innerHTML = '<div class="loading">Loading</div>';
}

// Show error message
function showError(element, message) {
    element.innerHTML = `<div style="color: #e74c3c; padding: 20px; text-align: center;">Error: ${message}</div>`;
}

// Create node detail HTML
function createNodeDetailHTML(node) {
    let html = `
        <div class="node-detail-item">
            <div class="node-detail-label">User ID</div>
            <div class="node-detail-value">${node.user_id}</div>
        </div>
        <div class="node-detail-item">
            <div class="node-detail-label">Name</div>
            <div class="node-detail-value">${node.name || 'N/A'}</div>
        </div>
        <div class="node-detail-item">
            <div class="node-detail-label">Connections</div>
            <div class="node-detail-value">
                Friends: ${node.friend_count || 0}<br>
                Followers: ${node.follower_count || 0}<br>
                Following: ${node.following_count || 0}
            </div>
        </div>
    `;

    if (node.location && node.location.length === 2) {
        html += `
            <div class="node-detail-item">
                <div class="node-detail-label">Location</div>
                <div class="node-detail-value">
                    Lat: ${node.location[0].toFixed(4)}<br>
                    Lon: ${node.location[1].toFixed(4)}
                </div>
            </div>
        `;
    }

    if (node.interests && node.interests.length > 0) {
        const mappedInterests = mapInterests(node.interests);
        html += `
            <div class="node-detail-item">
                <div class="node-detail-label">Interests</div>
                <div class="node-detail-value">
                    <ul class="node-detail-list">
                        ${mappedInterests.map(interest => `<li>${interest}</li>`).join('')}
                    </ul>
                </div>
            </div>
        `;
    }

    if (node.centrality) {
        html += `
            <div class="node-detail-item">
                <div class="node-detail-label">Centrality Metrics</div>
                <div class="node-detail-value">
                    Degree: ${(node.centrality.degree_centrality * 100).toFixed(2)}%<br>
                    Closeness: ${(node.centrality.closeness_centrality * 100).toFixed(2)}%<br>
                    Clustering: ${(node.centrality.clustering_coefficient * 100).toFixed(2)}%
                </div>
            </div>
        `;
    }

    if (node.created_at) {
        html += `
            <div class="node-detail-item">
                <div class="node-detail-label">Created</div>
                <div class="node-detail-value">${formatDate(node.created_at)}</div>
            </div>
        `;
    }

    return html;
}

// Generate color from string (for consistent community colors)
function stringToColor(str) {
    let hash = 0;
    for (let i = 0; i < str.length; i++) {
        hash = str.charCodeAt(i) + ((hash << 5) - hash);
    }
    const hue = hash % 360;
    return `hsl(${hue}, 70%, 50%)`;
}

// Export functions
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        debounce,
        formatNumber,
        formatDate,
        showLoading,
        showError,
        createNodeDetailHTML,
        stringToColor,
        getInterestName,
        mapInterests
    };
}

// Make functions globally available
window.getInterestName = getInterestName;
window.mapInterests = mapInterests;

