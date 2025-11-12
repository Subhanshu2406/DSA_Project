# DSA_Project
3rd sem DSA course's project

## Social Media Network Analyzer - Dataset Generator

A Python-based dataset generator for creating realistic social media network graphs with temporal evolution, geographic and interest-based clustering, and friend/fan relationship dynamics.

### Features

- **Realistic Graph Generation**: ~1K nodes with hybrid clustering (geographic regions + interest-based communities)
- **Relationship Types**: Friend (mutual follow) and Fan (one-way follow) relationships
- **Distance Metrics**: Calculated based on mutual friends and message frequency
- **Temporal Evolution**: Daily snapshots with:
  - Message count increments
  - Relationship changes (friend ↔ fan)
  - New and broken connections
  - Popular node dynamics (viral growth/decline)
- **Multiple Export Formats**: JSON (daily snapshots) and CSV (aggregated data)

### Project Structure

```
DSA_Project/
├── dataset_generator/      # Core modules
│   ├── clustering.py       # Geographic & interest clustering
│   ├── graph_generator.py  # Main graph generation
│   ├── relationship_manager.py  # Friend/fan logic & distance
│   ├── time_evolution.py   # Daily snapshots & updates
│   └── export.py           # JSON/CSV export
├── data/generated/         # Generated dataset files
├── config.py               # Configuration parameters
├── generate_dataset.py     # Main entry point
└── requirements.txt        # Python dependencies
```

### Installation

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Run the dataset generator:
```bash
python generate_dataset.py
```

### Configuration

Edit `config.py` to customize:
- Number of nodes (default: 1000)
- Number of days to generate (default: 90)
- Connection probabilities
- Relationship change rates
- Message increment rates
- Geographic regions and interest categories

### Output

The generator creates:
- **Daily JSON snapshots**: `data/generated/YYYY-MM-DD/nodes.json`, `edges.json`, `metadata.json`
- **Aggregated CSV files**: `data/generated/nodes.csv`, `edges_daily.csv`

### Data Model

**Node Attributes** (constant):
- `user_id`: Unique identifier
- `location`: Geographic coordinates (lat, lon)
- `region_id`: Geographic region
- `interests`: List of interest tags
- `created_at`: Account creation timestamp

**Edge Attributes** (time-varying):
- `relationship_type`: "friend" or "fan"
- `message_count`: Increments over time
- `last_interaction`: Timestamp
- `distance`: Calculated metric (mutual friends + message frequency)
- `established_at`: When relationship was formed

### Future Work

This dataset will be used to implement:
- Search history tracking
- Trending algorithms
- Influence radius calculation
- Global and local virality leaderboards
- Search autocomplete
- Community detection
- Friendship scoring
- Cluster blocking
- Mutual friends and friend recommendations

The backend implementation will be in C++ for performance.