#include "campus_manager.cpp"

// CampusMap class with improved features
class CampusMap {
private:
    vector<Location> locations;
    vector<vector<Path>> adjacencyList;
    Statistics stats;
    Logger logger;
    
public:
    CampusMap() {}
    
    // Add a new location
    void addLocation(const Location& loc) {
        // Ensure adjacency list is large enough
        if (loc.getId() >= adjacencyList.size()) {
            adjacencyList.resize(loc.getId() + 1);
        }
        
        // Check if location with this ID already exists
        for (size_t i = 0; i < locations.size(); i++) {
            if (locations[i].getId() == loc.getId()) {
                locations[i] = loc; // Replace existing location
                logger.log("Updated location: " + loc.getName());
                return;
            }
        }
        
        // Add new location
        locations.push_back(loc);
        logger.log("Added new location: " + loc.getName());
    }
    
    // Add a new path
    void addPath(const Path& path) {
        int from = path.getFrom();
        int to = path.getTo();
        
        // Ensure adjacency list is large enough
        if (from >= adjacencyList.size()) {
            adjacencyList.resize(from + 1);
        }
        
        // Check if path already exists
        for (size_t i = 0; i < adjacencyList[from].size(); i++) {
            if (adjacencyList[from][i].getTo() == to) {
                adjacencyList[from][i] = path; // Replace existing path
                logger.log("Updated path from " + to_string(from) + " to " + to_string(to));
                return;
            }
        }
        
        // Add new path
        adjacencyList[from].push_back(path);
        logger.log("Added new path from " + to_string(from) + " to " + to_string(to));
    }
    
    // Get a location by ID
    Location* getLocationById(int id) {
        for (auto& loc : locations) {
            if (loc.getId() == id) {
                return &loc;
            }
        }
        return nullptr;
    }
    
    // Get a location by name
    Location* getLocationByName(const string& name) {
        for (auto& loc : locations) {
            if (loc.getName() == name) {
                return &loc;
            }
        }
        return nullptr;
    }
    
    // Update cleanliness status of all locations
    void updateCleanlinessStatus(int days) {
        for (auto& loc : locations) {
            loc.updateCleanliness(days);
            stats.addCleanlinessRecord(loc.getId(), loc.getCleanlinessStatus());
        }
        logger.log("Updated cleanliness status for all locations over " + to_string(days) + " days");
    }
    
    // Clean a location
    void cleanLocation(int locId) {
        Location* loc = getLocationById(locId);
        if (loc) {
            loc->clean();
            stats.incrementVisitCount(locId);
            logger.log("Cleaned location: " + loc->getName());
        }
    }
    
    // Calculate dynamic priority for a location
    double calculateDynamicPriority(int locId) const {
        for (const auto& loc : locations) {
            if (loc.getId() == locId) {
                // Calculate priority based on multiple factors
                double timeFactorNormalized = min(1.0, static_cast<double>(loc.getLastCleaned()) / loc.getCleaningFrequency());
                if (loc.getLastCleaned() < loc.getCleaningFrequency()) {
                    timeFactorNormalized *= 0.2; // Penalize recently cleaned locations
                }
                
                double priority = (loc.getImportance() * 0.3) + 
                                 ((100 - loc.getCleanlinessStatus()) * 0.4) + 
                                 (loc.getVisitPriority() * 0.1) + 
                                 (timeFactorNormalized * 0.2);
                                 
                // Additional factors for special care
                if (loc.getNeedsSpecialCare()) {
                    priority *= 1.2; // 20% increase for locations needing special care
                }
                
                return priority;
            }
        }
        return 0.0;
    }
    
    // Get adjacency list for path finding
    const vector<vector<Path>>& getAdjacencyList() const {
        return adjacencyList;
    }
    
    // Get all locations
    const vector<Location>& getLocations() const {
        return locations;
    }
    
    // Get visit count for a location
    int getVisitCount(int locId) const {
        return stats.getVisitCount(locId);
    }
    
    // Print status of all locations
    void printLocationsStatus() const {
        cout << "\n=== Campus Locations Status ===\n";
        cout << setw(30) << left << "Location" 
             << setw(14) << right << "Cleanliness" 
             << setw(16) << right << "Last Cleaned" 
             << setw(14) << right << "Priority" 
             << setw(13) << right << "Visits"
             << setw(14) << right << "Category" << "\n";
    
        cout << string(101, '-') << "\n";  
    
        for (const auto& loc : locations) {
            double priority = const_cast<CampusMap*>(this)->calculateDynamicPriority(loc.getId());
            cout << setw(30) << left << loc.getName() 
                 << setw(9) << right << fixed << setprecision(2) << loc.getCleanlinessStatus() << " %"  
                 << setw(13) << right << loc.getLastCleaned() << " days"  
                 << setw(14) << right << fixed << setprecision(2) << priority
                 << setw(10) << right << stats.getVisitCount(loc.getId())
                 << setw(14) << right << loc.getCategory() << "\n";
        }
        cout << "\n";
    }
    
    // Print detailed information about a specific location
    void printLocationDetail(int locId) const {
        const Location* loc = nullptr;
        for (const auto& l : locations) {
            if (l.getId() == locId) {
                loc = &l;
                break;
            }
        }
        
        if (!loc) {
            cout << "Location with ID " << locId << " not found.\n";
            return;
        }
        
        cout << "\n=== Location Details ===\n";
        cout << "ID: " << loc->getId() << "\n";
        cout << "Name: " << loc->getName() << "\n";
        cout << "Category: " << loc->getCategory() << "\n";
        cout << "Description: " << (!loc->getDescription().empty() ? loc->getDescription() : "No description") << "\n";
        cout << "Importance (1-10): " << loc->getImportance() << "\n";
        cout << "Visit Priority (1-10): " << loc->getVisitPriority() << "\n";
        cout << "Cleaning Frequency: Every " << loc->getCleaningFrequency() << " days\n";
        cout << "Special Care Required: " << (loc->getNeedsSpecialCare() ? "Yes" : "No") << "\n";
        cout << "Current Cleanliness: " << fixed << setprecision(2) << loc->getCleanlinessStatus() << "%\n";
        cout << "Last Cleaned: " << loc->getLastCleaned() << " days ago\n";
        cout << "Total Visits: " << stats.getVisitCount(loc->getId()) << "\n";
        cout << "Average Cleanliness: " << fixed << setprecision(2) << stats.getAverageCleanliness(loc->getId()) << "%\n";
        
        // Show connected locations
        cout << "\nConnected to:\n";
        if (loc->getId() < adjacencyList.size()) {
            for (const auto& path : adjacencyList[loc->getId()]) {
                Location* destLoc = const_cast<CampusMap*>(this)->getLocationById(path.getTo());
                if (destLoc) {
                    cout << "  - " << destLoc->getName() 
                         << " (Distance: " << path.getDistance() 
                         << ", Travel Time: " << path.getTravelTime() 
                         << ", Difficulty: " << path.getDifficulty() << ")\n";
                }
            }
        }
        cout << "\n";
    }
    
    // Save campus data to file
    bool saveToFile(const string& filename) {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "Error opening file for writing: " << filename << endl;
            return false;
        }

        // Save locations
        outFile << "# Locations\n";
        for (const auto& loc : locations) {
            outFile << loc.serialize() << "\n";
        }

        // Save paths
        outFile << "# Paths\n";
        for (size_t from = 0; from < adjacencyList.size(); ++from) {
            for (const auto& path : adjacencyList[from]) {
                outFile << path.serialize() << "\n";
            }
        }

        outFile.close();
        logger.log("Saved campus data to " + filename);
        return true;
    }
    
    // Load campus data from file
    bool loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "Data file not found: " << filename << ". Starting with new data.\n";
            return false;
        }
        
        locations.clear();
        adjacencyList.clear();
        
        string line;
        string currentSection;
        
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            
            // Check section headers
            if (line[0] == '#') {
                currentSection = line;
                continue;
            }
            
            try {
                // Process locations
                if (currentSection == "# Locations") {
                    Location loc = Location::deserialize(line);
                    addLocation(loc);
                } 
                // Process paths
                else if (currentSection == "# Paths") {
                    Path path = Path::deserialize(line);
                    addPath(path);
                }
            }
            catch (const exception& e) {
                cerr << "Error parsing line: " << e.what() << endl;
            }
        }
        
        inFile.close();
        logger.log("Loaded campus data from " + filename);
        return true;
    }
    
    // Generate a backup of the data
    bool backupData() {
        bool success = saveToFile(BACKUP_FILE);
        if (success) {
            logger.log("Created backup of campus data");
        } else {
            logger.log("Failed to create backup of campus data");
        }
        return success;
    }
    
    // Restore data from backup
    bool restoreFromBackup() {
        bool success = loadFromFile(BACKUP_FILE);
        if (success) {
            logger.log("Restored campus data from backup");
        } else {
            logger.log("Failed to restore campus data from backup");
        }
        return success;
    }
    
    // Reset to default configuration
    bool resetToDefault() {
        bool success = loadFromFile(DATA_FILE);
        if (success) {
            logger.log("Reset campus data to default configuration");
            // After loading from default, save to backup
            backupData();
        } else {
            logger.log("Failed to reset campus data to default configuration");
        }
        return success;
    }
    
    // Get statistics
    Statistics& getStatistics() {
        return stats;
    }
};

// Advanced route optimizer with multiple algorithms
class RouteOptimizer {
private:
    CampusMap* campus;
    double alpha, beta, gamma, delta; // Weight factors
    string algorithm;
    
public:
    RouteOptimizer(CampusMap* c, const string& alg = "modified_dijkstra", 
                  double a = 0.6, double b = 0.3, double g = 0.1, double d = 0.2) 
        : campus(c), algorithm(alg), alpha(a), beta(b), gamma(g), delta(d) {}
    
    // Set algorithm type
    void setAlgorithm(const string& alg) {
        algorithm = alg;
    }
    
    // Set weight factors
    void setWeights(double a, double b, double g, double d) {
        alpha = a;
        beta = b;
        gamma = g;
        delta = d;
    }
    
    // Find optimal path between two locations
    vector<int> findPath(int start, int end) {
        if (algorithm == "modified_dijkstra") {
            return modifiedDijkstra(start, end);
        } else if (algorithm == "nearest_neighbor") {
            return nearestNeighbor(start, end);
        } else {
            // Default to modified Dijkstra
            return modifiedDijkstra(start, end);
        }
    }
    
    // Modified Dijkstra algorithm
    vector<int> modifiedDijkstra(int start, int end) {
        const auto& adjList = campus->getAdjacencyList();
        int n = adjList.size();
        
        if (start >= n || end >= n) {
            return {};  // Invalid start or end location
        }
        
        vector<double> distances(n, numeric_limits<double>::max());
        vector<int> parents(n, -1);
        distances[start] = 0;
        
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
        pq.push({0, start});
        
        while (!pq.empty()) {
            double cost = pq.top().first;
            int node = pq.top().second;
            pq.pop();
            
            if (cost > distances[node]) continue;
            
            for (const auto& edge : adjList[node]) {
                int to = edge.getTo();
                
                // Calculate weighted edge cost
                double priorityFactor = 1.0 - (campus->calculateDynamicPriority(to) / 100.0);
                double visitFactor = 1.0 + (campus->getVisitCount(to) * delta);
                
                double weight = (alpha * edge.getDistance()) + 
                               (beta * edge.getDifficulty()) + 
                               (gamma * visitFactor) + 
                               (edge.getRequiresKey() ? 5.0 : 0.0);  // Penalty for paths requiring keys
                
                // Priority reduces the cost (more important locations are easier to include)
                weight *= (2.0 - priorityFactor);
                
                // Indoor paths might be preferred in certain conditions
                if (edge.getIsIndoor()) {
                    weight *= 0.9;  // 10% reduction for indoor paths
                }
                
                double newCost = distances[node] + weight;
                
                if (newCost < distances[to]) {
                    distances[to] = newCost;
                    parents[to] = node;
                    pq.push({newCost, to});
                }
            }
        }
        
        vector<int> path;
        if (distances[end] == numeric_limits<double>::max()) {
            // No path exists
            return path;
        }
        
        for (int at = end; at != -1; at = parents[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
        return path;
    }
    
    // Nearest Neighbor algorithm for TSP-like problems
    vector<int> nearestNeighbor(int start, int end) {
        const auto& adjList = campus->getAdjacencyList();
        int n = adjList.size();
        
        if (start >= n || end >= n) {
            return {};  // Invalid start or end location
        }
        
        vector<bool> visited(n, false);
        vector<int> path;
        path.push_back(start);
        visited[start] = true;
        
        int current = start;
        while (current != end) {
            double minDist = numeric_limits<double>::max();
            int nextNode = -1;
            
            for (const auto& edge : adjList[current]) {
                int to = edge.getTo();
                
                if (!visited[to]) {
                    // Calculate weighted edge cost (similar to Dijkstra)
                    double priorityFactor = 1.0 - (campus->calculateDynamicPriority(to) / 100.0);
                    double visitFactor = 1.0 + (campus->getVisitCount(to) * delta);
                    
                    double weight = (alpha * edge.getDistance()) + 
                                   (beta * edge.getDifficulty()) + 
                                   (gamma * visitFactor) + 
                                   (edge.getRequiresKey() ? 5.0 : 0.0);
                    
                    weight *= (2.0 - priorityFactor);
                    
                    if (edge.getIsIndoor()) {
                        weight *= 0.9;
                    }
                    
                    if (weight < minDist) {
                        minDist = weight;
                        nextNode = to;
                    }
                }
            }
            
            if (nextNode == -1) {
                // No unvisited neighbor
                if (current == end) {
                    // Already at the destination
                    break;
                }
                
                // Try to find a path to the end using Dijkstra
                vector<int> remainingPath = modifiedDijkstra(current, end);
                
                if (remainingPath.size() > 1) {
                    // Skip the first node (current node)
                    for (size_t i = 1; i < remainingPath.size(); i++) {
                        path.push_back(remainingPath[i]);
                    }
                }
                
                break;
            }
            
            path.push_back(nextNode);
            visited[nextNode] = true;
            current = nextNode;
        }
        
        return path;
    }
    
    // Optimize a route through multiple locations using approximate TSP
    vector<int> optimizeRoute(int start, const vector<int>& locationsToVisit) {
        if (algorithm == "tsp") {
            return approximateTSP(start, locationsToVisit);
        } else if (algorithm == "nearest_neighbor") {
            return greedyNearestNeighbor(start, locationsToVisit);
        } else {
            // Default to greedy nearest neighbor
            return greedyNearestNeighbor(start, locationsToVisit);
        }
    }
    
    // Greedy Nearest Neighbor for route optimization
    vector<int> greedyNearestNeighbor(int start, const vector<int>& locationsToVisit) {
        vector<int> route = {start};
        int currentLocation = start;
        vector<bool> visited(locationsToVisit.size(), false);
        
        // Visit all locations
        while (true) {
            double bestDist = numeric_limits<double>::max();
            int bestIndex = -1;
            vector<int> bestPath;
            
            // Find the nearest unvisited location
            for (size_t i = 0; i < locationsToVisit.size(); i++) {
                if (!visited[i]) {
                    vector<int> path = modifiedDijkstra(currentLocation, locationsToVisit[i]);
                    
                    if (!path.empty() && path.size() > 1) {  // Valid path exists
                        double distance = path.size() - 1;  // Simple distance measure
                        
                        if (distance < bestDist) {
                            bestDist = distance;
                            bestIndex = i;
                            bestPath = path;
                        }
                    }
                }
            }
            
            if (bestIndex == -1) {
                // No more reachable unvisited locations
                break;
            }
            
            // Add all intermediate nodes to the route (excluding the first, which is current location)
            for (size_t i = 1; i < bestPath.size(); i++) {
                route.push_back(bestPath[i]);
            }
            
            // Update current location and mark as visited
            currentLocation = locationsToVisit[bestIndex];
            visited[bestIndex] = true;
            
            // Check if all locations have been visited
            bool allVisited = true;
            for (bool v : visited) {
                if (!v) {
                    allVisited = false;
                    break;
                }
            }
            
            if (allVisited) {
                break;
            }
        }
        
        // Calculate total distance for statistics
        double totalDistance = 0.0;
        for (size_t i = 1; i < route.size(); i++) {
            int from = route[i-1];
            int to = route[i];
            
            const auto& adjList = campus->getAdjacencyList();
            for (const auto& edge : adjList[from]) {
                if (edge.getTo() == to) {
                    totalDistance += edge.getDistance();
                    break;
                }
            }
        }
        
        campus->getStatistics().addDistanceTraveled(totalDistance);
        
        return route;
    }
    
    // Approximate TSP using 2-Opt local search
    vector<int> approximateTSP(int start, const vector<int>& locationsToVisit) {
        // First, generate an initial solution using greedy nearest neighbor
        vector<int> route = greedyNearestNeighbor(start, locationsToVisit);
        
        // If the route is too short, no need for optimization
        if (route.size() <= 3) {
            return route;
        }
        
        // Apply 2-opt local search
        bool improved = true;
        int iterations = 0;
        int maxIterations = 100;  // Limit iterations
        
        while (improved && iterations < maxIterations) {
            improved = false;
            iterations++;
            
            for (size_t i = 1; i < route.size() - 2; i++) {
                for (size_t j = i + 1; j < route.size() - 1; j++) {
                    // Try swapping edges (i-1,i) and (j,j+1) with (i-1,j) and (i,j+1)
                    double currentDist = pathDistance(route[i-1], route[i]) + pathDistance(route[j], route[j+1]);
                    double newDist = pathDistance(route[i-1], route[j]) + pathDistance(route[i], route[j+1]);
                    
                    if (newDist < currentDist) {
                        // Reverse the route between i and j
                        reverse(route.begin() + i, route.begin() + j + 1);
                        improved = true;
                    }
                }
            }
        }
        
        return route;
    }
    
    // Helper method to calculate distance between two locations
    double pathDistance(int from, int to) {
        vector<int> path = modifiedDijkstra(from, to);
        if (path.empty()) {
            return numeric_limits<double>::max();
        }
        
        double distance = 0.0;
        for (size_t i = 1; i < path.size(); i++) {
            int pathFrom = path[i-1];
            int pathTo = path[i];
            
            const auto& adjList = campus->getAdjacencyList();
            for (const auto& edge : adjList[pathFrom]) {
                if (edge.getTo() == pathTo) {
                    distance += edge.getDistance();
                    break;
                }
            }
        }
        
        return distance;
    }
}; 