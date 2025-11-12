"""
Main entry point for generating social media dataset.
"""

import os
from datetime import datetime, timedelta
import config
from dataset_generator.graph_generator import GraphGenerator
from dataset_generator.time_evolution import TimeEvolution
from dataset_generator.export import DataExporter


def main():
    """Generate the complete dataset."""
    print("=" * 60)
    print("Social Media Dataset Generator")
    print("=" * 60)
    print(f"Configuration:")
    print(f"  Nodes: {config.NUM_NODES}")
    print(f"  Days: {config.NUM_DAYS}")
    print(f"  Start Date: {config.START_DATE}")
    print(f"  Output Directory: {config.OUTPUT_DIR}")
    print("=" * 60)
    
    # Step 1: Generate initial graph
    print("\n[Step 1] Generating initial graph...")
    generator = GraphGenerator()
    graph = generator.generate()
    start_date = datetime.strptime(config.START_DATE, "%Y-%m-%d")
    
    print(f"\nInitial graph statistics:")
    print(f"  Nodes: {graph.number_of_nodes()}")
    print(f"  Edges: {graph.number_of_edges()}")
    
    # Step 2: Initialize time evolution
    print("\n[Step 2] Initializing time evolution...")
    evolution = TimeEvolution(graph, start_date)
    
    # Step 3: Initialize exporter
    exporter = DataExporter()
    
    # Step 4: Generate daily snapshots
    print(f"\n[Step 3] Generating {config.NUM_DAYS} daily snapshots...")
    all_nodes_data = []
    all_edges_data = []
    
    for day in range(config.NUM_DAYS):
        if (day + 1) % 10 == 0 or day == 0:
            print(f"  Day {day + 1}/{config.NUM_DAYS} ({evolution.get_current_date().strftime('%Y-%m-%d')})...")
        
        # Export current snapshot
        current_graph = evolution.get_current_graph()
        current_date = evolution.get_current_date()
        
        if config.EXPORT_JSON:
            exported = exporter.export_daily_snapshot(current_graph, current_date)
            if day == 0:
                print(f"    Exported to: {exported['nodes']}")
        
        # Collect data for aggregated CSV
        if config.EXPORT_CSV:
            for node_id, node_data in current_graph.nodes(data=True):
                all_nodes_data.append({
                    'user_id': node_id,
                    'name': node_data.get('name', ''),
                    'date': current_date.isoformat(),
                    'location_lat': node_data.get('location', (0, 0))[0],
                    'location_lon': node_data.get('location', (0, 0))[1],
                    'region_id': node_data.get('region_id', 0),
                    'interests': ','.join(node_data.get('interests', [])),
                    'created_at': node_data.get('created_at', '')
                })
            
            for source, target, edge_data in current_graph.edges(data=True):
                all_edges_data.append({
                    'date': current_date.isoformat(),
                    'source': source,
                    'target': target,
                    'relationship_type': edge_data.get('relationship_type', 'fan'),
                    'message_count': edge_data.get('message_count', 0),
                    'last_interaction': edge_data.get('last_interaction', ''),
                    'distance': edge_data.get('distance', 0.0),
                    'established_at': edge_data.get('established_at', '')
                })
        
        # Evolve to next day (except on last iteration)
        if day < config.NUM_DAYS - 1:
            evolution.evolve_one_day()
    
    # Step 5: Export aggregated CSV files
    if config.EXPORT_CSV:
        print("\n[Step 4] Exporting aggregated CSV files...")
        exporter.export_aggregated_csv(all_nodes_data, all_edges_data)
        print(f"    Exported nodes.csv and edges_daily.csv to {config.OUTPUT_DIR}")
    
    print("\n" + "=" * 60)
    print("Dataset generation complete!")
    print("=" * 60)
    print(f"\nOutput files are in: {os.path.abspath(config.OUTPUT_DIR)}")
    print(f"\nDaily snapshots: {config.NUM_DAYS} days")
    if config.EXPORT_JSON:
        print(f"  - JSON format: {config.OUTPUT_DIR}/YYYY-MM-DD/nodes.json, edges.json, metadata.json")
    if config.EXPORT_CSV:
        print(f"  - CSV format: {config.OUTPUT_DIR}/nodes.csv, edges_daily.csv")


if __name__ == "__main__":
    main()

