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


bool IsOutOfBounds(const Analog::ChaoticOscillator& osc)
{
    using namespace std;

    double x = osc.vx();
    double y = osc.vy();
    double z = osc.vz();
    if (!isfinite(x) || !isfinite(y) || !isfinite(z))
        return true;

    double r = sqrt(x*x + y*y + z*z);
    return r > 20.0;
}


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

    Plotter plotter(8000);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Danger Tractor prototype by Don Cross");
    SetTargetFPS(60);
    const double angleIncrement = 0.005;
    bool autoRotate = false;
    int knob = 0;
    int speed = 0;
    int knobRepeat = 0;
    const int knobThresh = 5;
    bool failure = false;
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
        if (IsKeyPressed(KEY_R))
            autoRotate = !autoRotate;
        if (autoRotate)
            plotter.rotateX(+angleIncrement);
        if (IsKeyDown(KEY_PAGE_UP) && (knob < 100) && (++knobRepeat >= knobThresh))
        {
            ++knob;
            knobRepeat = 0;
        }
        if (IsKeyDown(KEY_PAGE_DOWN) && (knob > -100) && (++knobRepeat >= knobThresh))
        {
            --knob;
            knobRepeat = 0;
        }
        if (IsKeyDown(KEY_HOME) && (speed < 100) && (++knobRepeat >= knobThresh))
        {
            ++speed;
            knobRepeat = 0;
        }
        if (IsKeyDown(KEY_END) && (speed > 0) && (++knobRepeat >= knobThresh))
        {
            --speed;
            knobRepeat = 0;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        plotter.displayKnob(knob);
        plotter.displaySpeed(speed);
        if (failure)
        {
            plotter.displayFailureText();
        }
        else
        {
            plotter.append(osc->vx(), osc->vy(), osc->vz());
            osc->setKnob(knob / 100.0);
            double dt = std::pow(10.0, 3.0*(speed/100.0)) / SAMPLE_RATE;
            double pt = 0.0;
            for (int s = 0; s < SAMPLES_PER_FRAME; ++s)
            {
                osc->update(dt);
                failure = IsOutOfBounds(*osc);
                if (failure)
                    break;
                pt += dt;
                if (pt > 0.01)
                {
                    pt = 0.0;
                    plotter.append(osc->vx(), osc->vy(), osc->vz());
                }
            }
        }
        plotter.plot();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
