#include "campus_manager_part2.cpp"

// MaintenanceScheduler for generating optimized daily routes
class MaintenanceScheduler {
private:
    CampusMap* campus;
    RouteOptimizer* routeOptimizer;
    ConfigManager* config;
    Logger logger;
    
public:
    MaintenanceScheduler(CampusMap* c, RouteOptimizer* r, ConfigManager* cfg) 
        : campus(c), routeOptimizer(r), config(cfg) {}
    
    // Generate optimized routes for a day
    vector<int> generateDailyRoutes(int startLocation) {
        // Update campus cleanliness status
        campus->updateCleanlinessStatus(1);
        campus->getStatistics().incrementDaysSimulated();
        
        logger.log("Generating daily route from location " + to_string(startLocation));
                
        // Get locations-priority for today
        vector<pair<int, double>> priorityList;
        const auto& locations = campus->getLocations();
        for (const auto& loc : locations) {
            double priority = campus->calculateDynamicPriority(loc.getId());
            priorityList.push_back({loc.getId(), priority});
        }
                
        // Sort by priority (descending)
        sort(priorityList.begin(), priorityList.end(), 
            [](const pair<int, double>& a, const pair<int, double>& b) {
            return a.second > b.second;
        });
        
        // Take top priorities (configurable portion of locations each day)
        int locationsPerDay = config->getIntValue("locations_per_day", 3);
        int locationsToVisit = max(1, min(static_cast<int>(locations.size()), locationsPerDay));
        
        vector<int> priorityLocations;
        for (int i = 0; i < min(locationsToVisit, static_cast<int>(priorityList.size())); i++) {
            priorityLocations.push_back(priorityList[i].first);
            logger.log("Selected high-priority location: " + 
                      campus->getLocationById(priorityList[i].first)->getName() + 
                      " (Priority: " + to_string(priorityList[i].second) + ")");
        }
                
        // Plan route to visit these locations
        vector<int> route = routeOptimizer->optimizeRoute(startLocation, priorityLocations);
                
        // Mark visited locations as cleaned
        for (int loc : route) {
            if (loc != startLocation) { // Don't clean the start location unless it's in the priority list
                bool isPriority = false;
                for (int priorityLoc : priorityLocations) {
                    if (priorityLoc == loc) {
                        isPriority = true;
                        break;
                    }
                }
                
                if (isPriority) {
                    campus->cleanLocation(loc);
                    logger.log("Cleaned location: " + campus->getLocationById(loc)->getName());
                }
            }
        }
        
        // Save statistics and data
        campus->getStatistics().saveToFile();
        campus->saveToFile(BACKUP_FILE);
                
        return route;
    }
    
    // Run a simulation for multiple days
    void runSimulation(int days, int startLocation) {
        logger.log("Starting simulation for " + to_string(days) + " days from location " + to_string(startLocation));
        
        cout << "\n=== Starting Campus Maintenance Simulation for " << days << " days ===\n";
        campus->printLocationsStatus();
        
        for (int day = 0; day < days; day++) {
            cout << "\n=== Day " << (day + 1) << " ===\n";
            vector<int> schedules = generateDailyRoutes(startLocation);
            displayPath(schedules);
            campus->printLocationsStatus();
        }
        
        // Print statistics summary
        printSimulationSummary();
    }
    
    // Display the path
    void displayPath(const vector<int>& path) {
        if (path.empty()) {
            cout << "No path found!\n";
            return;
        }
        
        cout << "Daily Maintenance Route:\n";
        for (size_t i = 0; i < path.size(); i++) {
            Location* loc = campus->getLocationById(path[i]);
            if (loc) {
                cout << loc->getName();
                if (i < path.size() - 1) {
                    cout << " -> ";
                }
            }
        }
        cout << "\n";
        
        // Print more details about the route
        cout << "\nDetailed Route Information:\n";
        cout << "-------------------------\n";
        double totalDistance = 0.0;
        
        for (size_t i = 1; i < path.size(); i++) {
            int from = path[i-1];
            int to = path[i];
            Location* fromLoc = campus->getLocationById(from);
            Location* toLoc = campus->getLocationById(to);
            
            if (fromLoc && toLoc) {
                // Find the path information
                const auto& adjList = campus->getAdjacencyList();
                for (const auto& edge : adjList[from]) {
                    if (edge.getTo() == to) {
                        cout << fromLoc->getName() << " to " << toLoc->getName() << ":\n";
                        cout << "  Distance: " << edge.getDistance() << " units\n";
                        cout << "  Travel Time: " << edge.getTravelTime() << " minutes\n";
                        cout << "  Path Type: " << (edge.getIsIndoor() ? "Indoor" : "Outdoor") << "\n";
                        
                        if (edge.getRequiresKey()) {
                            cout << "  Note: Requires access key\n";
                        }
                        
                        if (!edge.getDescription().empty()) {
                            cout << "  Description: " << edge.getDescription() << "\n";
                        }
                        
                        totalDistance += edge.getDistance();
                        break;
                    }
                }
                cout << "\n";
            }
        }
        
        cout << "Total Distance: " << totalDistance << " units\n";
        cout << "Total Locations Visited: " << path.size() << "\n";
    }
    
    // Print simulation summary
    void printSimulationSummary() {
        Statistics& stats = campus->getStatistics();
        
        cout << "\n=== Simulation Summary ===\n";
        cout << "Total Days: " << stats.getVisitCount(0) << "\n";
        cout << "Total Locations Visited: " << stats.getVisitCount(0) << "\n";
        cout << "Average Visits Per Day: " << fixed << setprecision(2) << stats.getAverageVisitsPerDay() << "\n";
        cout << "Average Distance Per Day: " << fixed << setprecision(2) << stats.getAverageDistancePerDay() << " units\n";
        
        // Most/least visited locations
        const auto& locations = campus->getLocations();
        int mostVisited = -1;
        int leastVisited = -1;
        int maxVisits = -1;
        int minVisits = INT_MAX;
        
        for (const auto& loc : locations) {
            int visits = stats.getVisitCount(loc.getId());
            
            if (visits > maxVisits) {
                maxVisits = visits;
                mostVisited = loc.getId();
            }
            
            if (visits < minVisits) {
                minVisits = visits;
                leastVisited = loc.getId();
            }
        }
        
        if (mostVisited != -1) {
            Location* loc = campus->getLocationById(mostVisited);
            if (loc) {
                cout << "Most Visited Location: " << loc->getName() << " (" << maxVisits << " visits)\n";
            }
        }
        
        if (leastVisited != -1) {
            Location* loc = campus->getLocationById(leastVisited);
            if (loc) {
                cout << "Least Visited Location: " << loc->getName() << " (" << minVisits << " visits)\n";
            }
        }
        
        // Average cleanliness
        double totalCleanliness = 0.0;
        for (const auto& loc : locations) {
            totalCleanliness += loc.getCleanlinessStatus();
        }
        
        cout << "Current Average Cleanliness: " << fixed << setprecision(2) 
             << (totalCleanliness / locations.size()) << "%\n\n";
    }
}; 