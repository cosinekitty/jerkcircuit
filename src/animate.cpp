/*
    animate.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-10-01

    Animates various chaotic oscillators I am studying as candidates
    for VCV Rack modules.

    https://github.com/cosinekitty/jerkcircuit
*/

#include <cstdio>
#include <cstring>
#include "MakeChaoticOscillator.hpp"
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
    auto osc = MakeChaoticOscillator(kind);
    if (!osc)
    {
        printf("ERROR: Unknown chaotic oscillator kind '%s'\n", kind);
        return 1;
    }

    Plotter plotter(5000);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jerk Circuit Simulation");
    SetTargetFPS(30);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        plotter.plot(osc->xVoltage(), osc->yVoltage());
        EndDrawing();
        for (int s = 0; s < SAMPLES_PER_FRAME; ++s)
            osc->update(SAMPLE_RATE);
    }
    CloseWindow();
    return 0;
}
