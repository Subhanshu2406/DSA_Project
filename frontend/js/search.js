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
                <strong>${result.name}</strong> (ID: ${result.user_id})
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
    if (window.cy) {
        const node = window.cy.getElementById(userId.toString());
        if (node.length > 0) {
            // Clear previous selection
            window.cy.elements().removeClass('selected');
            
            // Select and highlight
            node.addClass('selected');
            window.cy.center(node);
            window.cy.fit(node, 100);
            
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

