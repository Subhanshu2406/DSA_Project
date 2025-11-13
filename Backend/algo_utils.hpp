// include/algo_utils.hpp
/*
 * ALGORITHM UTILITY FUNCTIONS
 * ============================
 * Helper functions used by graph algorithms.
 * Separated for modularity and reusability.
 */

#pragma once

#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

// ============================================================================
// UTILITY FUNCTIONS - Helper functions for common operations
// ============================================================================

namespace AlgoUtils {
    /**
     * Calculate geographic distance between two points using Haversine formula.
     * Input: latitude and longitude in degrees
     * Output: distance in kilometers
     */
    inline double calculate_haversine_distance(
        double lat1, double lon1, 
        double lat2, double lon2) 
    {
        const double EARTH_RADIUS_KM = 6371.0;
        const double PI = 3.14159265358979323846;
        const double PI_OVER_180 = PI / 180.0;
        
        double lat_diff = (lat2 - lat1) * PI_OVER_180;
        double lon_diff = (lon2 - lon1) * PI_OVER_180;
        
        double a = sin(lat_diff / 2) * sin(lat_diff / 2) +
                   cos(lat1 * PI_OVER_180) * cos(lat2 * PI_OVER_180) *
                   sin(lon_diff / 2) * sin(lon_diff / 2);
        
        double c = 2.0 * asin(sqrt(a));
        return EARTH_RADIUS_KM * c;
    }

    /**
     * Find intersection of two sets (common elements).
     * Used for finding mutual friends, common interests, etc.
     */
    inline set<int> set_intersection_of_two(const set<int>& set_a, const set<int>& set_b) {
        set<int> result;
        set_intersection(set_a.begin(), set_a.end(), 
                        set_b.begin(), set_b.end(),
                        inserter(result, result.begin()));
        return result;
    }

    /**
     * Calculate Jaccard similarity between two sets.
     * Formula: intersection_size / union_size
     * Result: 0.0 (completely different) to 1.0 (identical)
     */
    inline double jaccard_similarity(const set<int>& set_a, const set<int>& set_b) {
        if (set_a.empty() && set_b.empty()) return 1.0;
        
        set<int> intersection = set_intersection_of_two(set_a, set_b);
        int union_size = set_a.size() + set_b.size() - intersection.size();
        
        return union_size == 0 ? 0.0 : (double)intersection.size() / union_size;
    }

    /**
     * Find common items between two vectors.
     * Used for finding common interests, etc.
     */
    inline vector<string> find_common_items(
        const vector<string>& list_a, 
        const vector<string>& list_b) 
    {
        vector<string> result;
        for (const auto& item : list_a) {
            if (find(list_b.begin(), list_b.end(), item) != list_b.end()) {
                result.push_back(item);
            }
        }
        return result;
    }

    /**
     * Normalize a value to 0.0-1.0 range.
     * Used for scoring metrics with different scales.
     */
    inline double normalize_to_01(double value, double max_value) {
        if (max_value <= 0) return 0.0;
        double normalized = value / max_value;
        return min(1.0, max(0.0, normalized));
    }
}
