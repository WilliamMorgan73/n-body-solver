#include <iostream>
#include <cmath>
#include <vector>
#include "NBodySimulation.h"
#include "IO.h"
#include <omp.h> 

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

    // Collision Constant
    const double C = 1e-2 / (double)N;

    // Total time spent

    double totalTime = 0.0;

    while (t < nbs.tFinal)
    {
        t += nbs.dt;

        // Set force arrays to zero
        std::fill(forceX.begin(), forceX.end(), 0.0);
        std::fill(forceY.begin(), forceY.end(), 0.0);
        std::fill(forceZ.begin(), forceZ.end(), 0.0);

        // Start timer
        double startTime = omp_get_wtime();

        // Compute gravitational forces
        for (int i = 0; i < N; ++i)
        {
            // Load body i data once
            double xi = nbs.pos_x[i];
            double yi = nbs.pos_y[i];
            double zi = nbs.pos_z[i];
            double mi = nbs.mass[i];

            double fx = 0.0;
            double fy = 0.0;
            double fz = 0.0;

            // Softening parameter to avoid singularities
            const double epsilon = 1e-9;

            #pragma omp simd reduction(+:fx,fy,fz)
            for (int j = 0; j < N; ++j)
            {
                double dx = nbs.pos_x[j] - xi;
                double dy = nbs.pos_y[j] - yi;
                double dz = nbs.pos_z[j] - zi;

                double distSqr = dx * dx + dy * dy + dz * dz + epsilon;
                
                // Inverse square root
                double invDist = 1.0 / std::sqrt(distSqr);
                double invDist3 = invDist * invDist * invDist;

                double F = (mi * nbs.mass[j]) * invDist3;

                fx += F * dx;
                fy += F * dy;
                fz += F * dz;
            }

            forceX[i] = fx;
            forceY[i] = fy;
            forceZ[i] = fz;
        }

        // Stop timer and accumulate time
        double endTime = omp_get_wtime();
        totalTime += (endTime - startTime);

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

        // Handle collisions
        for (int i = 0; i < N; ++i)
        {
            for (int j = i + 1; j < N; ++j)
            {
                double dx = nbs.pos_x[j] - nbs.pos_x[i];
                double dy = nbs.pos_y[j] - nbs.pos_y[i];
                double dz = nbs.pos_z[j] - nbs.pos_z[i];

                double dist = std::sqrt(dx * dx + dy * dy + dz * dz);

                if (dist <= C * (nbs.mass[i] + nbs.mass[j]))
                {
                    // Calculate new mass
                    double newMass = nbs.mass[i] + nbs.mass[j];

                    // Calculate Merged Position (Center of Mass)
                    nbs.pos_x[i] = (nbs.mass[i] * nbs.pos_x[i] + nbs.mass[j] * nbs.pos_x[j]) / newMass;
                    nbs.pos_y[i] = (nbs.mass[i] * nbs.pos_y[i] + nbs.mass[j] * nbs.pos_y[j]) / newMass;
                    nbs.pos_z[i] = (nbs.mass[i] * nbs.pos_z[i] + nbs.mass[j] * nbs.pos_z[j]) / newMass;

                    // Calculate Merged Velocity (Conservation of Momentum)
                    nbs.vel_x[i] = (nbs.mass[i] * nbs.vel_x[i] + nbs.mass[j] * nbs.vel_x[j]) / newMass;
                    nbs.vel_y[i] = (nbs.mass[i] * nbs.vel_y[i] + nbs.mass[j] * nbs.vel_y[j]) / newMass;
                    nbs.vel_z[i] = (nbs.mass[i] * nbs.vel_z[i] + nbs.mass[j] * nbs.vel_z[j]) / newMass;

                    // Update mass
                    nbs.mass[i] = newMass;

                    // Remove body j by swapping with the last body

                    int last = N - 1;

                    nbs.pos_x[j] = nbs.pos_x[last];
                    nbs.pos_y[j] = nbs.pos_y[last];
                    nbs.pos_z[j] = nbs.pos_z[last];
                    nbs.mass[j] = nbs.mass[last];
                    nbs.vel_x[j] = nbs.vel_x[last];
                    nbs.vel_y[j] = nbs.vel_y[last];
                    nbs.vel_z[j] = nbs.vel_z[last];

                    // Remove the last body
                    nbs.pos_x.pop_back();
                    nbs.pos_y.pop_back();
                    nbs.pos_z.pop_back();
                    nbs.mass.pop_back();
                    nbs.vel_x.pop_back();
                    nbs.vel_y.pop_back();
                    nbs.vel_z.pop_back();

                    // Decrease N
                    N--;
                    // Decrement j to account for removed body
                    j--;
                }
                else
                {
                    // No collision
                    continue;
                }
            }
        }
        if (t >= nextPlotTime)
        {
            snapshotCounter++;
            writeVTKSnapshot(nbs, snapshotCounter);
            addSnapshotToPVD(snapshotCounter);
            nextPlotTime += nbs.tPlotDelta;
        }
    }

    std::cout << "Total time spent on force calculations: " << totalTime << " seconds.\n";

    closePVDFile();
    std::cout << "Simulation completed.\n";

    return 0;
}
