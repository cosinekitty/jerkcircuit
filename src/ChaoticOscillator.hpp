#pragma once
namespace Analog
{
    const double AMPLITUDE = 5.0;  // the intended peak amplitude of output voltage

    inline double Remap(double v, double vmin, double vmax)
    {
        // Remaps v from the range [vmin, vmax] to [-AMPLITUDE, +AMPLITUDE].
        // How far along the range is v in the range [vmin, vmax]?
        double r = (v - vmin) / (vmax - vmin);      // [0, 1]
        return AMPLITUDE * (2*r - 1);
    }

    class ChaoticOscillator
    {
    protected:
        double x;
        double y;
        double z;

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

        virtual ~ChaoticOscillator() {}

        double xVoltage() const { return Remap(x, xmin, xmax); }
        double yVoltage() const { return Remap(y, ymin, ymax); }
        double zVoltage() const { return Remap(z, zmin, zmax); }

        virtual void update(float sampleRateHz) = 0;
    };


    class Rucklidge : public ChaoticOscillator     // http://www.3d-meier.de/tut19/Seite17.html
    {
    private:
        const double k = 2.0;
        const double a = 6.7;

    public:
        Rucklidge()
            : ChaoticOscillator(0.788174, 0.522280, 1.250344, -10.2, +10.2, -5.6, +5.6, 0.0, +15.4)
            {}

        void update(float sampleRateHz) override
        {
            double dt = 1.0 / sampleRateHz;
            double dx = dt*(-k*x + a*y - y*z);
            double dy = dt*(x);
            double dz = dt*(-z + y*y);
            x += dx;
            y += dy;
            z += dz;
        }
    };
}
