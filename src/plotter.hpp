#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include "raylib.h"

const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 800;
const int SAMPLE_RATE = 44100;
const int FRAME_RATE = 60;
const int SAMPLES_PER_FRAME = SAMPLE_RATE / FRAME_RATE;

const double MIN_VOLTAGE = -5.5;
const double MAX_VOLTAGE = +5.5;

struct ScreenPoint
{
    int sx;
    int sy;

    ScreenPoint(int _sx, int _sy)
        : sx(_sx)
        , sy(_sy)
        {}
};

struct PlotVector
{
    double nx;
    double ny;
    double nz;

    PlotVector(double _nx, double _ny, double _nz)
        : nx(_nx)
        , ny(_ny)
        , nz(_nz)
        {}

    void rotateX(double c, double s)
    {
        double ry = c*ny - s*nz;
        double rz = s*ny + c*nz;
        ny = ry;
        nz = rz;
    }

    void rotateY(double c, double s)
    {
        double rx = c*nx - s*nz;
        double rz = s*nx + c*nz;
        nx = rx;
        nz = rz;
    }
};

class Plotter
{
private:
    const std::size_t trailLength;
    std::size_t trailIndex = 0;
    std::vector<PlotVector> trail;
    PlotVector xdir{1.0, 0.0, 0.0};
    PlotVector ydir{0.0, 1.0, 0.0};

public:
    explicit Plotter(int _trailLength)
        : trailLength(std::max(2, _trailLength))
        {}

    void rotateX(double radians)
    {
        double c = std::cos(radians);
        double s = std::sin(radians);
        xdir.rotateX(c, s);
        ydir.rotateX(c, s);
    }

    void rotateY(double radians)
    {
        double c = std::cos(radians);
        double s = std::sin(radians);
        xdir.rotateY(c, s);
        ydir.rotateY(c, s);
    }

    ScreenPoint project(double vx, double vy, double vz) const
    {
        double x = vx*xdir.nx + vy*xdir.ny + vz*xdir.nz;
        double y = vx*ydir.nx + vy*ydir.ny + vz*ydir.nz;
        return ScreenPoint(
            static_cast<int>(std::round(((x - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE)) * SCREEN_WIDTH)),
            static_cast<int>(std::round(((MAX_VOLTAGE - y) / (MAX_VOLTAGE - MIN_VOLTAGE)) * SCREEN_HEIGHT))
        );
    }

    ScreenPoint project(const PlotVector &vec) const
    {
        return project(vec.nx, vec.ny, vec.nz);
    }

    void append(double vx, double vy, double vz)
    {
        PlotVector current(vx, vy, vz);

        // On the first render, prefill the trail buffer.
        while (trail.size() < trailLength)
            trail.push_back(current);

        trail.at(trailIndex) = current;
        trailIndex = (trailIndex + 1) % trailLength;
    }

    void plot()
    {
        Color color = BLACK;
        Color target = GREEN;

        const std::size_t fadeLength = std::max({target.r, target.g, target.b, static_cast<unsigned char>(1)});
        const std::size_t fadeInterval = std::max(std::size_t{1}, trailLength / (2 * fadeLength));
        std::size_t fadeCount = fadeInterval;

        std::size_t i = trailIndex;
        while (true)
        {
            std::size_t j = (i + 1) % trailLength;
            if (j == trailIndex)
                break;

            ScreenPoint a = project(trail.at(i));
            ScreenPoint b = project(trail.at(j));
            DrawLine(a.sx, a.sy, b.sx, b.sy, color);
            i = j;

            if (--fadeCount == 0)
            {
                fadeCount = fadeInterval;
                if (color.r < target.r) ++color.r;
                if (color.g < target.g) ++color.g;
                if (color.b < target.b) ++color.b;
            }
        }

        const PlotVector& current = trail.at((trailIndex + trailLength - 1) % trailLength);
        ScreenPoint s = project(current);
        DrawCircle(s.sx, s.sy, 2.0f, WHITE);
    }

    void displayKnob(int knob)
    {
        char text[50];
        snprintf(text, sizeof(text), "knob: %4d", knob);
        DrawText(text, 5, 5, 20, BROWN);
    }

    void displaySpeed(int speed)
    {
        char text[50];
        snprintf(text, sizeof(text), "speed: %4d", speed);
        DrawText(text, SCREEN_WIDTH-115, 5, 20, BROWN);
    }
};
