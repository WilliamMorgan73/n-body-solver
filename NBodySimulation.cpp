#include "NBodySimulation.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// Constructor that reads the input file
NBodySimulation::NBodySimulation(const char* inputFile) {
    std::ifstream infile(inputFile);
    if (!infile.is_open()) {
        throw std::runtime_error("Error: Could not open input file.");
    }

    // Read the simulation parameters (Line 1)
    // Format: dt final_time snapshot_interval
    if (!(infile >> dt >> tFinal >> tPlotDelta)) {
         throw std::runtime_error("Error: Failed to read simulation parameters.");
    }

    // Read particles (Subsequent lines)
    // Format: x y z vx vy vz mass
    while (infile) {
        double rx, ry, rz, vx, vy, vz, m;
        // Try to read the next 7 numbers
        if (infile >> rx >> ry >> rz >> vx >> vy >> vz >> m) {
            Body b;
            b.x[0] = rx; b.x[1] = ry; b.x[2] = rz;
            b.v[0] = vx; b.v[1] = vy; b.v[2] = vz;
            b.mass = m;
            bodies.push_back(b);
        }
    }

    infile.close();
    
    // Safety check
    if (bodies.empty()) {
        std::cerr << "Warning: No bodies were loaded from the file!\n";
    }
}