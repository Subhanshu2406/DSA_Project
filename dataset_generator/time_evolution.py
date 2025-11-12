"""
Time evolution module for daily snapshots and attribute updates.
"""

import networkx as nx
import random
from datetime import datetime, timedelta
from typing import List, Dict
import config
from dataset_generator.relationship_manager import RelationshipManager


class TimeEvolution:
    """Manages temporal evolution of the graph."""
    
    def __init__(self, graph: nx.DiGraph, start_date: datetime):
        """
        Initialize time evolution manager.
        
        Args:
            graph: Initial graph state
            start_date: Start date for evolution
        """
        self.graph = graph.copy()
        self.start_date = start_date
        self.current_date = start_date
        self.relationship_manager = RelationshipManager(self.graph)
        self.viral_nodes = self._select_viral_nodes()
    
    def _select_viral_nodes(self) -> List[int]:
        """Select nodes that can go viral."""
        nodes = list(self.graph.nodes())
        # Select nodes with high in-degree (already popular)
        in_degrees = dict(self.graph.in_degree())
        sorted_nodes = sorted(nodes, key=lambda n: in_degrees.get(n, 0), reverse=True)
        return sorted_nodes[:config.VIRAL_NODE_COUNT]
    
    def evolve_one_day(self) -> None:
        """Evolve graph state by one day."""
        self.current_date += timedelta(days=1)
        
        # Update message counts
        self._update_message_counts()
        
        # Update relationships
        self._update_relationships()
        
        # Update popular node dynamics
        self._update_popular_nodes()
        
        # Update relationship types and distances
        self.relationship_manager.update_relationship_types()
        self.relationship_manager.update_all_distances()
    
    def _update_message_counts(self) -> None:
        """Increment message counts for active edges."""
        for edge in list(self.graph.edges()):
            if random.random() < config.DAILY_MESSAGE_INCREMENT_PROB:
                message_increment = random.randint(
                    config.MIN_MESSAGES_PER_DAY,
                    config.MAX_MESSAGES_PER_DAY
                )
                self.graph[edge[0]][edge[1]]['message_count'] += message_increment
                self.graph[edge[0]][edge[1]]['last_interaction'] = self.current_date.isoformat()
    
    def _update_relationships(self) -> None:
        """Update relationships (friend ↔ fan, new connections, broken connections)."""
        edges_to_remove = []
        edges_to_add = []
        
        # Check existing edges for relationship changes
        for edge in list(self.graph.edges()):
            node1, node2 = edge
            relationship_type = self.relationship_manager.get_relationship_type(node1, node2)
            
            if relationship_type == "friend":
                # Friend → Fan (one person unfollows)
                if random.random() < config.FRIEND_TO_FAN_PROB:
                    # Randomly choose which direction to remove
                    if random.random() < 0.5:
                        edges_to_remove.append((node1, node2))
                    else:
                        edges_to_remove.append((node2, node1))
            
            elif relationship_type == "fan":
                # Fan → Friend (mutual follow established)
                if random.random() < config.FAN_TO_FRIEND_PROB:
                    # Add reverse edge if it doesn't exist
                    if not self.graph.has_edge(node2, node1):
                        edges_to_add.append((node2, node1))
            
            # Break connection
            if random.random() < config.BREAK_CONNECTION_PROB:
                edges_to_remove.append((node1, node2))
        
        # Remove broken connections
        for edge in edges_to_remove:
            if self.graph.has_edge(edge[0], edge[1]):
                self.graph.remove_edge(edge[0], edge[1])
        
        # Add new connections
        nodes = list(self.graph.nodes())
        for _ in range(int(len(nodes) * config.NEW_CONNECTION_PROB)):
            node1, node2 = random.sample(nodes, 2)
            if not self.graph.has_edge(node1, node2):
                self._add_new_edge(node1, node2)
                edges_to_add.append((node1, node2))
        
        # Add new edges
        for edge in edges_to_add:
            if not self.graph.has_edge(edge[0], edge[1]):
                self._add_new_edge(edge[0], edge[1])
    
    def _add_new_edge(self, from_node: int, to_node: int) -> None:
        """Add a new edge with initial attributes."""
        self.graph.add_edge(
            from_node,
            to_node,
            message_count=0,
            last_interaction=None,
            established_at=self.current_date.isoformat(),
            relationship_type="fan"
        )
    
    def _update_popular_nodes(self) -> None:
        """Update fan counts for popular nodes."""
        nodes = list(self.graph.nodes())
        
        for node in nodes:
            is_viral = node in self.viral_nodes
            
            # Gain fans
            gain_prob = config.VIRAL_GAIN_FANS_PROB if is_viral else config.NORMAL_GAIN_FANS_PROB
            if random.random() < gain_prob:
                # Find potential new fans (nodes not already following)
                potential_fans = [
                    n for n in nodes 
                    if n != node and not self.graph.has_edge(n, node)
                ]
                if potential_fans:
                    new_fan = random.choice(potential_fans)
                    self._add_new_edge(new_fan, node)
            
            # Lose fans
            lose_prob = config.VIRAL_LOSE_FANS_PROB if is_viral else config.NORMAL_LOSE_FANS_PROB
            if random.random() < lose_prob:
                # Find current fans (nodes following this node)
                current_fans = [n for n in self.graph.predecessors(node)]
                if current_fans:
                    lost_fan = random.choice(current_fans)
                    if self.graph.has_edge(lost_fan, node):
                        # Only remove if it's a fan relationship (not mutual)
                        relationship = self.relationship_manager.get_relationship_type(lost_fan, node)
                        if relationship == "fan":
                            self.graph.remove_edge(lost_fan, node)
    
    def get_current_graph(self) -> nx.DiGraph:
        """Get current graph state."""
        return self.graph
    
    def get_current_date(self) -> datetime:
        """Get current date."""
        return self.current_date

