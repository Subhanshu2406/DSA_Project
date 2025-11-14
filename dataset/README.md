# Dataset Generator - Realistic Social Network Data

## Overview

A Python-based dataset generator that creates realistic social media network graphs with temporal evolution, geographic clustering, interest-based communities, and dynamic relationship changes. The generator produces daily snapshots of a social network that evolves over time, simulating real-world social media dynamics.

## Features

### Core Capabilities

- **Realistic Graph Generation**: Creates networks with 100-1000+ nodes
- **Hybrid Clustering**: Combines geographic regions and interest-based communities
- **Relationship Types**: Friend (mutual follow) and Fan (one-way follow) relationships
- **Temporal Evolution**: Daily snapshots with dynamic attribute changes
- **Distance Metrics**: Calculated based on mutual friends and message frequency
- **Viral Dynamics**: Models popular nodes gaining/losing followers over time

## Architecture

### Project Structure

```
dataset/
├── dataset_generator/
│   ├── __init__.py
│   ├── graph_generator.py      # Main graph generation logic
│   ├── clustering.py            # Geographic & interest clustering
│   ├── relationship_manager.py  # Friend/fan relationship logic
│   ├── time_evolution.py        # Daily snapshots & attribute updates
│   └── export.py                # JSON/CSV export functionality
├── data/
│   └── generated/               # Generated dataset files
│       └── YYYY-MM-DD/          # Daily snapshots
│           ├── nodes.json
│           ├── edges.json
│           └── metadata.json
├── config.py                    # Configuration parameters
├── generate_dataset.py          # Main entry point
├── requirements.txt             # Python dependencies
└── README.md                    # This file
```

## Algorithms & Concepts

### 1. Graph Generation (`graph_generator.py`)

#### Node Generation
- **Geographic Distribution**: Random coordinates with regional clustering
- **Interest Assignment**: Each user gets 2-5 interests from 20 categories
- **Account Creation**: Staggered timestamps (up to 180 days before start date)
- **Region Assignment**: Based on geographic coordinates

#### Edge Generation
Uses **hybrid probability model**:

```
P(connection) = base_prob + geographic_boost + interest_boost
```

Where:
- `base_prob = 0.02` (2% base connection probability)
- `geographic_boost = 0.15` if same region
- `interest_boost = 0.10 × shared_interests` (max 0.30)

This creates **realistic clustering**:
- Users in same region more likely to connect
- Users with shared interests more likely to connect
- Some cross-cluster connections for realism

**Time Complexity**: O(V²) for full graph generation
**Space Complexity**: O(V + E)

---

### 2. Geographic Clustering (`clustering.py`)

#### Region Division
- Divides geographic space into **8 regions**
- Uses coordinate-based assignment
- Ensures regional clustering in connection probabilities

#### Clustering Strategy
- **Intra-region connections**: Higher probability (base + 0.15)
- **Inter-region connections**: Lower probability (base only)
- Creates **geographic communities** naturally

**Algorithm**: Spatial partitioning with probability weighting

---

### 3. Interest-Based Clustering

#### Interest Categories
20 predefined categories:
- Technology, Music, Sports, Travel, Food & Cooking
- Photography, Fitness, Art & Design, Gaming, Movies & TV
- Reading, Fashion, Nature & Outdoors, Science, Business
- Education, Health & Wellness, Comedy & Humor, Politics, DIY & Crafts

#### Clustering Mechanism
- Each user assigned 2-5 random interests
- **Shared interest boost**: +0.10 per shared interest
- Maximum boost: 0.30 (3+ shared interests)
- Creates **interest-based communities**

**Algorithm**: Set intersection with probability scaling

---

### 4. Relationship Management (`relationship_manager.py`)

#### Relationship Types

**Friend Relationship**:
- Both users follow each other (bidirectional edge)
- Represents mutual connection
- Base distance: 5.0

**Fan Relationship**:
- One-way follow (A follows B, but B doesn't follow A)
- Represents influencer-follower dynamic
- Base distance: 15.0

#### Distance Calculation

Uses **weighted distance metric**:

```
distance = base_distance - (mutual_friends × mutual_weight) - (message_freq × message_weight)
```

Where:
- `mutual_weight = 0.5`
- `message_weight = 0.3`
- `message_freq = normalized(message_count)`

**Lower distance = stronger relationship**

**Algorithm**: Graph traversal + normalization

---

### 5. Temporal Evolution (`time_evolution.py`)

#### Daily Updates

**Message Count Increments**:
- Probability: 30% per edge per day
- Increment: 0-10 messages per day
- Models active vs. inactive relationships

**Relationship Changes**:
- **Friend → Fan**: 1% probability (one person unfollows)
- **Fan → Friend**: 2% probability (mutual follow established)
- **New Connection**: 0.5% probability (new edge added)
- **Broken Connection**: 0.3% probability (edge removed)

**Popular Node Dynamics**:
- **Viral Nodes** (10 nodes):
  - Gain fans: 15% probability per day
  - Lose fans: 5% probability per day
- **Normal Nodes**:
  - Gain fans: 1% probability per day
  - Lose fans: 0.5% probability per day

**Algorithm**: Probabilistic state machine with weighted transitions

---

### 6. Data Export (`export.py`)

#### JSON Format
Daily snapshots in structured JSON:
- `nodes.json`: Array of node objects
- `edges.json`: Array of edge objects
- `metadata.json`: Graph statistics and date

#### CSV Format
Aggregated data for analysis:
- `nodes.csv`: All node attributes
- `edges_daily.csv`: Edge attributes with timestamps

---

## Data Model

### Node Attributes (Constant)

```json
{
  "user_id": 0,
  "name": "John Doe",
  "location": [latitude, longitude],
  "region_id": 3,
  "interests": ["interest_0", "interest_5", "interest_12"],
  "created_at": "2023-09-15T00:00:00"
}
```

**Fields**:
- `user_id`: Unique identifier (0 to N-1)
- `name`: Randomly generated full name
- `location`: Geographic coordinates [lat, lon]
- `region_id`: Geographic region (0-7)
- `interests`: List of interest tags (2-5 per user)
- `created_at`: Account creation timestamp

### Edge Attributes (Time-Varying)

```json
{
  "source": 5,
  "target": 12,
  "relationship_type": "friend",
  "message_count": 45,
  "last_interaction": "2024-01-03T14:30:00",
  "distance": 3.2,
  "established_at": "2023-12-15T00:00:00"
}
```

**Fields**:
- `source`: Source user ID
- `target`: Target user ID
- `relationship_type`: "friend" or "fan"
- `message_count`: Total messages exchanged (increments daily)
- `last_interaction`: Most recent interaction timestamp
- `distance`: Calculated relationship strength (lower = stronger)
- `established_at`: When relationship was formed

### Metadata

```json
{
  "date": "2024-01-01",
  "total_nodes": 100,
  "total_edges": 974,
  "friend_relationships": 450,
  "fan_relationships": 524,
  "average_degree": 9.74
}
```

---

## Configuration (`config.py`)

### Key Parameters

```python
NUM_NODES = 100                    # Number of users
NUM_REGIONS = 8                    # Geographic regions
MIN_INTERESTS_PER_USER = 2        # Minimum interests
MAX_INTERESTS_PER_USER = 5        # Maximum interests
TOTAL_INTEREST_CATEGORIES = 20    # Interest pool size

BASE_CONNECTION_PROB = 0.02        # Base connection probability
GEOGRAPHIC_BOOST = 0.15           # Same region boost
INTEREST_OVERLAP_BOOST = 0.10     # Per shared interest boost
MAX_INTEREST_BOOST = 0.30         # Maximum interest boost

NUM_DAYS = 5                       # Days to generate
START_DATE = "2024-01-01"         # Start date

DAILY_MESSAGE_INCREMENT_PROB = 0.3
MIN_MESSAGES_PER_DAY = 0
MAX_MESSAGES_PER_DAY = 10

FRIEND_TO_FAN_PROB = 0.01         # Relationship change rates
FAN_TO_FRIEND_PROB = 0.02
NEW_CONNECTION_PROB = 0.005
BREAK_CONNECTION_PROB = 0.003
```

---

## Installation & Usage

### Prerequisites

```bash
pip install -r requirements.txt
```

**Dependencies**:
- `numpy`: Numerical operations
- `pandas`: Data manipulation
- `networkx`: Graph operations (optional, for analysis)

### Generate Dataset

```bash
python generate_dataset.py
```

This creates:
- Daily JSON snapshots in `data/generated/YYYY-MM-DD/`
- Aggregated CSV files in `data/generated/`

### Customization

Edit `config.py` to customize:
- Number of nodes
- Number of days
- Connection probabilities
- Relationship change rates
- Message increment rates

---

## Realistic Features

### 1. Clustering
- **Geographic**: Users in same region cluster together
- **Interest-based**: Users with shared interests connect more
- **Hybrid**: Both factors combine for realistic communities

### 2. Relationship Dynamics
- **Friend relationships**: Mutual follows (stronger bonds)
- **Fan relationships**: One-way follows (influencer dynamics)
- **Evolution**: Relationships change over time

### 3. Temporal Patterns
- **Message activity**: Some relationships more active than others
- **Viral growth**: Some nodes gain followers rapidly
- **Natural decay**: Some connections fade over time

### 4. Network Properties
- **Small-world**: Short average path lengths
- **Scale-free**: Some nodes have many connections (power law)
- **Community structure**: Clear clusters visible

---

## Algorithm Complexity

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Graph Generation | O(V²) | O(V + E) |
| Edge Generation | O(V²) | O(E) |
| Relationship Detection | O(E) | O(V) |
| Distance Calculation | O(V × avg_degree) | O(V) |
| Daily Updates | O(E) | O(1) |
| Export | O(V + E) | O(V + E) |

---

## Use Cases

1. **Algorithm Testing**: Test graph algorithms on realistic data
2. **Visualization**: Create visualizations of social networks
3. **Research**: Study social network properties
4. **Education**: Teach graph theory concepts
5. **Benchmarking**: Compare algorithm performance

---

## Output Format

### Daily Snapshots Structure

```
data/generated/
├── 2024-01-01/
│   ├── nodes.json      # 100 nodes
│   ├── edges.json      # ~1000 edges
│   └── metadata.json   # Statistics
├── 2024-01-02/
│   ├── nodes.json      # Same nodes, updated edges
│   ├── edges.json      # Updated relationships
│   └── metadata.json   # Updated statistics
└── ...
```

### File Sizes (100 nodes, 5 days)
- `nodes.json`: ~15 KB per day
- `edges.json`: ~50 KB per day
- `metadata.json`: ~0.5 KB per day
- **Total**: ~65 KB per day

---

## Future Enhancements

- **More relationship types**: Groups, events, pages
- **Content generation**: Posts, comments, likes
- **Temporal patterns**: Daily/weekly activity cycles
- **Geographic realism**: Real-world location data
- **Interest evolution**: Changing interests over time
- **Multi-layer networks**: Multiple interaction types

---

## References

- **Small-World Networks**: Watts & Strogatz model
- **Scale-Free Networks**: Barabási–Albert model
- **Community Detection**: Modularity optimization
- **Temporal Networks**: Time-evolving graph models
