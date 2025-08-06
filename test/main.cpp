#include "campus_manager_part4.cpp"

int main() {
    try {
        UI ui;
        ui.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
} 