/*
    animate.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-10-01

    Runs the Jerk Circuit simulation and renders an X/Y plot.

    https://github.com/cosinekitty/jerkcircuit
*/

#include <cstdio>
#include <cstring>
#include "JerkCircuit.hpp"
#include "plotter.hpp"


int main(int argc, const char *argv[])
{
    using namespace Analog;

    if (argc != 2)
    {
        printf("USAGE: animate kind\n");
        return 1;
    }
    const char *kind = argv[1];

    Rucklidge ruck;

    ChaoticOscillator *signal = nullptr;
    if (!strcmp(kind, "ruck"))
        signal = &ruck;
    else
    {
        printf("ERROR: Unknown signal type '%s'\n", kind);
        return 1;
    }

    Plotter plotter(2000);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jerk Circuit Simulation");
    SetTargetFPS(30);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        plotter.plot(signal->xVoltage(), signal->yVoltage());
        EndDrawing();
        for (int s = 0; s < SAMPLES_PER_FRAME; ++s)
            signal->update(SAMPLE_RATE);
    }
    CloseWindow();
    return 0;
}
