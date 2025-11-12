"""
Configuration file for social media dataset generator.
"""

# Graph parameters
NUM_NODES = 1000
NUM_REGIONS = 8  # Geographic regions for clustering
MIN_INTERESTS_PER_USER = 2
MAX_INTERESTS_PER_USER = 5
TOTAL_INTEREST_CATEGORIES = 20

# Connection probabilities
BASE_CONNECTION_PROB = 0.02  # Base probability of connection
GEOGRAPHIC_BOOST = 0.15  # Additional probability if same region
INTEREST_OVERLAP_BOOST = 0.10  # Additional probability per shared interest
MAX_INTEREST_BOOST = 0.30  # Maximum boost from interests

# Relationship parameters
FRIEND_BASE_DISTANCE = 5.0  # Base distance for friend nodes
FAN_BASE_DISTANCE = 15.0  # Base distance for fan nodes
MUTUAL_FRIEND_WEIGHT = 0.5  # Weight for mutual friends in distance calculation
MESSAGE_FREQ_WEIGHT = 0.3  # Weight for message frequency in distance calculation

# Time evolution parameters
NUM_DAYS = 90  # Number of days to generate snapshots
START_DATE = "2024-01-01"  # Start date for dataset

# Daily update probabilities
DAILY_MESSAGE_INCREMENT_PROB = 0.3  # Probability an edge gets messages
MIN_MESSAGES_PER_DAY = 0
MAX_MESSAGES_PER_DAY = 10

# Relationship change probabilities
FRIEND_TO_FAN_PROB = 0.01  # Probability a friend relationship becomes fan (unfollow)
FAN_TO_FRIEND_PROB = 0.02  # Probability a fan relationship becomes friend (mutual follow)
NEW_CONNECTION_PROB = 0.005  # Probability of new connection forming per day
BREAK_CONNECTION_PROB = 0.003  # Probability of connection breaking per day

# Popular node dynamics
VIRAL_NODE_COUNT = 10  # Number of nodes that can go viral
VIRAL_GAIN_FANS_PROB = 0.15  # Probability viral node gains fans per day
VIRAL_LOSE_FANS_PROB = 0.05  # Probability viral node loses fans per day
NORMAL_GAIN_FANS_PROB = 0.01  # Probability normal node gains fans per day
NORMAL_LOSE_FANS_PROB = 0.005  # Probability normal node loses fans per day

# Account creation
ACCOUNT_CREATION_START_DAYS_BEFORE = 180  # Accounts created up to 180 days before start
ACCOUNT_CREATION_END_DAYS_BEFORE = 0  # Some accounts created on start date

# Export settings
EXPORT_JSON = True
EXPORT_CSV = True
OUTPUT_DIR = "data/generated"

