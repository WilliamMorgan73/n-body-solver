#include <iostream>
#include <cmath>
#include <vector>
#include "NBodySimulation.h"
#include "IO.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " input_file.txt\n";
        return 1;
    }
    NBodySimulation nbs(argv[1]);

    // Write initial snapshot
    openPVDFile();
    int snapshotCounter = 0;
    writeVTKSnapshot(nbs, snapshotCounter);
    addSnapshotToPVD(snapshotCounter);

    // Initialise simulation at t = 0
    double t = 0.0;
    double nextPlotTime = nbs.tPlotDelta;

    int N = (int)nbs.pos_x.size();

    // Allocate force arrays
    std::vector<double> forceX(N, 0.0);
    std::vector<double> forceY(N, 0.0);
    std::vector<double> forceZ(N, 0.0);

    while (t < nbs.tFinal)
    {
        // The reading of the input and writing of the output in IO.cpp may need to be modified if the data structures are
        // Do not modify the input or output file format. It must remain the same! - Think I have already done that. Make sure to check
        t += nbs.dt;

        // Set force arrays to zero
        std::fill(forceX.begin(), forceX.end(), 0.0);
        std::fill(forceY.begin(), forceY.end(), 0.0);
        std::fill(forceZ.begin(), forceZ.end(), 0.0);

        // Compute gravitational forces
        for (int i = 0; i < N; ++i)
        {
            // Load body i data once
            double xi = nbs.pos_x[i];
            double yi = nbs.pos_y[i];
            double zi = nbs.pos_z[i];
            double mi = nbs.mass[i];

            for (int j = i + 1; j < N; ++j)
            {
                double dx = nbs.pos_x[j] - xi;
                double dy = nbs.pos_y[j] - yi;
                double dz = nbs.pos_z[j] - zi;

                double distSqr = dx * dx + dy * dy + dz * dz;

                // Inverse square root
                double invDist = 1.0 / std::sqrt(distSqr);
                double invDist3 = invDist * invDist * invDist;

                double F = (mi * nbs.mass[j]) * invDist3;

                forceX[i] += F * dx;
                forceY[i] += F * dy;
                forceZ[i] += F * dz;

                forceX[j] -= F * dx;
                forceY[j] -= F * dy;
                forceZ[j] -= F * dz;
            }
        }

        // Update velocities and positions
        for (int i = 0; i < N; ++i)
        {
            double invMass = nbs.dt / nbs.mass[i];

            nbs.vel_x[i] += invMass * forceX[i];
            nbs.vel_y[i] += invMass * forceY[i];
            nbs.vel_z[i] += invMass * forceZ[i];

            nbs.pos_x[i] += nbs.dt * nbs.vel_x[i];
            nbs.pos_y[i] += nbs.dt * nbs.vel_y[i];
            nbs.pos_z[i] += nbs.dt * nbs.vel_z[i];
        }

        if (t >= nextPlotTime) {
            snapshotCounter++;
            writeVTKSnapshot(nbs, snapshotCounter);
            addSnapshotToPVD(snapshotCounter);
            nextPlotTime += nbs.tPlotDelta;
        }
    }

    closePVDFile();
    std::cout << "Simulation completed.\n";

    return 0;
}
