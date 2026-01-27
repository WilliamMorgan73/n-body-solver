#ifndef NBODY_SIMULATION_H
#define NBODY_SIMULATION_H

#include <vector>
#include <string>

class NBodySimulation {
public:
    double tPlotDelta;
    double tFinal;
    double dt;

    // Separate vectors for each component
    std::vector<double> pos_x;
    std::vector<double> pos_y;
    std::vector<double> pos_z;

    std::vector<double> vel_x;
    std::vector<double> vel_y;
    std::vector<double> vel_z;

    std::vector<double> mass;

    // Helper function to get the number of particles
    size_t getN() const { return pos_x.size(); }

    // Constructor declaration
    explicit NBodySimulation(const std::string& filename);
};

#endif // NBODY_SIMULATION_H