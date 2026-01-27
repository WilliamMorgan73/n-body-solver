#include "NBodySimulation.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

// File Loading Constructor
NBodySimulation::NBodySimulation(const std::string& filename)
    : tPlotDelta(0.0), tFinal(0.0), dt(0.0) {
    
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error: Cannot open input file: " << filename << "\n";
        exit(1);
    }

    // Read simulation parameters
    if (!(infile >> dt >> tFinal >> tPlotDelta)) {
        std::cerr << "Error: Failed to read simulation parameters.\n";
        exit(1);
    }
    double rx, ry, rz, vx, vy, vz, m;   
        // Read 7 values from the file
        while (infile >> rx >> ry >> rz >> vx >> vy >> vz >> m) {
            // Push them into the parallel vectors
            pos_x.push_back(rx);
            pos_y.push_back(ry);
            pos_z.push_back(rz);

            vel_x.push_back(vx);
            vel_y.push_back(vy);
            vel_z.push_back(vz);

            mass.push_back(m);
        }
    
    std::cout << "Loaded " << pos_x.size() << " bodies from " << filename << "\n";
    std::cout << "dt=" << dt << ", tFinal=" << tFinal << ", plotDelta=" << tPlotDelta << "\n";
}