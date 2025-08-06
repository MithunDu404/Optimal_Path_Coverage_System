#include "campus_manager_part3.cpp"

// UI class for user interaction
class UI {
private:
    CampusMap campus;
    ConfigManager config;
    RouteOptimizer routeOptimizer;
    MaintenanceScheduler scheduler;
    Logger logger;
    
    // Flag for user authentication
    bool authenticated;
    string username;
    
public:
    UI() : routeOptimizer(&campus, config.getValue("algorithm"), 
                         config.getDoubleValue("alpha_weight"), 
                         config.getDoubleValue("beta_weight"),
                         config.getDoubleValue("gamma_weight"),
                         config.getDoubleValue("delta_weight")),
          scheduler(&campus, &routeOptimizer, &config),
          authenticated(false) {
        
        // Load campus data from backup if available, otherwise from default
        if (!campus.loadFromFile(BACKUP_FILE)) {
            if (campus.loadFromFile(DATA_FILE)) {
                campus.backupData();
            } else {
                // Create default data if no files exist
                createDefaultData();
            }
        }
        
        // Load statistics
        campus.getStatistics().loadFromFile();
        
        logger.log("System initialized");
    }
    
    // Create default data if no files exist
    void createDefaultData() {
        logger.log("Creating default campus data");
        
        // Add default locations
        campus.addLocation(Location(0, "Library", 9, 3, 8, 100.0, 
                                  "Central campus library with study areas", "Academic"));
        campus.addLocation(Location(1, "Main Building", 10, 2, 9, 100.0, 
                                  "Administrative headquarters", "Administrative", true));
        campus.addLocation(Location(2, "Science Lab", 8, 4, 7, 100.0, 
                                  "Laboratories for physics and chemistry", "Academic"));
        campus.addLocation(Location(3, "Student Center", 7, 2, 8, 100.0, 
                                  "Student activities and services center", "Services"));
        campus.addLocation(Location(4, "Cafeteria", 6, 1, 10, 95.0, 
                                  "Main campus dining hall", "Services", true));
        campus.addLocation(Location(5, "Administration", 7, 5, 6, 100.0, 
                                  "Faculty offices and meeting rooms", "Administrative"));
        campus.addLocation(Location(6, "Sports Complex", 5, 3, 7, 100.0, 
                                  "Gymnasium and athletic facilities", "Recreational"));
        campus.addLocation(Location(7, "Research Center", 8, 7, 5, 100.0, 
                                  "Advanced research facilities", "Academic", true));
        campus.addLocation(Location(8, "Garden", 4, 14, 3, 100.0, 
                                  "Campus garden and outdoor seating", "Recreational"));
        campus.addLocation(Location(9, "Dormitory A", 6, 7, 5, 100.0, 
                                  "Student housing block A", "Residential"));
        campus.addLocation(Location(10, "Dormitory B", 6, 7, 5, 100.0, 
                                  "Student housing block B", "Residential"));
        campus.addLocation(Location(11, "Parking Lot", 3, 10, 4, 100.0, 
                                  "Main campus parking area", "Services"));
        
        // Add default paths
        campus.addPath(Path(0, 1, 3.0, 5.0, 1.0, true));
        campus.addPath(Path(0, 2, 4.0, 6.0, 1.2));
        campus.addPath(Path(0, 3, 2.0, 4.0, 1.0, true));
        campus.addPath(Path(1, 0, 3.0, 5.0, 1.0, true));
        campus.addPath(Path(1, 3, 2.5, 5.0, 1.0));
        campus.addPath(Path(1, 4, 3.0, 5.5, 1.1, true));
        campus.addPath(Path(1, 5, 2.0, 4.0, 1.0, true, true, "Secure administrative corridor"));
        campus.addPath(Path(2, 0, 4.0, 6.0, 1.2));
        campus.addPath(Path(2, 6, 3.5, 5.0, 1.1));
        campus.addPath(Path(3, 0, 2.0, 4.0, 1.0, true));
        campus.addPath(Path(3, 1, 2.5, 5.0, 1.0));
        campus.addPath(Path(4, 1, 3.0, 5.5, 1.1, true));
        campus.addPath(Path(5, 1, 2.0, 4.0, 1.0, true, true, "Secure administrative corridor"));
        campus.addPath(Path(5, 7, 4.0, 6.0, 1.2, false, true, "Research access corridor"));
        campus.addPath(Path(6, 2, 3.5, 5.0, 1.1));
        campus.addPath(Path(7, 5, 4.0, 6.0, 1.2, false, true, "Research access corridor"));
        campus.addPath(Path(7, 8, 3.0, 5.0, 1.3));
        campus.addPath(Path(8, 7, 3.0, 5.0, 1.3));
        campus.addPath(Path(8, 9, 5.0, 7.0, 1.4));
        campus.addPath(Path(9, 8, 5.0, 7.0, 1.4));
        campus.addPath(Path(9, 11, 3.0, 4.0, 1.0));
        campus.addPath(Path(10, 11, 2.0, 3.0, 1.0));
        campus.addPath(Path(11, 9, 3.0, 4.0, 1.0));
        campus.addPath(Path(11, 10, 2.0, 3.0, 1.0));
        
        // Save the default data
        campus.saveToFile(DATA_FILE);
        campus.backupData();
    }
    
    // Run the main menu loop
    void run() {
        bool running = true;
        
        // Login prompt
        if (!login()) {
            cout << "Access denied. Exiting program.\n";
            return;
        }
        
        while (running) {
            displayMainMenu();
            int choice = getIntInput("Enter your choice: ");
            
            switch (choice) {
                case 1:
                    findOptimalPath();
                    break;
                    
                case 2:
                    viewCampusStatus();
                    break;
                    
                case 3:
                    runSimulation();
                    break;
                    
                case 4:
                    locationManagement();
                    break;
                    
                case 5:
                    pathManagement();
                    break;
                    
                case 6:
                    configurationMenu();
                    break;
                    
                case 7:
                    viewStatistics();
                    break;
                    
                case 8:
                    resetToDefault();
                    break;
                    
                case 9:
                    campus.backupData();
                    campus.getStatistics().saveToFile();
                    cout << "All data saved. Exiting program.\n";
                    running = false;
                    break;
                    
                default:
                    cout << "Invalid choice. Please try again.\n";
                    break;
            }
        }
    }
    
private:
    // Display the main menu
    void displayMainMenu() {
        clearScreen();
        cout << "=== Campus Maintenance Management System ===\n";
        cout << "Welcome, " << username << "!\n\n";
        cout << "1. Find optimal path between locations\n";
        cout << "2. View campus status\n";
        cout << "3. Run maintenance simulation\n";
        cout << "4. Location management\n";
        cout << "5. Path management\n";
        cout << "6. Configuration settings\n";
        cout << "7. View statistics\n";
        cout << "8. Reset to default configuration\n";
        cout << "9. Exit\n";
    }
    
    // Clear the screen (platform-dependent)
    void clearScreen() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }
    
    // Wait for user to press Enter
    void waitForEnter() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getchar();
    }
    
    // Get integer input with validation
    int getIntInput(const string& prompt) {
        int value;
        string input;
        
        while (true) {
            cout << prompt;
            getline(cin, input);
            
            stringstream ss(input);
            if (ss >> value && ss.eof()) {
                return value;
            }
            
            cout << "Invalid input. Please enter a number.\n";
        }
    }
    
    // Get double input with validation
    double getDoubleInput(const string& prompt) {
        double value;
        string input;
        
        while (true) {
            cout << prompt;
            getline(cin, input);
            
            stringstream ss(input);
            if (ss >> value && ss.eof()) {
                return value;
            }
            
            cout << "Invalid input. Please enter a number.\n";
        }
    }
    
    // Get string input
    string getStringInput(const string& prompt) {
        string input;
        cout << prompt;
        getline(cin, input);
        return input;
    }
    
    // User login function
    bool login() {
        clearScreen();
        cout << "=== Campus Maintenance Management System ===\n";
        cout << "Please log in to continue\n\n";
        
        // Simple login mechanism (can be expanded with actual user database)
        username = getStringInput("Username: ");
        string password = getStringInput("Password: ");
        
        // For demo purposes, accept any non-empty username/password
        if (!username.empty() && !password.empty()) {
            logger.log("User logged in: " + username);
            authenticated = true;
            return true;
        }
        
        logger.log("Failed login attempt with username: " + username);
        return false;
    }
    
    // Find optimal path between two locations
    void findOptimalPath() {
        clearScreen();
        cout << "=== Find Optimal Path ===\n\n";
        
        // Display available locations
        cout << "Available locations:\n";
        const auto& locations = campus.getLocations();
        for (const auto& loc : locations) {
            cout << loc.getId() << ": " << loc.getName() << "\n";
        }
        
        // Get source and destination
        int sourceId = getIntInput("\nEnter source location ID: ");
        int destId = getIntInput("Enter destination location ID: ");
        
        // Validate IDs
        Location* sourceLoc = campus.getLocationById(sourceId);
        Location* destLoc = campus.getLocationById(destId);
        
        if (!sourceLoc || !destLoc) {
            cout << "Invalid location ID(s). Please try again.\n";
            waitForEnter();
            return;
        }
        
        // Find path
        vector<int> path = routeOptimizer.findPath(sourceId, destId);
        
        cout << "\nOptimal path found:\n";
        scheduler.displayPath(path);
        
        waitForEnter();
    }
    
    // View campus status
    void viewCampusStatus() {
        clearScreen();
        cout << "=== Campus Status ===\n\n";
        
        campus.printLocationsStatus();
        
        cout << "\nOptions:\n";
        cout << "1. View detailed information for a location\n";
        cout << "2. Return to main menu\n";
        
        int choice = getIntInput("Enter your choice: ");
        
        if (choice == 1) {
            int locId = getIntInput("Enter location ID for detailed view: ");
            campus.printLocationDetail(locId);
        }
        
        waitForEnter();
    }
    
    // Run maintenance simulation
    void runSimulation() {
        clearScreen();
        cout << "=== Run Maintenance Simulation ===\n\n";
        
        int days = getIntInput("Enter number of days to simulate (1-30): ");
        
        if (days <= 0 || days > 30) {
            cout << "Invalid number of days. Please enter a value between 1 and 30.\n";
            waitForEnter();
            return;
        }
        
        cout << "Available locations for starting point:\n";
        const auto& locations = campus.getLocations();
        for (const auto& loc : locations) {
            cout << loc.getId() << ": " << loc.getName() << "\n";
        }
        
        int startLocation = getIntInput("Enter starting location ID: ");
        
        // Validate ID
        if (campus.getLocationById(startLocation) == nullptr) {
            cout << "Invalid location ID. Using default (0).\n";
            startLocation = 0;
        }
        
        scheduler.runSimulation(days, startLocation);
        
        waitForEnter();
    }
    
    // Location management submenu
    void locationManagement() {
        while (true) {
            clearScreen();
            cout << "=== Location Management ===\n\n";
            cout << "1. View all locations\n";
            cout << "2. Add new location\n";
            cout << "3. Edit existing location\n";
            cout << "4. Remove location\n";
            cout << "5. Return to main menu\n";
            
            int choice = getIntInput("Enter your choice: ");
            
            switch (choice) {
                case 1:
                    viewAllLocations();
                    break;
                    
                case 2:
                    addNewLocation();
                    break;
                    
                case 3:
                    editLocation();
                    break;
                    
                case 4:
                    removeLocation();
                    break;
                    
                case 5:
                    return;
                    
                default:
                    cout << "Invalid choice. Please try again.\n";
                    waitForEnter();
                    break;
            }
        }
    }
    
    // View all locations
    void viewAllLocations() {
        clearScreen();
        cout << "=== All Locations ===\n\n";
        
        const auto& locations = campus.getLocations();
        for (const auto& loc : locations) {
            cout << "ID: " << loc.getId() << "\n";
            cout << "Name: " << loc.getName() << "\n";
            cout << "Category: " << loc.getCategory() << "\n";
            cout << "Importance: " << loc.getImportance() << "\n";
            cout << "Cleaning Frequency: " << loc.getCleaningFrequency() << " days\n";
            cout << "Special Care Required: " << (loc.getNeedsSpecialCare() ? "Yes" : "No") << "\n";
            cout << "Description: " << loc.getDescription() << "\n";
            cout << string(30, '-') << "\n";
        }
        
        waitForEnter();
    }
    
    // Add new location
    void addNewLocation() {
        clearScreen();
        cout << "=== Add New Location ===\n\n";
        
        int id = getIntInput("Enter ID for new location: ");
        
        // Check if ID already exists
        if (campus.getLocationById(id) != nullptr) {
            cout << "A location with this ID already exists.\n";
            waitForEnter();
            return;
        }
        
        string name = getStringInput("Enter name: ");
        string category = getStringInput("Enter category: ");
        string description = getStringInput("Enter description: ");
        int importance = getIntInput("Enter importance (1-10): ");
        int cleaningFreq = getIntInput("Enter cleaning frequency (days): ");
        int visitPriority = getIntInput("Enter visit priority (1-10): ");
        
        string specialCareInput = getStringInput("Requires special care? (y/n): ");
        bool needsSpecialCare = (specialCareInput == "y" || specialCareInput == "Y");
        
        Location newLoc(id, name, importance, cleaningFreq, visitPriority, 100.0, 
                       description, category, needsSpecialCare);
        
        campus.addLocation(newLoc);
        cout << "Location added successfully.\n";
        
        waitForEnter();
    }
    
    // Edit existing location
    void editLocation() {
        clearScreen();
        cout << "=== Edit Location ===\n\n";
        
        cout << "Available locations:\n";
        const auto& locations = campus.getLocations();
        for (const auto& loc : locations) {
            cout << loc.getId() << ": " << loc.getName() << "\n";
        }
        
        int id = getIntInput("\nEnter ID of location to edit: ");
        
        Location* loc = campus.getLocationById(id);
        if (loc == nullptr) {
            cout << "Location not found.\n";
            waitForEnter();
            return;
        }
        
        cout << "\nEditing location: " << loc->getName() << "\n";
        cout << "Leave fields blank to keep current values.\n\n";
        
        string name = getStringInput("New name [" + loc->getName() + "]: ");
        string category = getStringInput("New category [" + loc->getCategory() + "]: ");
        string description = getStringInput("New description [" + loc->getDescription() + "]: ");
        
        string importanceStr = getStringInput("New importance (1-10) [" + to_string(loc->getImportance()) + "]: ");
        string cleaningFreqStr = getStringInput("New cleaning frequency (days) [" + to_string(loc->getCleaningFrequency()) + "]: ");
        string visitPriorityStr = getStringInput("New visit priority (1-10) [" + to_string(loc->getVisitPriority()) + "]: ");
        
        string specialCareInput = getStringInput(string("Requires special care? (y/n) [") + 
                                               (loc->getNeedsSpecialCare() ? "y" : "n") + "]: ");
        
        // Apply changes (only for non-empty inputs)
        Location newLoc = *loc;
        if (!name.empty()) newLoc = Location(loc->getId(), name, loc->getImportance(), 
                                          loc->getCleaningFrequency(), loc->getVisitPriority(), 
                                          loc->getCleanlinessStatus(), loc->getDescription(), 
                                          loc->getCategory(), loc->getNeedsSpecialCare());
        
        if (!category.empty()) newLoc.setCategory(category);
        if (!description.empty()) newLoc.setDescription(description);
        
        if (!importanceStr.empty()) {
            try {
                int importance = stoi(importanceStr);
                newLoc = Location(loc->getId(), newLoc.getName(), importance, 
                               newLoc.getCleaningFrequency(), newLoc.getVisitPriority(), 
                               newLoc.getCleanlinessStatus(), newLoc.getDescription(), 
                               newLoc.getCategory(), newLoc.getNeedsSpecialCare());
            } catch (...) {}
        }
        
        if (!cleaningFreqStr.empty()) {
            try {
                int cleaningFreq = stoi(cleaningFreqStr);
                newLoc = Location(loc->getId(), newLoc.getName(), newLoc.getImportance(), 
                               cleaningFreq, newLoc.getVisitPriority(), 
                               newLoc.getCleanlinessStatus(), newLoc.getDescription(), 
                               newLoc.getCategory(), newLoc.getNeedsSpecialCare());
            } catch (...) {}
        }
        
        if (!visitPriorityStr.empty()) {
            try {
                int visitPriority = stoi(visitPriorityStr);
                newLoc = Location(loc->getId(), newLoc.getName(), newLoc.getImportance(), 
                               newLoc.getCleaningFrequency(), visitPriority, 
                               newLoc.getCleanlinessStatus(), newLoc.getDescription(), 
                               newLoc.getCategory(), newLoc.getNeedsSpecialCare());
            } catch (...) {}
        }
        
        if (!specialCareInput.empty()) {
            newLoc.setNeedsSpecialCare(specialCareInput == "y" || specialCareInput == "Y");
        }
        
        // Preserve cleanliness status and last cleaned days
        newLoc.setCleanlinessStatus(loc->getCleanlinessStatus());
        newLoc.setLastCleaned(loc->getLastCleaned());
        
        campus.addLocation(newLoc);
        cout << "Location updated successfully.\n";
        
        waitForEnter();
    }
    
    // Remove location
    void removeLocation() {
        // This is a placeholder - removing locations would require more complex changes to the adjacency list
        cout << "Feature not yet implemented.\n";
        waitForEnter();
    }
    
    // Path management submenu
    void pathManagement() {
        // Similar structure to locationManagement
        cout << "Feature not yet implemented.\n";
        waitForEnter();
    }
    
    // Configuration settings submenu
    void configurationMenu() {
        while (true) {
            clearScreen();
            cout << "=== Configuration Settings ===\n\n";
            cout << "1. View current settings\n";
            cout << "2. Change routing algorithm\n";
            cout << "3. Change algorithm weights\n";
            cout << "4. Change daily locations to visit\n";
            cout << "5. Return to main menu\n";
            
            int choice = getIntInput("Enter your choice: ");
            
            switch (choice) {
                case 1:
                    viewSettings();
                    break;
                    
                case 2:
                    changeAlgorithm();
                    break;
                    
                case 3:
                    changeWeights();
                    break;
                    
                case 4:
                    changeLocationsPerDay();
                    break;
                    
                case 5:
                    return;
                    
                default:
                    cout << "Invalid choice. Please try again.\n";
                    waitForEnter();
                    break;
            }
        }
    }
    
    // View current settings
    void viewSettings() {
        clearScreen();
        cout << "=== Current Settings ===\n\n";
        
        cout << "Routing Algorithm: " << config.getValue("algorithm") << "\n";
        cout << "Alpha Weight (Distance): " << config.getDoubleValue("alpha_weight") << "\n";
        cout << "Beta Weight (Difficulty): " << config.getDoubleValue("beta_weight") << "\n";
        cout << "Gamma Weight (Visit History): " << config.getDoubleValue("gamma_weight") << "\n";
        cout << "Delta Weight (Visit Count): " << config.getDoubleValue("delta_weight") << "\n";
        cout << "Default Start Location: " << config.getIntValue("default_start_location") << "\n";
        cout << "Locations to Visit Per Day: " << config.getIntValue("locations_per_day") << "\n";
        cout << "Default Simulation Days: " << config.getIntValue("simulation_days") << "\n";
        
        waitForEnter();
    }
    
    // Change routing algorithm
    void changeAlgorithm() {
        clearScreen();
        cout << "=== Change Routing Algorithm ===\n\n";
        cout << "Available algorithms:\n";
        cout << "1. modified_dijkstra - Modified Dijkstra's algorithm\n";
        cout << "2. nearest_neighbor - Nearest Neighbor algorithm\n";
        cout << "3. tsp - Approximate TSP algorithm\n";
        
        int choice = getIntInput("\nSelect algorithm (1-3): ");
        
        string algorithm;
        switch (choice) {
            case 1:
                algorithm = "modified_dijkstra";
                break;
            case 2:
                algorithm = "nearest_neighbor";
                break;
            case 3:
                algorithm = "tsp";
                break;
            default:
                cout << "Invalid choice. Keeping current algorithm.\n";
                waitForEnter();
                return;
        }
        
        config.setValue("algorithm", algorithm);
        config.saveToFile();
        routeOptimizer.setAlgorithm(algorithm);
        
        cout << "Algorithm updated to " << algorithm << "\n";
        waitForEnter();
    }
    
    // Change algorithm weights
    void changeWeights() {
        clearScreen();
        cout << "=== Change Algorithm Weights ===\n\n";
        cout << "Current weights:\n";
        cout << "Alpha (Distance): " << config.getDoubleValue("alpha_weight") << "\n";
        cout << "Beta (Difficulty): " << config.getDoubleValue("beta_weight") << "\n";
        cout << "Gamma (Visit History): " << config.getDoubleValue("gamma_weight") << "\n";
        cout << "Delta (Visit Count): " << config.getDoubleValue("delta_weight") << "\n\n";
        
        double alpha = getDoubleInput("Enter new Alpha weight: ");
        double beta = getDoubleInput("Enter new Beta weight: ");
        double gamma = getDoubleInput("Enter new Gamma weight: ");
        double delta = getDoubleInput("Enter new Delta weight: ");
        
        // Normalize weights to ensure they sum to 1
        double sum = alpha + beta + gamma;
        if (sum > 0) {
            alpha /= sum;
            beta /= sum;
            gamma /= sum;
        } else {
            alpha = 0.6;
            beta = 0.3;
            gamma = 0.1;
        }
        
        config.setValue("alpha_weight", to_string(alpha));
        config.setValue("beta_weight", to_string(beta));
        config.setValue("gamma_weight", to_string(gamma));
        config.setValue("delta_weight", to_string(delta));
        config.saveToFile();
        
        routeOptimizer.setWeights(alpha, beta, gamma, delta);
        
        cout << "Weights updated successfully.\n";
        waitForEnter();
    }
    
    // Change locations per day
    void changeLocationsPerDay() {
        clearScreen();
        cout << "=== Change Locations Per Day ===\n\n";
        cout << "Current setting: " << config.getIntValue("locations_per_day") << "\n\n";
        
        int locationsPerDay = getIntInput("Enter new number of locations to visit per day: ");
        
        if (locationsPerDay <= 0) {
            cout << "Invalid value. Must be greater than 0.\n";
            waitForEnter();
            return;
        }
        
        config.setValue("locations_per_day", to_string(locationsPerDay));
        config.saveToFile();
        
        cout << "Setting updated successfully.\n";
        waitForEnter();
    }
    
    // View statistics
    void viewStatistics() {
        clearScreen();
        cout << "=== Maintenance Statistics ===\n\n";
        
        Statistics& stats = campus.getStatistics();
        
        cout << "Total Days Simulated: " << stats.getVisitCount(0) << "\n";
        cout << "Total Locations Visited: " << stats.getVisitCount(0) << "\n";
        cout << "Average Visits Per Day: " << fixed << setprecision(2) << stats.getAverageVisitsPerDay() << "\n";
        cout << "Average Distance Per Day: " << fixed << setprecision(2) << stats.getAverageDistancePerDay() << " units\n\n";
        
        cout << "Visit Counts by Location:\n";
        cout << string(40, '-') << "\n";
        cout << setw(30) << left << "Location" << setw(10) << right << "Visits" << "\n";
        cout << string(40, '-') << "\n";
        
        const auto& locations = campus.getLocations();
        for (const auto& loc : locations) {
            cout << setw(30) << left << loc.getName() 
                 << setw(10) << right << stats.getVisitCount(loc.getId()) << "\n";
        }
        
        waitForEnter();
    }
    
    // Reset to default
    void resetToDefault() {
        clearScreen();
        cout << "=== Reset to Default ===\n\n";
        cout << "Warning: This will reset all campus data to default values.\n";
        string confirm = getStringInput("Are you sure? (y/n): ");
        
        if (confirm == "y" || confirm == "Y") {
            campus.resetToDefault();
            cout << "Campus data reset to default configuration.\n";
        } else {
            cout << "Reset cancelled.\n";
        }
        
        waitForEnter();
    }
};

// Note: main() function removed to avoid duplication with main.cpp 