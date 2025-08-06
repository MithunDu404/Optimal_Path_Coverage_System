# Campus Maintenance Management System

A comprehensive system for managing and optimizing campus maintenance routes, cleaning schedules, and resource allocation.

## Features

- **Smart Route Planning**: Optimizes maintenance routes using advanced algorithms (Modified Dijkstra, Nearest Neighbor, TSP)
- **Location Management**: Add, edit, and manage campus locations with detailed attributes
- **Path Management**: Define connections between locations with distance, travel time, and accessibility properties
- **Dynamic Prioritization**: Calculates cleaning priorities based on multiple factors (importance, cleanliness, frequency)
- **Simulation**: Run multi-day simulations to test different maintenance strategies
- **Statistics**: Track and analyze maintenance performance with comprehensive statistics
- **User Authentication**: Basic user login system 
- **Configuration**: Customize algorithms, weights, and system parameters

## System Requirements

- C++ Compiler with C++11 support
- Standard Template Library (STL)

## Installation

1. Clone the repository
2. Compile the source code:
   ```
   g++ -o campus_manager main.cpp -std=c++11
   ```
3. Run the executable:
   ```
   ./campus_manager
   ```

## File Structure

- `campus_manager.cpp` - Core classes for Locations, Paths, and data management
- `campus_manager_part2.cpp` - CampusMap and RouteOptimizer implementations
- `campus_manager_part3.cpp` - MaintenanceScheduler implementation
- `campus_manager_part4.cpp` - UI and main program entry point
- `main.cpp` - Ties everything together with error handling
- `campus_data.txt` - Default campus data
- `campus_backup.txt` - Working copy of campus data
- `maintenance_stats.txt` - Statistics and historical data
- `maintenance_log.txt` - System logs
- `config.txt` - Configuration settings

## Usage

When you first run the program, you'll be prompted to log in. For demonstration purposes, any non-empty username and password will work.

The main menu provides access to all system features:

1. **Find optimal path**: Calculate the best route between two locations
2. **View campus status**: See cleanliness status and details for all locations
3. **Run simulation**: Perform multi-day maintenance simulations
4. **Location management**: Add, edit, and manage campus locations
5. **Path management**: Manage connections between locations
6. **Configuration**: Adjust system parameters and algorithms
7. **View statistics**: Analyze maintenance performance
8. **Reset to default**: Restore the system to default settings

## Improvements Over Original Version

The system has been completely redesigned with:

1. **Better Object-Oriented Design**: Classes with proper encapsulation and well-defined interfaces
2. **Enhanced Data Models**: More detailed location and path attributes (categories, descriptions, accessibility, etc.)
3. **Multiple Routing Algorithms**: Options for different optimization strategies
4. **Data Persistence**: Robust file I/O with serialization/deserialization
5. **Statistics**: Comprehensive tracking and analysis of maintenance performance
6. **Improved UI**: Intuitive menu system with user-friendly interactions
7. **Configuration**: Easily adjustable system parameters
8. **Error Handling**: Robust validation and error handling
9. **Logging**: System activity logging for monitoring and debugging

## Future Enhancements

- Visualization with graphical interface
- Multi-user support with role-based permissions
- Mobile app integration
- Weather-based scheduling
- Equipment and inventory tracking
- Work order management
- Maintenance staff scheduling
- Budget analysis and cost tracking

## License

This software is provided as-is under the MIT License. 