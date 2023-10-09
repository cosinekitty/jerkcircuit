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
        printf("where kind is one of the following:\n");
        for (const char *kind : ChaoticOscillatorKinds)
            printf("    %s\n", kind);
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
    const double dt = 1.0 / SAMPLE_RATE;
    const double angleIncrement = 0.005;
    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_DOWN))
            plotter.rotateX(+angleIncrement);
        if (IsKeyDown(KEY_UP))
            plotter.rotateX(-angleIncrement);
        if (IsKeyDown(KEY_LEFT))
            plotter.rotateY(+angleIncrement);
        if (IsKeyDown(KEY_RIGHT))
            plotter.rotateY(-angleIncrement);
        BeginDrawing();
        ClearBackground(BLACK);
        plotter.plot(osc->vx(), osc->vy(), osc->vz());
        EndDrawing();
        for (int s = 0; s < SAMPLES_PER_FRAME; ++s)
            osc->update(dt);
    }
    CloseWindow();
    return 0;
}
