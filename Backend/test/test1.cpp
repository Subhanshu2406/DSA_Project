// Backend/test/test1.cpp
#include "graph_generator.hpp"   // correct relative path from Backend/test -> Backend/graph_generator.hpp
#include <iostream>
using namespace std;

int main() {
    SocialGraph g;

    // Update these paths if your dataset folder is somewhere else.
    // From Backend/test the dataset in your tree appears at: ../dataset/data/generated/2024-01-01/...
    const string nodesPath = "../dataset/data/generated/2024-01-01/nodes.json";
    const string edgesPath = "../dataset/data/generated/2024-01-01/edges.json";
    const string metaPath  = "../dataset/data/generated/2024-01-01/metadata.json";

    if (!g.initializeGraph(nodesPath, edgesPath, metaPath)) {
        cerr << "Failed to initialize graph (check file paths and json library)\n";
        return 1;
    }

    g.printStatistics();
    return 0;
}
