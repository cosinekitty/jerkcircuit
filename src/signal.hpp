#pragma once
namespace Analog
{
    class Signal
    {
    public:
        virtual double xVoltage() const = 0;
        virtual double yVoltage() const = 0;
        virtual double zVoltage() const = 0;
        virtual int update(float sampleRateHz) = 0;
    };


    class Rucklidge : public Signal     // http://www.3d-meier.de/tut19/Seite17.html
    {
    private:
        const double k = 2.0;
        const double a = 6.7;

        double x = 1.0;
        double y = 0.0;
        double z = 0.0;

    public:
        double xVoltage() const override
        {
            return x;
        }

        double yVoltage() const override
        {
            return y;
        }

        double zVoltage() const override
        {
            return z;
        }

        int update(float sampleRateHz) override
        {
            double dt = 1.0 / sampleRateHz;
            double dx = dt*(-k*x + a*y - y*z);
            double dy = dt*(x);
            double dz = dt*(-z + y*y);
            x += dx;
            y += dy;
            z += dz;
            return 1;
        }
    };
}
