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
            printf("\nTesting: %s\n", oscKind);
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
    const double LIMIT = osc.isTuned ? Analog::AMPLITUDE : 1000.0;
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
    const double dt = 1.0 / SAMPLE_RATE;

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
        osc.update(dt);
        if (CheckLimits(osc)) return 1;
    }

    printf("Settled  at: rx=%10.6lf, ry=%10.6lf, rz=%10.6lf\n", osc.rx(), osc.ry(), osc.rz());

    for (int i = 0; i < SIM_SAMPLES; ++i)
    {
        osc.update(dt);
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

    printf("Finished at: rx=%10.6lf, ry=%10.6lf, rz=%10.6lf\n", osc.rx(), osc.ry(), osc.rz());

    printf("vx range: %10.6lf %10.6lf\n", xMin, xMax);
    printf("vy range: %10.6lf %10.6lf\n", yMin, yMax);
    printf("vz range: %10.6lf %10.6lf\n", zMin, zMax);

    if (osc.isTuned)
    {
        // Try larger and larger dt values until we find the limits of stability.
        // Allow a tiny excess amplitude before we consider the simulation unstable.
        const double limit = Analog::AMPLITUDE + 0.1;
        const double dilate = 1.05;
        const int STABLE_SAMPLES = 600 * SAMPLE_RATE;
        for (double et = dt * dilate; et < 1.0; et *= dilate)
        {
            //printf("Dilated time increment = %lg\n", et);
            osc.initialize();
            for (int i = 0; i < STABLE_SAMPLES; ++i)
            {
                osc.update(et);
                if (!std::isfinite(osc.vx()) || !std::isfinite(osc.vy()) || !std::isfinite(osc.vz()))
                {
                    printf("Encountered non-finite voltage at sample %d, et=%lg.\n", i, et);
                    goto done;
                }
                double r = std::abs(osc.vx());
                r = std::max(r, std::abs(osc.vy()));
                r = std::max(r, std::abs(osc.vz()));
                if (r > limit)
                {
                    printf("Encountered excessive value %lg at sample %d, et=%lg.\n", r, i, et);
                    goto done;
                }
            }
        }
    }
done:

    return 0;
}
