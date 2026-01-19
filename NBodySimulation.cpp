#include "NBodySimulation.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

// Default Constructor
NBodySimulation::NBodySimulation() 
    : tPlotDelta(0.1), tFinal(1.0), dt(0.01), bodies() { // Adjusted defaults
    
    // Simple 3-body setup for testing
    Body b;
    // Body 1
    b.x[0] = 0.0; b.x[1] = 0.0; b.x[2] = 0.0;
    b.v[0] = 1.0; b.v[1] = 0.0; b.v[2] = 0.0;
    b.mass = 1.0;
    bodies.push_back(b);

    // Body 2
    b.x[0] = 1.0; b.x[1] = 0.0; b.x[2] = 0.0;
    b.v[0] = 0.0; b.v[1] = 1.0; b.v[2] = 0.0;
    b.mass = 1.0;
    bodies.push_back(b);

    // Body 3
    b.x[0] = -1.0; b.x[1] = 0.0; b.x[2] = 0.0;
    b.v[0] = 0.0; b.v[1] = -1.0; b.v[2] = 0.0;
    b.mass = 1.0;
    bodies.push_back(b);
}

// File Loading Constructor
NBodySimulation::NBodySimulation(const std::string& filename)
    : tPlotDelta(0.0), tFinal(0.0), dt(0.0), bodies() {
    
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Cannot open input file: " << filename << "\n";
        exit(1); // Stop execution if file not found
    }

    // CORRECTED ORDER according to PDF 
    // 1. Time Step (dt)
    // 2. Final Time (tFinal)
    // 3. Snapshot Interval (tPlotDelta)
    if (!(infile >> dt >> tFinal >> tPlotDelta)) {
         std::cerr << "Error: Failed to read simulation parameters.\n";
         exit(1);
    }

    Body b;
    // Loop to read all particles
    while (infile >> b.x[0] >> b.x[1] >> b.x[2]
                  >> b.v[0] >> b.v[1] >> b.v[2]
                  >> b.mass) {
        bodies.push_back(b);
    }
    
    std::cout << "Loaded " << bodies.size() << " bodies from " << filename << "\n";
    std::cout << "dt=" << dt << ", tFinal=" << tFinal << ", plotDelta=" << tPlotDelta << "\n";
}