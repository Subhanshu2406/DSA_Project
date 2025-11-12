# Social Media Dataset Generator

## Overview

Generate a realistic social media network dataset with ~1K nodes, hybrid clustering (geographic + interests), friend/fan relationships, and time-varying attributes with daily snapshots.

## Project Structure

```
DSA_Project/
├── dataset_generator/
│   ├── __init__.py
│   ├── graph_generator.py      # Main graph generation logic
│   ├── clustering.py            # Geographic & interest clustering
│   ├── relationship_manager.py  # Friend/fan relationship logic
│   ├── time_evolution.py        # Daily snapshots & attribute updates
│   └── export.py                # Export to JSON/CSV formats
├── data/
│   └── generated/               # Generated dataset files
├── config.py                    # Configuration parameters
├── requirements.txt             # Python dependencies
├── generate_dataset.py          # Main entry point
└── README.md                    # Updated documentation
```

## Implementation Details

### 1. Data Models (`graph_generator.py`)

- **Node attributes** (constant):
  - `user_id`: Unique identifier
  - `location`: Geographic coordinates (lat, lon) or region ID
  - `interests`: Set of interest tags (e.g., ["sports", "music", "tech"])
  - `created_at`: Account creation timestamp

- **Edge attributes** (time-varying):
  - `relationship_type`: "friend" (mutual) or "fan" (one-way)
  - `message_count`: Increments over time
  - `last_interaction`: Timestamp
  - `distance`: Calculated metric (mutual friends + message frequency)
  - `established_at`: When relationship was formed

### 2. Clustering Strategy (`clustering.py`)

- **Geographic clustering**:
  - Divide space into regions (e.g., 5-10 regions)
  - Users in same region have higher connection probability
  - Some cross-region connections for realism

- **Interest-based clustering**:
  - Assign 2-5 interests per user from a pool (e.g., 20 interest categories)
  - Higher connection probability for shared interests
  - Combine with geographic proximity for hybrid clustering

### 3. Graph Generation (`graph_generator.py`)

- Generate ~1K nodes with:
  - Random geographic distribution (with regional clustering)
  - Interest assignment (some overlap for clustering)
  - Account creation timestamps (staggered over time)

- Generate edges using:
  - Base probability + geographic proximity boost + interest overlap boost
  - Ensure realistic clustering (not random)
  - Initial friend/fan assignment based on mutual follows

### 4. Relationship Management (`relationship_manager.py`)

- **Friend detection**: Both nodes have edges pointing to each other
- **Fan detection**: One-way edge (A→B but not B→A)
- **Distance calculation**:
  - Count mutual friends
  - Factor in message count (normalized)
  - Formula: `distance = base_distance - (mutual_friends * weight) - (message_freq * weight)`
  - Friend nodes: smaller base_distance
  - Fan nodes: larger base_distance

### 5. Time Evolution (`time_evolution.py`)

- Generate daily snapshots over configurable period (e.g., 30-90 days)
- **Daily updates**:
  - Increment message counts (randomized, some edges more active)
  - Relationship changes:
    - Friend → Fan (one person unfollows)
    - Fan → Friend (mutual follow established)
    - New connections (new edges added)
    - Broken connections (edges removed)
  - Popular node dynamics:
    - Some nodes gain fans (viral growth)
    - Some nodes lose fans (decline)
    - Modeled with probability distributions

### 6. Export Format (`export.py`)

- **Daily snapshots**: JSON files (one per day)
  - `nodes.json`: All node attributes
  - `edges.json`: All edge attributes for that day
  - `metadata.json`: Date, stats (total nodes, edges, friends, fans)
- **Aggregated format**: CSV for easy analysis
  - `nodes.csv`: Node data
  - `edges_daily.csv`: Edge data with timestamp column

### 7. Configuration (`config.py`)

- Number of nodes (~1K)
- Number of regions for geographic clustering
- Interest pool and distribution
- Time period (number of days)
- Connection probabilities
- Relationship change rates
- Message increment rates

## Key Algorithms

1. **Clustered Graph Generation**:

   - Use NetworkX with custom connection probabilities
   - Implement geographic + interest similarity scoring
   - Ensure realistic community structure

2. **Distance Calculation**:

   - BFS/DFS to find mutual friends
   - Normalize message counts
   - Apply distance formula

3. **Temporal Evolution**:

   - Maintain graph state
   - Apply daily mutations (add/remove edges, update attributes)
   - Export snapshots

## Dependencies

- `networkx`: Graph structure
- `numpy`: Numerical operations
- `pandas`: Data manipulation
- `json`: Export format
- `random`: Randomization
- `datetime`: Time handling

## Output

- Daily snapshot files in `data/generated/`
- Each day contains complete graph state
- Ready for C++ backend import later