#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <random>
#include <sstream>
#include <climits>  // Added for INT_MAX

using namespace std;

// Constants
const string DATA_FILE = "campus_data.txt";
const string BACKUP_FILE = "campus_backup.txt";
const string CONFIG_FILE = "config.txt";
const string STATS_FILE = "maintenance_stats.txt";
const string LOG_FILE = "maintenance_log.txt";

// Forward declarations
class Location;
class Path;
class CampusMap;
class RouteOptimizer;
class MaintenanceScheduler;
class Logger;
class Statistics;
class ConfigManager;
class UI;

// Location class with improved features
class Location {
private:
    int id;
    string name;
    int importance;          // 1-10 scale
    int cleaningFrequency;   // Preferred days between cleanings
    int visitPriority;       // Base priority 1-10
    int lastCleaned;         // Days since last cleaned
    double cleanlinessStatus; // 0-100%
    string description;      // New field: description of the location
    string category;         // New field: category of location (academic, residential, etc.)
    bool needsSpecialCare;   // New field: requires special cleaning equipment

public:
    // Constructor
    Location(int id, string name, int importance, int cleaningFrequency, 
            int visitPriority, double cleanlinessStatus,
            string description = "", string category = "General", bool needsSpecialCare = false)
        : id(id), name(name), importance(importance), cleaningFrequency(cleaningFrequency),
          visitPriority(visitPriority), lastCleaned(0), cleanlinessStatus(cleanlinessStatus),
          description(description), category(category), needsSpecialCare(needsSpecialCare) {}
    
    // Getters
    int getId() const { return id; }
    const string& getName() const { return name; }
    int getImportance() const { return importance; }
    int getCleaningFrequency() const { return cleaningFrequency; }
    int getVisitPriority() const { return visitPriority; }
    int getLastCleaned() const { return lastCleaned; }
    double getCleanlinessStatus() const { return cleanlinessStatus; }
    const string& getDescription() const { return description; }
    const string& getCategory() const { return category; }
    bool getNeedsSpecialCare() const { return needsSpecialCare; }
    
    // Setters
    void setLastCleaned(int days) { lastCleaned = days; }
    void setCleanlinessStatus(double status) { 
        cleanlinessStatus = max(0.0, min(100.0, status)); 
    }
    void setDescription(const string& desc) { description = desc; }
    void setCategory(const string& cat) { category = cat; }
    void setNeedsSpecialCare(bool special) { needsSpecialCare = special; }
    
    // Methods
    void clean() {
        lastCleaned = 0;
        cleanlinessStatus = 100.0;
    }
    
    void updateCleanliness(int days) {
        lastCleaned += days;
        double decayRate = 100.0 / (cleaningFrequency * 2); // % loss per day
        cleanlinessStatus = max(0.0, cleanlinessStatus - (days * decayRate));
    }
    
    // Serialization methods
    string serialize() const {
        stringstream ss;
        ss << id << ","
           << name << ","
           << importance << ","
           << cleaningFrequency << ","
           << visitPriority << ","
           << cleanlinessStatus << ","
           << lastCleaned << ","
           << description << ","
           << category << ","
           << (needsSpecialCare ? "1" : "0");
        return ss.str();
    }
    
    static Location deserialize(const string& data) {
        stringstream ss(data);
        string token;
        vector<string> tokens;
        
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 7) {
            throw runtime_error("Invalid location data format");
        }
        
        int id = stoi(tokens[0]);
        string name = tokens[1];
        int importance = stoi(tokens[2]);
        int cleaningFrequency = stoi(tokens[3]);
        int visitPriority = stoi(tokens[4]);
        double cleanlinessStatus = stod(tokens[5]);
        int lastCleaned = stoi(tokens[6]);
        
        string description = (tokens.size() > 7) ? tokens[7] : "";
        string category = (tokens.size() > 8) ? tokens[8] : "General";
        bool needsSpecialCare = (tokens.size() > 9) ? (tokens[9] == "1") : false;
        
        Location loc(id, name, importance, cleaningFrequency, visitPriority, 
                   cleanlinessStatus, description, category, needsSpecialCare);
        loc.setLastCleaned(lastCleaned);
        
        return loc;
    }
};

// Path class with improved features
class Path {
private:
    int from, to;
    double distance;
    double travelTime;
    double difficulty;    // Factor representing road condition
    bool isIndoor;        // New field: is the path indoor?
    bool requiresKey;     // New field: does the path require access key?
    string description;   // New field: description of the path

public:
    // Constructor
    Path(int from, int to, double distance, double travelTime = 1.0, double difficulty = 1.0,
         bool isIndoor = false, bool requiresKey = false, string description = "")
        : from(from), to(to), distance(distance), travelTime(travelTime), difficulty(difficulty),
          isIndoor(isIndoor), requiresKey(requiresKey), description(description) {}
    
    // Getters
    int getFrom() const { return from; }
    int getTo() const { return to; }
    double getDistance() const { return distance; }
    double getTravelTime() const { return travelTime; }
    double getDifficulty() const { return difficulty; }
    bool getIsIndoor() const { return isIndoor; }
    bool getRequiresKey() const { return requiresKey; }
    const string& getDescription() const { return description; }
    
    // Serialization methods
    string serialize() const {
        stringstream ss;
        ss << from << ","
           << to << ","
           << distance << ","
           << travelTime << ","
           << difficulty << ","
           << (isIndoor ? "1" : "0") << ","
           << (requiresKey ? "1" : "0") << ","
           << description;
        return ss.str();
    }
    
    static Path deserialize(const string& data) {
        stringstream ss(data);
        string token;
        vector<string> tokens;
        
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 5) {
            throw runtime_error("Invalid path data format");
        }
        
        int from = stoi(tokens[0]);
        int to = stoi(tokens[1]);
        double distance = stod(tokens[2]);
        double travelTime = stod(tokens[3]);
        double difficulty = stod(tokens[4]);
        
        bool isIndoor = (tokens.size() > 5) ? (tokens[5] == "1") : false;
        bool requiresKey = (tokens.size() > 6) ? (tokens[6] == "1") : false;
        string description = (tokens.size() > 7) ? tokens[7] : "";
        
        return Path(from, to, distance, travelTime, difficulty, isIndoor, requiresKey, description);
    }
};

// Logger class for recording activities
class Logger {
private:
    string logFile;
    ofstream logStream;

public:
    Logger(const string& filename = LOG_FILE) : logFile(filename) {
        logStream.open(logFile, ios::app);
        if (!logStream) {
            cerr << "Warning: Could not open log file " << logFile << endl;
        }
    }
    
    ~Logger() {
        if (logStream.is_open()) {
            logStream.close();
        }
    }
    
    void log(const string& message) {
        // Get current time
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        
        if (logStream.is_open()) {
            logStream << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << " - " << message << endl;
        }
        
        // Also print to console for debug purposes
        #ifdef DEBUG
        cout << "LOG: " << message << endl;
        #endif
    }
};

// Statistics class for tracking maintenance metrics
class Statistics {
private:
    string statsFile;
    unordered_map<int, int> visitCounts;
    unordered_map<int, vector<double>> cleanlinessHistory;
    int totalDaysSimulated;
    int totalLocationsVisited;
    double totalDistanceTraveled;
    
public:
    Statistics(const string& filename = STATS_FILE) 
        : statsFile(filename), totalDaysSimulated(0), totalLocationsVisited(0), totalDistanceTraveled(0.0) {}
    
    void incrementVisitCount(int locationId) {
        visitCounts[locationId]++;
        totalLocationsVisited++;
    }
    
    void addCleanlinessRecord(int locationId, double cleanliness) {
        cleanlinessHistory[locationId].push_back(cleanliness);
    }
    
    void addDistanceTraveled(double distance) {
        totalDistanceTraveled += distance;
    }
    
    void incrementDaysSimulated(int days = 1) {
        totalDaysSimulated += days;
    }
    
    int getVisitCount(int locationId) const {
        if (visitCounts.find(locationId) != visitCounts.end()) {
            return visitCounts.at(locationId);
        }
        return 0;
    }
    
    double getAverageVisitsPerDay() const {
        if (totalDaysSimulated == 0) return 0.0;
        return static_cast<double>(totalLocationsVisited) / totalDaysSimulated;
    }
    
    double getAverageDistancePerDay() const {
        if (totalDaysSimulated == 0) return 0.0;
        return totalDistanceTraveled / totalDaysSimulated;
    }
    
    double getAverageCleanliness(int locationId) const {
        if (cleanlinessHistory.find(locationId) == cleanlinessHistory.end() || 
            cleanlinessHistory.at(locationId).empty()) {
            return 0.0;
        }
        
        const auto& history = cleanlinessHistory.at(locationId);
        double sum = 0.0;
        for (double value : history) {
            sum += value;
        }
        
        return sum / history.size();
    }
    
    void saveToFile() {
        ofstream outFile(statsFile);
        if (!outFile) {
            cerr << "Error opening statistics file for writing" << endl;
            return;
        }
        
        outFile << "# Statistics\n";
        outFile << "TotalDays," << totalDaysSimulated << "\n";
        outFile << "TotalVisits," << totalLocationsVisited << "\n";
        outFile << "TotalDistance," << totalDistanceTraveled << "\n";
        
        outFile << "# VisitCounts\n";
        for (const auto& pair : visitCounts) {
            outFile << pair.first << "," << pair.second << "\n";
        }
        
        outFile << "# CleanlinessHistory\n";
        for (const auto& pair : cleanlinessHistory) {
            outFile << pair.first;
            for (double value : pair.second) {
                outFile << "," << value;
            }
            outFile << "\n";
        }
        
        outFile.close();
    }
    
    bool loadFromFile() {
        ifstream inFile(statsFile);
        if (!inFile) {
            cerr << "Statistics file not found. Starting with new data.\n";
            return false;
        }
        
        visitCounts.clear();
        cleanlinessHistory.clear();
        totalDaysSimulated = 0;
        totalLocationsVisited = 0;
        totalDistanceTraveled = 0.0;
        
        string line;
        string currentSection;
        
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            
            // Check for section headers
            if (line[0] == '#') {
                currentSection = line;
                continue;
            }
            
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            try {
                if (currentSection == "# Statistics" && tokens.size() >= 2) {
                    if (tokens[0] == "TotalDays") {
                        totalDaysSimulated = stoi(tokens[1]);
                    } else if (tokens[0] == "TotalVisits") {
                        totalLocationsVisited = stoi(tokens[1]);
                    } else if (tokens[0] == "TotalDistance") {
                        totalDistanceTraveled = stod(tokens[1]);
                    }
                } else if (currentSection == "# VisitCounts" && tokens.size() >= 2) {
                    int locationId = stoi(tokens[0]);
                    int count = stoi(tokens[1]);
                    visitCounts[locationId] = count;
                } else if (currentSection == "# CleanlinessHistory" && tokens.size() >= 2) {
                    int locationId = stoi(tokens[0]);
                    vector<double> history;
                    
                    for (size_t i = 1; i < tokens.size(); i++) {
                        history.push_back(stod(tokens[i]));
                    }
                    
                    cleanlinessHistory[locationId] = history;
                }
            } catch (const exception& e) {
                cerr << "Error parsing line in statistics file: " << e.what() << endl;
            }
        }
        
        inFile.close();
        return true;
    }
};

// Configuration Manager class
class ConfigManager {
private:
    string configFile;
    unordered_map<string, string> config;
    
    // Default configuration values
    void setDefaults() {
        config["default_start_location"] = "0";
        config["simulation_days"] = "7";
        config["locations_per_day"] = "3";
        config["alpha_weight"] = "0.6";
        config["beta_weight"] = "0.3";
        config["gamma_weight"] = "0.1";
        config["delta_weight"] = "0.2";
        config["algorithm"] = "modified_dijkstra";  // Options: modified_dijkstra, tsp, nearest_neighbor
    }
    
public:
    ConfigManager(const string& filename = CONFIG_FILE) : configFile(filename) {
        setDefaults();
        loadFromFile();
    }
    
    string getValue(const string& key) const {
        if (config.find(key) != config.end()) {
            return config.at(key);
        }
        return "";
    }
    
    int getIntValue(const string& key, int defaultValue = 0) const {
        try {
            if (config.find(key) != config.end()) {
                return stoi(config.at(key));
            }
        } catch (...) {}
        return defaultValue;
    }
    
    double getDoubleValue(const string& key, double defaultValue = 0.0) const {
        try {
            if (config.find(key) != config.end()) {
                return stod(config.at(key));
            }
        } catch (...) {}
        return defaultValue;
    }
    
    void setValue(const string& key, const string& value) {
        config[key] = value;
    }
    
    bool saveToFile() {
        ofstream outFile(configFile);
        if (!outFile) {
            cerr << "Error opening configuration file for writing" << endl;
            return false;
        }
        
        outFile << "# Configuration\n";
        for (const auto& pair : config) {
            outFile << pair.first << "=" << pair.second << "\n";
        }
        
        outFile.close();
        return true;
    }
    
    bool loadFromFile() {
        ifstream inFile(configFile);
        if (!inFile) {
            cerr << "Configuration file not found. Using defaults.\n";
            return false;
        }
        
        string line;
        while (getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string key = line.substr(0, pos);
                string value = line.substr(pos + 1);
                config[key] = value;
            }
        }
        
        inFile.close();
        return true;
    }
};

// More classes will be defined in subsequent files 