#pragma once

#include <cmath>

namespace Analog
{
    const double AMPLITUDE = 5.0;  // the intended peak amplitude of output voltage

    inline double Remap(double v, double vmin, double vmax)
    {
        // Remaps v from the range [vmin, vmax] to [-AMPLITUDE, +AMPLITUDE].
        // But before we know the range, return the unmodified signal.
        if (vmax <= vmin)
            return v;

        // How far along the range is v in the range [vmin, vmax]?
        double r = (v - vmin) / (vmax - vmin);      // [0, 1]
        return AMPLITUDE * (2*r - 1);
    }


    struct SlopeVector
    {
        double mx;
        double my;
        double mz;

        SlopeVector(double _mx, double _my, double _mz)
            : mx(_mx)
            , my(_my)
            , mz(_mz)
            {}
    };


    class ChaoticOscillator
    {
    protected:
        double max_dt = 0.0;

        double x{};
        double y{};
        double z{};

        virtual SlopeVector slopes() const = 0;

    private:
        const double x0;
        const double y0;
        const double z0;

        const double xmin;
        const double xmax;
        const double ymin;
        const double ymax;
        const double zmin;
        const double zmax;

    public:
        const bool isTuned;

        ChaoticOscillator(
            double _x0, double _y0, double _z0,
            double _xmin, double _xmax,
            double _ymin, double _ymax,
            double _zmin, double _zmax
        )
            : x0(_x0)
            , y0(_y0)
            , z0(_z0)
            , xmin(_xmin)
            , xmax(_xmax)
            , ymin(_ymin)
            , ymax(_ymax)
            , zmin(_zmin)
            , zmax(_zmax)
            , isTuned(true)
        {
            initialize();
        }

        // Use this version to bootstrap oscillators with unknown ranges
        ChaoticOscillator(double _x0, double _y0, double _z0)
            : x0(_x0)
            , y0(_y0)
            , z0(_z0)
            , xmin(0.0)
            , xmax(0.0)
            , ymin(0.0)
            , ymax(0.0)
            , zmin(0.0)
            , zmax(0.0)
            , isTuned(false)
        {
            initialize();
        }

        virtual ~ChaoticOscillator() {}

        bool hasStabilityProtection() const { return max_dt > 0.0; }

        void initialize()
        {
            x = x0;
            y = y0;
            z = z0;
        }

        // Scaled values...
        double vx() const { return Remap(x, xmin, xmax); }
        double vy() const { return Remap(y, ymin, ymax); }
        double vz() const { return Remap(z, zmin, zmax); }

        // Raw values...
        double rx() const { return x; }
        double ry() const { return y; }
        double rz() const { return z; }

        void update(double dt)
        {
            // If the derived class has informed us of a maximum stable time increment,
            // use oversampling to keep the actual time increment within that limit:
            // find the smallest positive integer n such that dt/n <= max_dt.
            const int n = (max_dt <= 0.0) ? 1 : static_cast<int>(std::ceil(dt / max_dt));
            const double et = dt / n;
            for (int i = 0; i < n; ++i)
            {
                SlopeVector s = slopes();
                x += et * s.mx;
                y += et * s.my;
                z += et * s.mz;
            }
        }
    };


    class Rucklidge : public ChaoticOscillator     // http://www.3d-meier.de/tut19/Seite17.html
    {
    private:
        const double k = 2.0;
        const double a = 6.7;

    protected:
        SlopeVector slopes() const override
        {
            return SlopeVector (
                -k*x + a*y - y*z,
                x,
                -z + y*y
            );
        }

    public:
        Rucklidge()
            : ChaoticOscillator(
                0.788174, 0.522280, 1.250344,
                -10.144, +10.168,
                 -5.570,  +5.565,
                 +0.040,  +15.387)
        {
            max_dt = 0.002;
        }
    };


    class Aizawa : public ChaoticOscillator     // http://www.3d-meier.de/tut19/Seite3.html
    {
    private:
        const double a = 0.95;
        const double b = 0.7;
        const double c = 0.6;
        const double d = 3.5;
        const double e = 0.25;
        const double f = 0.1;

    protected:
        SlopeVector slopes() const override
        {
            return SlopeVector(
                (z-b)*x - d*y,
                d*x + (z-b)*y,
                c + a*z - z*z*z/3 - (x*x + y*y)*(1 + e*z) + f*z*x*x*x
            );
        }

    public:
        Aizawa()
            : ChaoticOscillator(
                0.440125, -0.781267, -0.277170,
                -1.505, +1.490,
                -1.455, +1.530,
                -0.370, +1.853)
        {
            max_dt = 0.0007;
        }
    };


    class Sprott : public ChaoticOscillator     // http://www.3d-meier.de/tut19/Seite192.html
    {
    private:
        const double a = 2.5;
        const double b = 1.5;

    protected:
        SlopeVector slopes() const override
        {
            return SlopeVector(
                a*(y - x),
                x*z,
                b - y*y
            );
        }

    public:
        Sprott()
            : ChaoticOscillator(
                0.010847, 0.003817, 0.485189,
                -3.860,  +4.061,
                -5.565,  +6.006,
                -8.437,  +8.085)
        {
            max_dt = 0.0001;
        }
    };
}
