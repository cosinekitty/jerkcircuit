#pragma once
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
        double x;
        double y;
        double z;

        virtual SlopeVector slopes() const = 0;

    private:
        const double xmin;
        const double xmax;
        const double ymin;
        const double ymax;
        const double zmin;
        const double zmax;

    public:
        ChaoticOscillator(
            double x0, double y0, double z0,
            double _xmin, double _xmax,
            double _ymin, double _ymax,
            double _zmin, double _zmax
        )
            : x(x0)
            , y(y0)
            , z(z0)
            , xmin(_xmin)
            , xmax(_xmax)
            , ymin(_ymin)
            , ymax(_ymax)
            , zmin(_zmin)
            , zmax(_zmax)
            {}

        // Use this version to bootstrap oscillators with unknown ranges
        ChaoticOscillator(double x0, double y0, double z0)
            : x(x0)
            , y(y0)
            , z(z0)
            , xmin(0.0)
            , xmax(0.0)
            , ymin(0.0)
            , ymax(0.0)
            , zmin(0.0)
            , zmax(0.0)
            {}

        virtual ~ChaoticOscillator() {}

        double vx() const { return Remap(x, xmin, xmax); }
        double vy() const { return Remap(y, ymin, ymax); }
        double vz() const { return Remap(z, zmin, zmax); }

        void update(float sampleRateHz)
        {
            SlopeVector s = slopes();
            double dt = 1.0 / sampleRateHz;
            x += dt * s.mx;
            y += dt * s.my;
            z += dt * s.mz;
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
            : ChaoticOscillator(0.788174, 0.522280, 1.250344, -10.2, +10.2, -5.6, +5.6, 0.0, +15.4)
            {}
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
            : ChaoticOscillator(0.1, 0.0, 0.0)
            {}
    };
}
