import networkx as nx
import pandas as pd
import numpy as np
import json
import random

class SocialNetworkGenerator:
    def __init__(self, num_users=1000, seed=42):
        """
        Initialize the social network generator
        
        Args:
            num_users: Number of users in the network
            seed: Random seed for reproducibility
        """
        self.num_users = num_users
        random.seed(seed)
        np.random.seed(seed)
        self.G = nx.Graph()
        
    def generate_user_attributes(self):
        """Generate user attributes with Sr no. and Name only"""
        first_names = ['Alex', 'Sam', 'Jordan', 'Taylor', 'Morgan', 'Casey', 
                      'Riley', 'Avery', 'Quinn', 'Reese', 'Jamie', 'Parker',
                      'Skyler', 'Charlie', 'Drew', 'Elliot', 'Finley', 'Harper',
                      'Blake', 'Cameron', 'Dakota', 'Emerson', 'Francis', 'Gray']
        last_names = ['Smith', 'Johnson', 'Williams', 'Brown', 'Jones', 'Garcia',
                     'Miller', 'Davis', 'Rodriguez', 'Martinez', 'Hernandez',
                     'Lopez', 'Wilson', 'Anderson', 'Thomas', 'Taylor', 'Moore',
                     'Jackson', 'Martin', 'Lee', 'Perez', 'Thompson', 'White']
        
        users = []
        for i in range(1, self.num_users + 1):
            user = {
                'sr_no': i,
                'user_id': f'USER_{i:05d}',
                'name': f"{random.choice(first_names)} {random.choice(last_names)}",
                'followers': random.randint(0, 10000),
                'following': random.randint(0, 5000)
            }
            users.append(user)
        
        return pd.DataFrame(users)
    
    def generate_edges(self, users_df, connection_probability=0.05):
        """
        Generate edges between users with attributes
        
        Args:
            users_df: DataFrame containing user information
            connection_probability: Base probability of connection between users
        """
        edges = []
        
        # Create connections between users
        for i in range(len(users_df)):
            for j in range(i + 1, len(users_df)):
                user1 = users_df.iloc[i]
                user2 = users_df.iloc[j]
                
                # Create edge based on probability
                if random.random() < connection_probability:
                    edge = {
                        'source': user1['user_id'],
                        'target': user2['user_id'],
                        'num_messages': random.randint(1, 1000),
                        'num_mutuals': 0,  # Will calculate later
                        'interaction_frequency': random.choice(['daily', 'weekly', 'monthly', 'rarely']),
                        'relationship_type': random.choice(['friend', 'colleague', 'family', 'acquaintance'])
                    }
                    edges.append(edge)
                    
                    # Add edge to graph
                    self.G.add_edge(user1['user_id'], user2['user_id'], **edge)
        
        return pd.DataFrame(edges)
    
    def calculate_mutuals(self, edges_df):
        """Calculate number of mutual connections for each edge"""
        for idx, row in edges_df.iterrows():
            source_neighbors = set(self.G.neighbors(row['source']))
            target_neighbors = set(self.G.neighbors(row['target']))
            mutuals = len(source_neighbors & target_neighbors)
            edges_df.at[idx, 'num_mutuals'] = mutuals
        
        return edges_df
    
    def generate_dataset(self, output_format='csv', output_dir='./'):
        """
        Generate complete dataset with users and connections
        
        Args:
            output_format: 'csv', 'json', 'graphml', or 'all'
            output_dir: Directory to save output files
        """
        print("Generating user attributes...")
        users_df = self.generate_user_attributes()
        
        # Add nodes to graph with attributes
        for _, user in users_df.iterrows():
            self.G.add_node(user['user_id'], **user.to_dict())
        
        print("Generating edges and connections...")
        edges_df = self.generate_edges(users_df)
        
        print("Calculating mutual connections...")
        edges_df = self.calculate_mutuals(edges_df)
        
        # Save in different formats
        if output_format == 'csv' or output_format == 'all':
            users_df.to_csv(f'{output_dir}users.csv', index=False)
            edges_df.to_csv(f'{output_dir}edges.csv', index=False)
            print(f"Saved CSV files to {output_dir}")
        
        if output_format == 'json' or output_format == 'all':
            with open(f'{output_dir}users.json', 'w') as f:
                json.dump(users_df.to_dict('records'), f, indent=2)
            
            with open(f'{output_dir}edges.json', 'w') as f:
                json.dump(edges_df.to_dict('records'), f, indent=2)
            
            print(f"Saved JSON files to {output_dir}")
        
        if output_format == 'graphml' or output_format == 'all':
            # Save as GraphML (NetworkX format)
            nx.write_graphml(self.G, f'{output_dir}social_network.graphml')
            print(f"Saved GraphML file to {output_dir}")
        
        # Print statistics
        print("\n=== Dataset Statistics ===")
        print(f"Total Users: {len(users_df)}")
        print(f"Total Connections: {len(edges_df)}")
        print(f"Average Connections per User: {len(edges_df) * 2 / len(users_df):.2f}")
        print(f"Average Messages per Connection: {edges_df['num_messages'].mean():.2f}")
        print(f"Average Mutual Connections: {edges_df['num_mutuals'].mean():.2f}")
        print(f"Graph Density: {nx.density(self.G):.4f}")
        
        return self.G, users_df, edges_df


# Example usage
if __name__ == "__main__":
    # Create generator with 1000 users
    generator = SocialNetworkGenerator(num_users=1000, seed=42)
    
    # Generate dataset and save in all formats
    graph, users, edges = generator.generate_dataset(output_format='all', output_dir='./')
    
    # You can also access the NetworkX graph directly for algorithms
    print("\n=== Sample Analysis ===")
    print(f"Graph is connected: {nx.is_connected(graph)}")
    print(f"Number of connected components: {nx.number_connected_components(graph)}")
    
    # Find top 5 users by degree centrality
    centrality = nx.degree_centrality(graph)
    top_users = sorted(centrality.items(), key=lambda x: x[1], reverse=True)[:5]
    print("\nTop 5 Most Connected Users:")
    for user_id, score in top_users:
        print(f"  {user_id}: {score:.4f}")