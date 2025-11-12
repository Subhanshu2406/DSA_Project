"""
Geographic and interest-based clustering for realistic social network generation.
"""

import random
import math
from typing import List, Set, Tuple


class ClusteringManager:
    """Manages geographic and interest-based clustering."""
    
    def __init__(self, num_regions: int, total_interests: int):
        """
        Initialize clustering manager.
        
        Args:
            num_regions: Number of geographic regions
            total_interests: Total number of interest categories
        """
        self.num_regions = num_regions
        self.total_interests = total_interests
        self.interest_pool = [f"interest_{i}" for i in range(total_interests)]
        
    def assign_location(self) -> Tuple[float, float, int]:
        """
        Assign geographic location to a user.
        
        Returns:
            Tuple of (latitude, longitude, region_id)
        """
        region_id = random.randint(0, self.num_regions - 1)
        
        # Create clustered regions (each region has a center)
        region_centers = [
            (random.uniform(-90, 90), random.uniform(-180, 180))
            for _ in range(self.num_regions)
        ]
        
        center_lat, center_lon = region_centers[region_id]
        
        # Add some noise around center (clustering effect)
        lat = center_lat + random.gauss(0, 10)  # ~10 degree spread
        lon = center_lon + random.gauss(0, 10)
        
        # Clamp to valid ranges
        lat = max(-90, min(90, lat))
        lon = max(-180, min(180, lon))
        
        return (lat, lon, region_id)
    
    def assign_interests(self, min_interests: int, max_interests: int) -> Set[str]:
        """
        Assign interests to a user.
        
        Args:
            min_interests: Minimum number of interests
            max_interests: Maximum number of interests
            
        Returns:
            Set of interest strings
        """
        num_interests = random.randint(min_interests, max_interests)
        interests = set(random.sample(self.interest_pool, num_interests))
        return interests
    
    def calculate_geographic_similarity(self, region1: int, region2: int) -> float:
        """
        Calculate geographic similarity boost.
        
        Args:
            region1: Region ID of first user
            region2: Region ID of second user
            
        Returns:
            Similarity boost (0.0 to 1.0)
        """
        if region1 == region2:
            return 1.0
        return 0.0
    
    def calculate_interest_similarity(self, interests1: Set[str], interests2: Set[str]) -> float:
        """
        Calculate interest overlap similarity.
        
        Args:
            interests1: Interests of first user
            interests2: Interests of second user
            
        Returns:
            Similarity score (0.0 to 1.0)
        """
        if not interests1 or not interests2:
            return 0.0
        
        intersection = len(interests1 & interests2)
        union = len(interests1 | interests2)
        
        if union == 0:
            return 0.0
        
        # Jaccard similarity
        return intersection / union
    
    def calculate_connection_probability(
        self,
        region1: int,
        region2: int,
        interests1: Set[str],
        interests2: Set[str],
        base_prob: float,
        geo_boost: float,
        interest_boost: float,
        max_interest_boost: float
    ) -> float:
        """
        Calculate probability of connection between two users.
        
        Args:
            region1: Region of first user
            region2: Region of second user
            interests1: Interests of first user
            interests2: Interests of second user
            base_prob: Base connection probability
            geo_boost: Geographic boost value
            interest_boost: Interest boost per shared interest
            max_interest_boost: Maximum boost from interests
            
        Returns:
            Connection probability (0.0 to 1.0)
        """
        prob = base_prob
        
        # Geographic boost
        if self.calculate_geographic_similarity(region1, region2) > 0:
            prob += geo_boost
        
        # Interest boost
        interest_sim = self.calculate_interest_similarity(interests1, interests2)
        interest_boost_amount = min(interest_sim * interest_boost * 10, max_interest_boost)
        prob += interest_boost_amount
        
        # Clamp to [0, 1]
        return min(1.0, max(0.0, prob))

