"""
Manages friend/fan relationships and distance calculations.
"""

import networkx as nx
from typing import Dict, Tuple, Set
import config


class RelationshipManager:
    """Manages relationship types and distance calculations."""
    
    def __init__(self, graph: nx.DiGraph):
        """
        Initialize relationship manager.
        
        Args:
            graph: Directed graph representing the social network
        """
        self.graph = graph
    
    def get_relationship_type(self, node1: int, node2: int) -> str:
        """
        Determine relationship type between two nodes.
        
        Args:
            node1: First node ID
            node2: Second node ID
            
        Returns:
            "friend" if mutual, "fan" if one-way, None if no connection
        """
        has_edge_1_to_2 = self.graph.has_edge(node1, node2)
        has_edge_2_to_1 = self.graph.has_edge(node2, node1)
        
        if has_edge_1_to_2 and has_edge_2_to_1:
            return "friend"
        elif has_edge_1_to_2 or has_edge_2_to_1:
            return "fan"
        else:
            return None
    
    def get_mutual_friends(self, node1: int, node2: int) -> Set[int]:
        """
        Find mutual friends between two nodes.
        
        Args:
            node1: First node ID
            node2: Second node ID
            
        Returns:
            Set of mutual friend node IDs
        """
        # Get followers of node1 (who node1 follows)
        followers_1 = set(self.graph.successors(node1))
        
        # Get followers of node2 (who node2 follows)
        followers_2 = set(self.graph.successors(node2))
        
        # Mutual friends: nodes that both node1 and node2 follow
        # AND who follow back both node1 and node2
        mutual = set()
        for node in followers_1 & followers_2:
            if (self.graph.has_edge(node, node1) and 
                self.graph.has_edge(node, node2)):
                mutual.add(node)
        
        return mutual
    
    def calculate_distance(
        self,
        node1: int,
        node2: int,
        relationship_type: str
    ) -> float:
        """
        Calculate distance between two nodes.
        
        Args:
            node1: First node ID
            node2: Second node ID
            relationship_type: "friend" or "fan"
            
        Returns:
            Distance value (lower = closer)
        """
        # Get base distance based on relationship type
        if relationship_type == "friend":
            base_distance = config.FRIEND_BASE_DISTANCE
        elif relationship_type == "fan":
            base_distance = config.FAN_BASE_DISTANCE
        else:
            return float('inf')  # No connection
        
        # Count mutual friends
        mutual_friends = self.get_mutual_friends(node1, node2)
        mutual_friend_count = len(mutual_friends)
        
        # Get message count (normalized)
        edge_data = None
        if self.graph.has_edge(node1, node2):
            edge_data = self.graph[node1][node2]
        elif self.graph.has_edge(node2, node1):
            edge_data = self.graph[node2][node1]
        
        message_count = edge_data.get('message_count', 0) if edge_data else 0
        # Normalize message count (assuming max ~1000 messages)
        message_freq = min(message_count / 1000.0, 1.0)
        
        # Calculate distance
        distance = (base_distance - 
                   (mutual_friend_count * config.MUTUAL_FRIEND_WEIGHT) -
                   (message_freq * config.MESSAGE_FREQ_WEIGHT * 10))
        
        # Ensure minimum distance
        return max(0.1, distance)
    
    def update_all_distances(self):
        """Update distance attribute for all edges in the graph."""
        for edge in self.graph.edges():
            node1, node2 = edge
            relationship_type = self.get_relationship_type(node1, node2)
            
            if relationship_type:
                distance = self.calculate_distance(node1, node2, relationship_type)
                
                # Update both directions if they exist
                if self.graph.has_edge(node1, node2):
                    self.graph[node1][node2]['distance'] = distance
                if self.graph.has_edge(node2, node1):
                    self.graph[node2][node1]['distance'] = distance
    
    def update_relationship_types(self):
        """Update relationship_type attribute for all edges."""
        for edge in list(self.graph.edges()):
            node1, node2 = edge
            relationship_type = self.get_relationship_type(node1, node2)
            
            if relationship_type:
                self.graph[node1][node2]['relationship_type'] = relationship_type

