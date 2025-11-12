"""
Main graph generation logic for social media network.
"""

import networkx as nx
import random
from datetime import datetime, timedelta
from typing import Dict, Tuple, Set
import config
from dataset_generator.clustering import ClusteringManager


class GraphGenerator:
    """Generates the initial social network graph."""
    
    # Common first and last names for random name generation
    FIRST_NAMES = [
        "Alex", "Jordan", "Taylor", "Morgan", "Casey", "Riley", "Avery", "Quinn",
        "Sam", "Cameron", "Dakota", "Skylar", "Blake", "Sage", "River", "Phoenix",
        "Emma", "Liam", "Olivia", "Noah", "Ava", "Ethan", "Sophia", "Mason",
        "Isabella", "James", "Mia", "Benjamin", "Charlotte", "Lucas", "Amelia", "Henry",
        "Harper", "Alexander", "Evelyn", "Michael", "Abigail", "Daniel", "Emily", "Matthew",
        "Elizabeth", "Aiden", "Sofia", "Joseph", "Avery", "David", "Ella", "Jackson",
        "Madison", "Logan", "Scarlett", "John", "Victoria", "Luke", "Aria", "Jack",
        "Grace", "Owen", "Chloe", "Wyatt", "Penelope", "Carter", "Layla", "Julian"
    ]
    
    LAST_NAMES = [
        "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", "Davis",
        "Rodriguez", "Martinez", "Hernandez", "Lopez", "Wilson", "Anderson", "Thomas", "Taylor",
        "Moore", "Jackson", "Martin", "Lee", "Thompson", "White", "Harris", "Sanchez",
        "Clark", "Ramirez", "Lewis", "Robinson", "Walker", "Young", "Allen", "King",
        "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores", "Green", "Adams",
        "Nelson", "Baker", "Hall", "Rivera", "Campbell", "Mitchell", "Carter", "Roberts",
        "Gomez", "Phillips", "Evans", "Turner", "Diaz", "Parker", "Cruz", "Edwards"
    ]
    
    def __init__(self):
        """Initialize graph generator."""
        self.graph = nx.DiGraph()
        self.clustering = ClusteringManager(
            config.NUM_REGIONS,
            config.TOTAL_INTEREST_CATEGORIES
        )
        self.start_date = datetime.strptime(config.START_DATE, "%Y-%m-%d")
    
    def _generate_random_name(self) -> str:
        """
        Generate a random name for a user.
        
        Returns:
            Random name string (first name + last name)
        """
        first_name = random.choice(self.FIRST_NAMES)
        last_name = random.choice(self.LAST_NAMES)
        return f"{first_name} {last_name}"
    
    def generate_nodes(self) -> None:
        """Generate all nodes with attributes."""
        print(f"Generating {config.NUM_NODES} nodes...")
        
        for user_id in range(config.NUM_NODES):
            # Assign location
            lat, lon, region_id = self.clustering.assign_location()
            
            # Assign interests
            interests = self.clustering.assign_interests(
                config.MIN_INTERESTS_PER_USER,
                config.MAX_INTERESTS_PER_USER
            )
            
            # Assign creation date (staggered over time)
            days_before = random.randint(
                config.ACCOUNT_CREATION_END_DAYS_BEFORE,
                config.ACCOUNT_CREATION_START_DAYS_BEFORE
            )
            created_at = self.start_date - timedelta(days=days_before)
            
            # Generate random name
            name = self._generate_random_name()
            
            # Add node with attributes
            self.graph.add_node(
                user_id,
                user_id=user_id,
                name=name,
                location=(lat, lon),
                region_id=region_id,
                interests=list(interests),  # Convert set to list for JSON serialization
                created_at=created_at.isoformat()
            )
        
        print(f"Generated {len(self.graph.nodes())} nodes")
    
    def generate_edges(self) -> None:
        """Generate edges based on clustering and probabilities."""
        print("Generating edges...")
        
        nodes = list(self.graph.nodes())
        edges_created = 0
        
        # Generate edges with clustering
        for i, node1 in enumerate(nodes):
            if (i + 1) % 100 == 0:
                print(f"  Processed {i + 1}/{len(nodes)} nodes...")
            
            node1_data = self.graph.nodes[node1]
            region1 = node1_data['region_id']
            interests1 = set(node1_data['interests'])
            
            for node2 in nodes:
                if node1 == node2:
                    continue
                
                node2_data = self.graph.nodes[node2]
                region2 = node2_data['region_id']
                interests2 = set(node2_data['interests'])
                
                # Calculate connection probability
                prob = self.clustering.calculate_connection_probability(
                    region1, region2,
                    interests1, interests2,
                    config.BASE_CONNECTION_PROB,
                    config.GEOGRAPHIC_BOOST,
                    config.INTEREST_OVERLAP_BOOST,
                    config.MAX_INTEREST_BOOST
                )
                
                # Decide if connection should be made
                if random.random() < prob:
                    # Determine direction (some will be mutual, some one-way)
                    # Higher probability of mutual if high similarity
                    similarity = (
                        self.clustering.calculate_geographic_similarity(region1, region2) +
                        self.clustering.calculate_interest_similarity(interests1, interests2)
                    ) / 2.0
                    
                    # Create edge from node1 to node2
                    if random.random() < (0.3 + similarity * 0.4):  # 30-70% chance
                        self._add_edge(node1, node2)
                        edges_created += 1
                    
                    # Create edge from node2 to node1 (mutual follow)
                    if random.random() < (0.3 + similarity * 0.4):
                        self._add_edge(node2, node1)
                        edges_created += 1
        
        print(f"Generated {edges_created} edges")
        print(f"Total edges in graph: {len(self.graph.edges())}")
    
    def _add_edge(self, from_node: int, to_node: int) -> None:
        """
        Add an edge with initial attributes.
        
        Args:
            from_node: Source node
            to_node: Target node
        """
        if not self.graph.has_edge(from_node, to_node):
            self.graph.add_edge(
                from_node,
                to_node,
                message_count=0,
                last_interaction=None,
                established_at=self.start_date.isoformat(),
                relationship_type="fan"  # Will be updated later
            )
    
    def get_graph(self) -> nx.DiGraph:
        """Get the generated graph."""
        return self.graph
    
    def generate(self) -> nx.DiGraph:
        """
        Generate complete graph.
        
        Returns:
            Generated directed graph
        """
        self.generate_nodes()
        self.generate_edges()
        return self.graph

