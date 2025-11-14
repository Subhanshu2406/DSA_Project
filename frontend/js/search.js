// Search functionality with autocomplete

let searchTimeout;
const searchInput = document.getElementById('searchInput');
const searchResults = document.getElementById('searchResults');

// Debounced search function
function performSearch(query) {
    if (!query || query.trim().length < 1) {
        searchResults.classList.remove('show');
        return;
    }

    searchUsers(query)
        .then(results => {
            displaySearchResults(results);
        })
        .catch(error => {
            console.error('Search error:', error);
            searchResults.innerHTML = '<div class="search-result-item">Error searching users</div>';
            searchResults.classList.add('show');
        });
}

// Display search results
function displaySearchResults(results) {
    if (results.length === 0) {
        searchResults.innerHTML = '<div class="search-result-item">No users found</div>';
        searchResults.classList.add('show');
        return;
    }

    searchResults.innerHTML = results
        .map(result => `
            <div class="search-result-item" data-user-id="${result.user_id}" data-user-name="${result.name}">
                <span class="search-username">${result.name}</span>
                <span class="search-userid">(ID: ${result.user_id})</span>
            </div>
        `)
        .join('');

    // Add click handlers
    searchResults.querySelectorAll('.search-result-item').forEach(item => {
        item.addEventListener('click', () => {
            const userId = parseInt(item.dataset.userId);
            const userName = item.dataset.userName;
            selectUserFromSearch(userId, userName);
        });
    });

    searchResults.classList.add('show');
}

// Handle user selection from search
function selectUserFromSearch(userId, userName) {
    searchInput.value = userName;
    searchResults.classList.remove('show');
    
    // Highlight and center the node
    if (window.network && window.nodes) {
        const node = window.nodes.get(userId.toString());
        if (node) {
            // Clear previous selection
            if (window.clearHighlights) clearHighlights();
            
            // Select and highlight
            window.highlightNodes([userId], 'selected');
            window.network.selectNodes([userId.toString()]);
            // Show label for selected node
            if (window.nodes) {
                const node = window.nodes.get(userId.toString());
                if (node) {
                    window.nodes.update({
                        id: userId.toString(),
                        font: {
                            size: 14,
                            face: 'Arial',
                            color: '#2c3e50'
                        }
                    });
                }
            }
            window.network.focus(userId.toString(), {
                scale: 1.5,
                animation: {
                    duration: 1000,
                    easingFunction: 'easeInOutQuad'
                }
            });
            
            // Show node details
            if (window.showNodeDetails) {
                showNodeDetails(userId);
            }
        }
    }
}

// Event listeners
searchInput.addEventListener('input', (e) => {
    const query = e.target.value.trim();
    clearTimeout(searchTimeout);
    
    if (query.length >= 1) {
        searchTimeout = setTimeout(() => performSearch(query), 300);
    } else {
        searchResults.classList.remove('show');
    }
});

// Close search results when clicking outside
document.addEventListener('click', (e) => {
    if (!searchInput.contains(e.target) && !searchResults.contains(e.target)) {
        searchResults.classList.remove('show');
    }
});

// Allow Enter key to select first result
searchInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
        const firstResult = searchResults.querySelector('.search-result-item');
        if (firstResult) {
            firstResult.click();
        }
    }
});

