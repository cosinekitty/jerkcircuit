#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "ChaoticOscillator.hpp"

static int RangeTest(Analog::ChaoticOscillator& osc);

int main(int argc, const char *argv[])
{
    using namespace Analog;

    if (argc != 2)
    {
        printf("USAGE: rangetest kind\n");
        return 1;
    }

    ChaoticOscillator *osc = nullptr;
    const char *kind = argv[1];
    if (!strcmp(kind, "ruck"))
        osc = new Rucklidge();

    if (osc == nullptr)
    {
        printf("ERROR: Unknown kind '%s'\n", kind);
        return 1;
    }

    int rc = RangeTest(*osc);
    delete osc;
    return rc;
}

static int CheckLimits(const Analog::ChaoticOscillator& osc)
{
    const double LIMIT = 1000.0;
    double x = osc.xVoltage();
    double y = osc.yVoltage();
    double z = osc.zVoltage();
    if (!std::isfinite(x) || std::abs(x) > LIMIT)
    {
        printf("x is out of bounds: %lg\n", x);
        return 1;
    }
    if (!std::isfinite(y) || std::abs(y) > LIMIT)
    {
        printf("y is out of bounds: %lg\n", y);
        return 1;
    }
    if (!std::isfinite(z) || std::abs(z) > LIMIT)
    {
        printf("z is out of bounds: %lg\n", z);
        return 1;
    }
    return 0;
}

static int RangeTest(Analog::ChaoticOscillator& osc)
{
    const int SAMPLE_RATE = 44100;
    const int SIM_SECONDS = 4 * 3600;
    const int SIM_SAMPLES = SIM_SECONDS * SAMPLE_RATE;

    double xMin = 0;
    double xMax = 0;
    double yMin = 0;
    double yMax = 0;
    double zMin = 0;
    double zMax = 0;

    const int SETTLE_SECONDS = 60;
    const int SETTLE_SAMPLES = SETTLE_SECONDS * SAMPLE_RATE;
    for (int i = 0; i < SETTLE_SAMPLES; ++i)
    {
        osc.update(SAMPLE_RATE);
        if (CheckLimits(osc)) return 1;
    }

    printf("Settled  at: x=%10.6lf, y=%10.6lf, z=%10.6lf\n", osc.xVoltage(), osc.yVoltage(), osc.zVoltage());

    for (int i = 0; i < SIM_SAMPLES; ++i)
    {
        osc.update(SAMPLE_RATE);
        if (CheckLimits(osc)) return 1;
        if (i == 0)
        {
            xMin = xMax = osc.xVoltage();
            yMin = yMax = osc.yVoltage();
            zMin = zMax = osc.zVoltage();
        }
        else
        {
            xMin = std::min(xMin, osc.xVoltage());
            xMax = std::max(xMax, osc.xVoltage());
            yMin = std::min(yMin, osc.yVoltage());
            yMax = std::max(yMax, osc.yVoltage());
            zMin = std::min(zMin, osc.zVoltage());
            zMax = std::max(zMax, osc.zVoltage());
        }
    }

    printf("Finished at: x=%10.6lf, y=%10.6lf, z=%10.6lf\n", osc.xVoltage(), osc.yVoltage(), osc.zVoltage());

    printf("x range: %10.6lf %10.6lf\n", xMin, xMax);
    printf("y range: %10.6lf %10.6lf\n", yMin, yMax);
    printf("z range: %10.6lf %10.6lf\n", zMin, zMax);

    return 0;
}
