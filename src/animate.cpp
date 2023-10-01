/*
    animate.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-10-01

    Runs the Jerk Circuit simulation and renders an X/Y plot.

    https://github.com/cosinekitty/jerkcircuit
*/

#include "JerkCircuit.hpp"
#include "plotter.hpp"


int main()
{
    using namespace Analog;

    Plotter plotter(5000);

    JerkCircuit circuit;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jerk Circuit Simulation");
    SetTargetFPS(30);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        plotter.plot(circuit.xVoltage(), circuit.yVoltage());
        EndDrawing();
        for (int s = 0; s < SAMPLES_PER_FRAME; ++s)
            circuit.update(SAMPLE_RATE);
    }
    CloseWindow();
    return 0;
}

