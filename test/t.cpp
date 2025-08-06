
#include<bits/stdc++.h>
using namespace std;

const string DATA_FILE = "campus_data.txt";
const string BACKUP_FILE = "campus_backup.txt";

// Location class -> Informations of every Location
class Location {
public:
    int id;
    string name;
    int importance; // 1 - 10
    int cleaningFrequency; // Preffered Days between cleanings
    int visitPriority; // Base priority 1-10
    int lastCleaned; // Days since last cleaned
    double cleanlinessStatus; // 0-100%

    Location(int id, string name, int importance, int cleaningFrequency, 
            int visitPriority, double cleanlinessStatus)
        : id(id), name(name), importance(importance), cleaningFrequency(cleaningFrequency),
          visitPriority(visitPriority), lastCleaned(0), cleanlinessStatus(cleanlinessStatus) {}
};

// Path class -> representing connections between locations
class Path {
public:
    int from, to;
    double distance;
    double travelTime;
    double difficulty;  // Factor representing road condition

    Path(int from, int to, double distance, double travelTime = 1.0, double difficulty = 1.0)
        : from(from), to(to), distance(distance), travelTime(travelTime), difficulty(difficulty) {}
};

// CampusMap class -> manage locations and paths
class CampusMap {
private:
    vector<Location> locations;
    vector<vector<Path>> adjacencyList;
    unordered_map<int, int> visitCounts;

public:
    CampusMap(int n) { adjacencyList.resize(n); }

    void addLocation(Location loc) {
        locations.push_back(loc);
        visitCounts[loc.id] = 0;
    }

    void addPath(Path p) { adjacencyList[p.from].push_back(p); }

    Location* getLocationById(int id) {
        for (auto &loc : locations) {
            if (loc.id == id) {
                return &loc;
            }
        }
        return nullptr;
    }

    Location* getLocationByName(const string& name) {
        for (auto &loc : locations) {
            if (loc.name == name) {
                return &loc;
            }
        }
        return nullptr;
    }

    void updateCleanlinessStatus(int days) {
        for (auto &loc : locations) {
            loc.lastCleaned += days;
            // Cleanliness decreases over time
            double decayRate = 100.0 / (loc.cleaningFrequency * 2); // % loss per day
            loc.cleanlinessStatus = max(0.0, loc.cleanlinessStatus - (days * decayRate));
        }
    }

    void cleanLocation(int locId) {
        for (auto &loc : locations) {
            if (loc.id == locId) {
                loc.lastCleaned = 0;
                loc.cleanlinessStatus = 100.0;
                visitCounts[locId]++;
                break;
            }
        }
    }

    double calculateDynamicPriority(int locId) {
        for (const auto &loc : locations) {
            if (loc.id == locId) {
                // Calculate priority based on multiple factors
                double timeFactorNormalized = min(1.0, static_cast<double>(loc.lastCleaned) / loc.cleaningFrequency);
                if (loc.lastCleaned < loc.cleaningFrequency) timeFactorNormalized *= 0.2; // Penalize recently cleaned locations
                
                double priority = (loc.importance * 0.3) + 
                                 ((100 - loc.cleanlinessStatus) * 0.4) + 
                                 (loc.visitPriority * 0.1) + 
                                 (timeFactorNormalized * 0.2);
                return priority;
            }
        }
        return 0.0;
    }

    const vector<vector<Path>>& getAdjacencyList() const {
        return adjacencyList;
    }

    const vector<Location>& getLocations() const {
        return locations;
    }

    int getVisitCount(int locId) const {
        if (visitCounts.find(locId) != visitCounts.end()) {
            return visitCounts.at(locId);
        }
        return 0;
    }

    void printLocationsStatus() const {
        cout << "\n=== Campus Locations Status ===\n";
        cout << setw(30) << left << "Location" 
             << setw(14) << right << "Cleanliness" 
             << setw(16) << right << "Last Cleaned" 
             << setw(14) << right << "Priority" 
             << setw(13) << right << "Visits\n";
    
        cout << string(87, '-') << "\n";  
    
        for (const auto &loc : locations) {
            double priority = const_cast<CampusMap*>(this)->calculateDynamicPriority(loc.id);
            cout << setw(30) << left << loc.name 
                 << setw(9) << right << fixed << setprecision(2) << loc.cleanlinessStatus << " %"  
                 << setw(13) << right << loc.lastCleaned << " days"  
                 << setw(14) << right << fixed << setprecision(2) << priority
                 << setw(10) << right << visitCounts.at(loc.id) << "\n";
        }
        cout << "\n";
    }

    // New file handling methods
    bool saveToFile(const string& filename) {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << "Error opening file for writing: " << filename << endl;
            return false;
        }

        // Save locations
        outFile << "# Locations\n";
        for (const auto& loc : locations) {
            outFile << loc.id << ","
                    << loc.name << ","
                    << loc.importance << ","
                    << loc.cleaningFrequency << ","
                    << loc.visitPriority << ","
                    << loc.cleanlinessStatus << ","
                    << loc.lastCleaned << ","
                    << visitCounts[loc.id] << "\n";
        }

        // Save paths
        outFile << "# Paths\n";
        for (int from = 0; from < adjacencyList.size(); ++from) {
            for (const auto& path : adjacencyList[from]) {
                outFile << path.from << ","
                        << path.to << ","
                        << path.distance << ","
                        << path.travelTime << ","
                        << path.difficulty << "\n";
            }
        }

        outFile.close();
        return true;
    }

    bool loadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cerr << "Data file not found. Starting with new data.\n";
            return false;
        }
        locations.clear();
        visitCounts.clear();
        adjacencyList.clear();
        
        string line;
        string currentSection;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            // Check section headers
            if (line == "# Locations" || line == "# Paths") {
                currentSection = line;
                continue;
            }

            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, ',')) tokens.push_back(token);
            
            try {
                // Process locations
                if (currentSection == "# Locations" && tokens.size() == 8) {
                    Location loc(stoi(tokens[0]), tokens[1], stoi(tokens[2]), 
                                 stoi(tokens[3]), stoi(tokens[4]), stod(tokens[5]));
                    loc.lastCleaned = stoi(tokens[6]);
                    locations.push_back(loc);
                    visitCounts[loc.id] = stoi(tokens[7]);
                } 
                // Process paths
                else if (currentSection == "# Paths" && tokens.size() == 5) {
                    int from = stoi(tokens[0]);
                    Path p(from, stoi(tokens[1]), stod(tokens[2]), 
                           stod(tokens[3]), stod(tokens[4]));
                    
                    if (from >= adjacencyList.size()) adjacencyList.resize(from + 1);
                    adjacencyList[from].push_back(p);
                }
                else  cerr << "Invalid format in " << currentSection << ": " << line << endl;
            }
            catch (const exception& e) { cerr << "Error parsing line: " << e.what() << endl; }
        }
        
        inFile.close();
        return true;
    }
};

class ModifiedDijkstra {
    private:
        CampusMap* campus;
        double alpha, beta, gamma, delta; // Weight factors
    
    public:
        ModifiedDijkstra(CampusMap* c, double a = 0.6, double b = 0.3, double g = 0.1, double d = 0.2) 
            : campus(c), alpha(a), beta(b), gamma(g), delta(d) {}
    
        vector<int> findPath(int start, int end) {
            const auto& adjList = campus->getAdjacencyList();
            int n = adjList.size();
            vector<double> distances(n, INT_MAX);
            vector<int> parents(n, -1);
            distances[start] = 0;
    
            priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
            pq.push({0, start});
    
            while (!pq.empty()) {
                double cost = pq.top().first;
                int node = pq.top().second;
                pq.pop();
    
                if (cost > distances[node]) continue;
    
                for (const auto& edge : adjList[node]) {
                    int to = edge.to;
                    
                    // Calculate weighted edge cost
                    double priorityFactor = 1.0 - (campus->calculateDynamicPriority(to) / 100.0);
                    double visitFactor = 1.0 + (campus->getVisitCount(to) * delta);
                    
                    double weight = (alpha * edge.distance) + 
                                   (beta * edge.difficulty) + 
                                   (gamma * visitFactor);
                    
                    // Priority reduces the cost (more important locations are easier to include)
                    weight *= (2.0 - priorityFactor);
                    
                    double newCost = distances[node] + weight;
    
                    if (newCost < distances[to]) {
                        distances[to] = newCost;
                        parents[to] = node;
                        pq.push({newCost, to});
                    }
                }
            }
    
            vector<int> path;
            if (distances[end] == INT_MAX) {
                // No path exists
                return path;
            }
            
            for (int at = end; at != -1; at = parents[at]) {
                path.push_back(at);
            }
            reverse(path.begin(), path.end());
            return path;
        }
    };
    
    // Maintenance Scheduler for generating daily routes
    class MaintenanceScheduler {
    private:
        CampusMap* campus;
        ModifiedDijkstra* pathFinder;
        
    public:
        MaintenanceScheduler(CampusMap* c, ModifiedDijkstra* d) 
            : campus(c), pathFinder(d) {}
        
        vector<int> generateDailyRoutes(int startLocation) {
            // Update campus cleanliness status
            campus->updateCleanlinessStatus(1);
                
            // Get locations-priority for today
            vector<pair<int, double>> priorityList;
            const auto& locations = campus->getLocations();
            for (const auto& loc : locations) {
                double priority = campus->calculateDynamicPriority(loc.id);
                priorityList.push_back({loc.id, priority});
            }
                
            // Sort by priority (descending)
            sort(priorityList.begin(), priorityList.end(), 
                [](const pair<int, double>& a, const pair<int, double>& b) {
                return a.second > b.second;
            });
                
            // Take top priorities (about 1/3 of locations each day)
            int locationsToVisit = max(1, static_cast<int>(locations.size() / 3));
            vector<int> priorityLocations;
            for (int i = 0; i < min(locationsToVisit, static_cast<int>(priorityList.size())); i++) {
                priorityLocations.push_back(priorityList[i].first);
            }
                
            // Plan route to visit these locations
            vector<int> route = planRoute(startLocation, priorityLocations);
                
            // Mark visited locations as cleaned
            for (int loc : route) {
                campus->cleanLocation(loc);
            }
                
            return route;
        }
        
    private:
        // Helper function to plan a route visiting all specified destinations
        vector<int> planRoute(int start, vector<int> destinations) {
            vector<int> route = {start};
            int currentLocation = start;
            
            // Simple greedy approach: visit nearest unvisited high-priority location
            while (!destinations.empty()) {
                double bestDist = INT_MAX;
                int bestDest = -1;
                int bestIndex = -1;
                
                for (int i = 0; i < destinations.size(); i++) {
                    vector<int> path = pathFinder->findPath(currentLocation, destinations[i]);
                    
                    if (!path.empty() && path.size() > 1) {  // Valid path exists
                        double distance = path.size() - 1;  // Simple distance measure
                        
                        if (distance < bestDist) {
                            bestDist = distance;
                            bestDest = destinations[i];
                            bestIndex = i;
                        }
                    }
                }
                
                if (bestDest == -1) {
                    // No reachable destination
                    break;
                }
                
                // Get the path to the best destination
                vector<int> path = pathFinder->findPath(currentLocation, bestDest);
                
                // Add all intermediate nodes to the route (excluding the first, which is current location)
                for (int i = 1; i < path.size(); i++) {
                    route.push_back(path[i]);
                }
                
                // Update current location
                currentLocation = bestDest;
                
                // Remove the visited destination
                destinations.erase(destinations.begin() + bestIndex);
            }
            
            return route;
        }
};
    
    // Utility functions for simulation
    void displayPath(const vector<int>& path, CampusMap* campus) {
        if (path.empty()) {
            cout << "No path found!\n";
            return;
        }
        
        cout << "Path: ";
        for (int i = 0; i < path.size(); i++) {
            Location* loc = campus->getLocationById(path[i]);
            if (loc) {
                cout << loc->name;
                if (i < path.size() - 1) {
                    cout << " -> ";
                }
            }
        }
        cout << "\n";
    }
    
    void runSimulation(CampusMap* campus, ModifiedDijkstra* pathFinder, int days) {
        MaintenanceScheduler scheduler(campus, pathFinder);
        int startDepot = 0;  // Library is the starting point
        
        cout << "\n=== Starting Campus Maintenance Simulation for " << days << " days ===\n";
        campus->printLocationsStatus();
        
        for (int day = 0; day < days; day++) {
            cout << "\n=== Day " << (day + 1) << " ===\n";
            vector<int> schedules = scheduler.generateDailyRoutes(startDepot);
            displayPath(schedules, campus);
            campus->printLocationsStatus();
        }
    } 

int main() {
    CampusMap campus(0);
    bool loaded = campus.loadFromFile(BACKUP_FILE);
    int numLocations;

    ModifiedDijkstra pathFinder(&campus);

    bool running = true;
    while (running) {
        cout << "\nOptions:\n";
        cout << "1. Find optimal path between two locations\n";
        cout << "2. View campus status\n";
        cout << "3. Run simulation for multiple days\n";
        cout << "4. Reset to default configuration\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "\nAvailable locations:\n";
                const auto& locations = campus.getLocations();
                for (const auto& loc : locations) {
                    cout << loc.id << ": " << loc.name << "\n";
                }
                
                int sourceId, destId;
                cout << "Enter source location ID: ";
                cin >> sourceId;
                cout << "Enter destination location ID: ";
                cin >> destId;
                
                if (sourceId < 0 || sourceId >= numLocations || destId < 0 || destId >= numLocations) {
                    cout << "Invalid location ID(s). Please try again.\n";
                    break;
                }
                
                vector<int> path = pathFinder.findPath(sourceId, destId);
                cout << "\nOptimal path found: \n";
                displayPath(path, &campus);
                break;
            }

            case 2:
                campus.printLocationsStatus();
                break;

            case 3: {
                int days;
                cout << "Enter number of days to simulate: ";
                cin >> days;
                
                if (days <= 0 || days > 30) {
                    cout << "Invalid number of days. Please enter a value between 1 and 30.\n";
                    break;
                }
                
                runSimulation(&campus, &pathFinder, days);
                break;
            }

            case 4: {
                // Copy the Default Campus data to Backup data
                ifstream source(DATA_FILE, ios::binary);
                ofstream dest(BACKUP_FILE, ios::binary);
                if (!source || !dest) {
                    cerr << "Error opening files!" << endl;
                    return 0;
                }
                dest << source.rdbuf();
                source.close();
                dest.close();


                campus.loadFromFile(BACKUP_FILE);
                break;
            }

            case 5:
                campus.saveToFile(BACKUP_FILE);
                running = false;
                break;
                
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}
