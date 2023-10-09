#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include "MakeChaoticOscillator.hpp"

static int RangeTest(Analog::ChaoticOscillator& osc);

int main(int argc, const char *argv[])
{
    using namespace Analog;

    if (argc != 2)
    {
        printf("USAGE: rangetest [kind | all]\n");
        printf("\nwhere kind is one of:\n");
        for (const char *kind : ChaoticOscillatorKinds)
            printf("    %s\n", kind);
        return 1;
    }

    const char *kind = argv[1];
    int rc = 1;
    if (!strcmp(kind, "all"))
    {
        for (const char *oscKind : ChaoticOscillatorKinds)
        {
            auto osc = MakeChaoticOscillator(oscKind);
            if (!osc)
            {
                printf("INTERNAL ERROR: Unknown chaotic oscillator kind '%s'\n", kind);
                return 1;
            }
            printf("Testing: %s\n", oscKind);
            rc = RangeTest(*osc);
            if (rc != 0)
                break;
        }
    }
    else
    {
        auto osc = MakeChaoticOscillator(kind);
        if (!osc)
        {
            printf("ERROR: Unknown chaotic oscillator kind '%s'\n", kind);
            return 1;
        }
        rc = RangeTest(*osc);
    }
    return rc;
}

static int CheckLimits(const Analog::ChaoticOscillator& osc)
{
    const double LIMIT = 1000.0;
    double x = osc.vx();
    double y = osc.vy();
    double z = osc.vz();
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

    printf("Settled  at: x=%10.6lf, y=%10.6lf, z=%10.6lf\n", osc.vx(), osc.vy(), osc.vz());

    for (int i = 0; i < SIM_SAMPLES; ++i)
    {
        osc.update(SAMPLE_RATE);
        if (CheckLimits(osc)) return 1;
        if (i == 0)
        {
            xMin = xMax = osc.vx();
            yMin = yMax = osc.vy();
            zMin = zMax = osc.vz();
        }
        else
        {
            xMin = std::min(xMin, osc.vx());
            xMax = std::max(xMax, osc.vx());
            yMin = std::min(yMin, osc.vy());
            yMax = std::max(yMax, osc.vy());
            zMin = std::min(zMin, osc.vz());
            zMax = std::max(zMax, osc.vz());
        }
    }

    printf("Finished at: x=%10.6lf, y=%10.6lf, z=%10.6lf\n", osc.vx(), osc.vy(), osc.vz());

    printf("x range: %10.6lf %10.6lf\n", xMin, xMax);
    printf("y range: %10.6lf %10.6lf\n", yMin, yMax);
    printf("z range: %10.6lf %10.6lf\n", zMin, zMax);

    return 0;
}
