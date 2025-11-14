# Dataset Generator - Realistic Social Graph Data

Python-based dataset generator for creating realistic social media network graphs with temporal evolution, geographic and interest-based clustering, and friend/fan relationship dynamics.

## Overview

This module generates realistic social network datasets that simulate real-world social media behavior. The generator creates daily snapshots of a social graph with evolving relationships, message counts, and network dynamics.

## How We Generate Realistic Data

### 1. Hybrid Clustering Strategy

**Geographic Clustering:**
- Divides users into 8 geographic regions
- Users in the same region have **15% higher connection probability**
- Creates natural geographic communities
- Some cross-region connections for realism

**Interest-Based Clustering:**
- Each user gets 2-5 interests from a pool of 20 categories
  - Technology, Music, Sports, Travel, Food & Cooking, Photography, Fitness, Art & Design, Gaming, Movies & TV, Reading, Fashion, Nature & Outdoors, Science, Business, Education, Health & Wellness, Comedy & Humor, Politics, DIY & Crafts
- **10% connection boost per shared interest** (max 30% boost)
- Users with common interests are more likely to connect

**Combined Effect:**
```
Connection Probability = Base (2%) + Geographic Boost (0-15%) + Interest Boost (0-30%)
```

This creates **realistic clustering** - not random, but structured communities.

### 2. Node Generation

**Attributes:**
- `user_id`: Unique identifier
- `name`: Randomly generated realistic names
- `location`: Geographic coordinates (latitude, longitude)
- `region_id`: Assigned geographic region (0-7)
- `interests`: 2-5 interest tags from 20 categories
- `created_at`: Account creation timestamp (staggered over 180 days)

**Distribution:**
- Random geographic distribution with regional clustering
- Interest assignment with some overlap for clustering
- Staggered account creation (not all at once)

### 3. Edge Generation (Connections)

**Connection Probability Formula:**
```python
prob = BASE_CONNECTION_PROB  # 2%
if same_region:
    prob += GEOGRAPHIC_BOOST  # +15%
prob += min(shared_interests × INTEREST_OVERLAP_BOOST, MAX_INTEREST_BOOST)  # +0-30%
```

**Relationship Types:**
- **Friend**: Mutual connection (both users follow each other)
  - Detected when edges exist in both directions
  - Base distance: 5.0
- **Fan**: One-way connection (A follows B, but B doesn't follow A)
  - Base distance: 15.0

**Realism Factors:**
- Not purely random - probability increases with similarity
- Creates natural communities
- Some long-distance connections for realism

### 4. Distance Metric Calculation

**Formula:**
```
distance = base_distance - (mutual_friends × 0.5) - (message_frequency × 0.3)
```

- **Mutual friends**: More mutual friends = lower distance (stronger connection)
- **Message frequency**: More messages = lower distance (more interaction)
- **Base distances**: Friends (5.0) vs Fans (15.0)

This metric measures **relationship strength** based on real social factors.

### 5. Temporal Evolution (Daily Snapshots)

**Daily Updates:**

1. **Message Count Increments:**
   - 30% of edges get 0-10 new messages per day
   - Simulates active vs inactive relationships
   - More active relationships get more messages

2. **Relationship Changes:**
   - **Friend → Fan**: 1% probability (one person unfollows)
   - **Fan → Friend**: 2% probability (mutual follow established)
   - Models real relationship dynamics

3. **Network Dynamics:**
   - **New connections**: 0.5% probability per day
   - **Broken connections**: 0.3% probability per day
   - Network grows and evolves over time

4. **Viral Node Dynamics:**
   - 10 nodes designated as "viral" (influencers)
   - **Viral nodes**: 15% chance to gain fans, 5% chance to lose fans per day
   - **Normal nodes**: 1% chance to gain fans, 0.5% chance to lose fans per day
   - Models real social media growth patterns

### 6. Edge Attributes (Time-Varying)

Each edge has time-varying attributes:
- `relationship_type`: "friend" or "fan" (can change daily)
- `message_count`: Increments over time (0-10 per day)
- `last_interaction`: Timestamp of last activity
- `distance`: Recalculated based on mutual friends and messages
- `established_at`: When relationship was formed (constant)

## Generation Process

### Step-by-Step:

1. **Initialize Graph** (Day 0):
   - Generate 100 nodes with geographic locations, regions, interests
   - Create connections using probability formula
   - Detect friend vs fan relationships
   - Calculate initial distance metrics
   - Assign account creation timestamps (staggered)

2. **Daily Evolution** (Day 1, 2, 3...):
   - Update message counts (30% of edges)
   - Change relationship types (friend ↔ fan)
   - Add new connections (0.5% probability)
   - Remove broken connections (0.3% probability)
   - Apply viral node dynamics
   - Recalculate distance metrics
   - Export daily snapshot

3. **Export Snapshots**:
   - `nodes.json`: All node attributes (constant)
   - `edges.json`: All edge attributes for that day (time-varying)
   - `metadata.json`: Date, statistics (nodes, edges, friends, fans, avg degree)

## Configuration

Edit `config.py` to customize:

```python
NUM_NODES = 100                    # Number of users
NUM_REGIONS = 8                    # Geographic regions
MIN_INTERESTS_PER_USER = 2         # Min interests per user
MAX_INTERESTS_PER_USER = 5         # Max interests per user
TOTAL_INTEREST_CATEGORIES = 20     # Interest pool size

BASE_CONNECTION_PROB = 0.02        # Base connection probability
GEOGRAPHIC_BOOST = 0.15            # Same region boost
INTEREST_OVERLAP_BOOST = 0.10      # Per shared interest boost
MAX_INTEREST_BOOST = 0.30          # Max interest boost

NUM_DAYS = 5                       # Days to generate
START_DATE = "2024-01-01"          # Start date

DAILY_MESSAGE_INCREMENT_PROB = 0.3 # Message update probability
FRIEND_TO_FAN_PROB = 0.01         # Relationship change rates
FAN_TO_FRIEND_PROB = 0.02
NEW_CONNECTION_PROB = 0.005       # Network growth rates
BREAK_CONNECTION_PROB = 0.003

VIRAL_NODE_COUNT = 10             # Number of viral nodes
VIRAL_GAIN_FANS_PROB = 0.15       # Viral growth rates
```

## Installation

```bash
cd dataset
pip install -r requirements.txt
```

## Usage

```bash
python generate_dataset.py
```

This generates daily snapshots in `data/generated/YYYY-MM-DD/` directory.

## Output Format

### Daily Snapshots

**`nodes.json`**: Array of node objects
```json
[
  {
    "user_id": 0,
    "name": "John Doe",
    "location": [40.7128, -74.0060],
    "region_id": 3,
    "interests": ["Technology", "Music", "Sports"],
    "created_at": "2023-09-15T00:00:00"
  }
]
```

**`edges.json`**: Array of edge objects
```json
[
  {
    "source": 0,
    "target": 1,
    "relationship_type": "friend",
    "message_count": 45,
    "last_interaction": "2024-01-01T12:30:00",
    "distance": 3.2,
    "established_at": "2023-10-20T00:00:00"
  }
]
```

**`metadata.json`**: Graph statistics
```json
{
  "date": "2024-01-01",
  "total_nodes": 100,
  "total_edges": 974,
  "friend_relationships": 487,
  "fan_relationships": 487,
  "average_degree": 9.74
}
```

## Realism Factors

1. **Clustering**: Geographic + interest-based clustering creates natural communities
2. **Temporal Dynamics**: Daily changes simulate real network evolution
3. **Relationship Types**: Friend vs fan distinction models real social media
4. **Viral Dynamics**: Some nodes gain/lose popularity over time
5. **Staggered Creation**: Accounts created over time, not all at once
6. **Probabilistic Updates**: Realistic randomness in messages and relationship changes
7. **Distance Metrics**: Based on mutual friends and interaction frequency

## Project Structure

```
dataset/
├── dataset_generator/
│   ├── graph_generator.py      # Main graph generation
│   ├── clustering.py            # Geographic & interest clustering
│   ├── relationship_manager.py  # Friend/fan logic & distance
│   ├── time_evolution.py        # Daily snapshots & updates
│   └── export.py                # JSON/CSV export
├── data/
│   └── generated/               # Generated dataset files
│       └── YYYY-MM-DD/          # Daily snapshots
├── config.py                    # Configuration parameters
├── generate_dataset.py          # Main entry point
├── requirements.txt             # Python dependencies
└── README.md                    # This file
```

## Data Model

**Node Attributes** (constant):
- `user_id`: Unique identifier
- `name`: User name
- `location`: [latitude, longitude]
- `region_id`: Geographic region (0-7)
- `interests`: Array of interest strings
- `created_at`: ISO timestamp

**Edge Attributes** (time-varying):
- `source`: Source user ID
- `target`: Target user ID
- `relationship_type`: "friend" or "fan"
- `message_count`: Total messages exchanged
- `last_interaction`: Last activity timestamp
- `distance`: Relationship strength metric
- `established_at`: Relationship formation timestamp

## Statistics

For default configuration (100 nodes, 5 days):
- **Nodes**: 100 (constant)
- **Edges**: ~900-1000 per day (varies with evolution)
- **Friends**: ~45-50% of edges
- **Fans**: ~45-50% of edges
- **Average degree**: ~9-10 connections per node

## Future Enhancements

- Support for larger graphs (1000+ nodes)
- More sophisticated viral dynamics
- Interest evolution over time
- Geographic migration patterns
- Message content simulation
