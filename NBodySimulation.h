#ifndef NBODY_SIMULATION_H
#define NBODY_SIMULATION_H

#include <vector>
#include <string>

struct Body {
    double x[3];
    double v[3];
    double mass;
};

class NBodySimulation {
public:
    double tPlotDelta;
    double tFinal;
    double dt;
    std::vector<Body> bodies;

    // Declarations only
    NBodySimulation();
    explicit NBodySimulation(const std::string& filename);
};

#endif // NBODY_SIMULATION_H