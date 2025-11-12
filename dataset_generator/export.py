"""
Export module for saving graph data to JSON and CSV formats.
"""

import networkx as nx
import json
import csv
import os
from datetime import datetime
from typing import Dict, List
import config
from dataset_generator.relationship_manager import RelationshipManager


class DataExporter:
    """Handles export of graph data to various formats."""
    
    def __init__(self, output_dir: str = None):
        """
        Initialize exporter.
        
        Args:
            output_dir: Output directory for generated files
        """
        self.output_dir = output_dir or config.OUTPUT_DIR
        os.makedirs(self.output_dir, exist_ok=True)
    
    def export_daily_snapshot(
        self,
        graph: nx.DiGraph,
        date: datetime,
        snapshot_dir: str = None
    ) -> Dict[str, str]:
        """
        Export daily snapshot of graph.
        
        Args:
            graph: Graph to export
            date: Date of snapshot
            snapshot_dir: Subdirectory for this snapshot (defaults to date)
            
        Returns:
            Dictionary with paths to exported files
        """
        if snapshot_dir is None:
            snapshot_dir = date.strftime("%Y-%m-%d")
        
        snapshot_path = os.path.join(self.output_dir, snapshot_dir)
        os.makedirs(snapshot_path, exist_ok=True)
        
        exported_files = {}
        
        # Export nodes
        nodes_data = []
        for node_id, node_data in graph.nodes(data=True):
            node_export = {
                'user_id': node_id,
                'location': node_data.get('location', (0, 0)),
                'region_id': node_data.get('region_id', 0),
                'interests': node_data.get('interests', []),
                'created_at': node_data.get('created_at', '')
            }
            nodes_data.append(node_export)
        
        nodes_file = os.path.join(snapshot_path, 'nodes.json')
        with open(nodes_file, 'w') as f:
            json.dump(nodes_data, f, indent=2)
        exported_files['nodes'] = nodes_file
        
        # Export edges
        edges_data = []
        for source, target, edge_data in graph.edges(data=True):
            edge_export = {
                'source': source,
                'target': target,
                'relationship_type': edge_data.get('relationship_type', 'fan'),
                'message_count': edge_data.get('message_count', 0),
                'last_interaction': edge_data.get('last_interaction'),
                'distance': edge_data.get('distance', 0.0),
                'established_at': edge_data.get('established_at', '')
            }
            edges_data.append(edge_export)
        
        edges_file = os.path.join(snapshot_path, 'edges.json')
        with open(edges_file, 'w') as f:
            json.dump(edges_data, f, indent=2)
        exported_files['edges'] = edges_file
        
        # Export metadata
        relationship_manager = RelationshipManager(graph)
        friend_pairs = set()
        fan_count = 0
        
        # Count unique friend relationships (each pair counted once)
        # and fan relationships (one-way edges)
        for edge in graph.edges():
            node1, node2 = edge
            rel_type = relationship_manager.get_relationship_type(node1, node2)
            if rel_type == "friend":
                # Store as tuple with smaller ID first to avoid duplicates
                pair = tuple(sorted([node1, node2]))
                friend_pairs.add(pair)
            elif rel_type == "fan":
                fan_count += 1
        
        friend_count = len(friend_pairs)
        
        metadata = {
            'date': date.isoformat(),
            'total_nodes': graph.number_of_nodes(),
            'total_edges': graph.number_of_edges(),
            'friend_relationships': friend_count,
            'fan_relationships': fan_count,
            'average_degree': sum(dict(graph.degree()).values()) / graph.number_of_nodes() if graph.number_of_nodes() > 0 else 0
        }
        
        metadata_file = os.path.join(snapshot_path, 'metadata.json')
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        exported_files['metadata'] = metadata_file
        
        return exported_files
    
    def export_aggregated_csv(
        self,
        all_nodes: List[Dict],
        all_edges: List[Dict],
        nodes_file: str = None,
        edges_file: str = None
    ) -> Dict[str, str]:
        """
        Export aggregated data to CSV files.
        
        Args:
            all_nodes: List of all node dictionaries across all days
            all_edges: List of all edge dictionaries across all days
            nodes_file: Output file for nodes CSV
            edges_file: Output file for edges CSV
            
        Returns:
            Dictionary with paths to exported files
        """
        exported_files = {}
        
        # Export nodes CSV
        if nodes_file is None:
            nodes_file = os.path.join(self.output_dir, 'nodes.csv')
        
        if all_nodes:
            with open(nodes_file, 'w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=all_nodes[0].keys())
                writer.writeheader()
                writer.writerows(all_nodes)
            exported_files['nodes_csv'] = nodes_file
        
        # Export edges CSV
        if edges_file is None:
            edges_file = os.path.join(self.output_dir, 'edges_daily.csv')
        
        if all_edges:
            with open(edges_file, 'w', newline='') as f:
                writer = csv.DictWriter(f, fieldnames=all_edges[0].keys())
                writer.writeheader()
                writer.writerows(all_edges)
            exported_files['edges_csv'] = edges_file
        
        return exported_files

