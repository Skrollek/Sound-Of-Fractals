#ifndef FRACTAL
#define FRACTAL
#include "pch.hpp"
#define NUMBER_OF_FRACTALS 1
// this class should be static singleton
class FractalDrawer;

class Fractal
{
friend FractalDrawer;
protected:
    std::vector <std::complex<double> > orbit;
    std::complex<double> base;
    uint64_t id;
    uint64_t NOsamples;
    bool normalized;
public:
    virtual std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base) = 0;
    void calculateOrbit(std::complex<double> point, std::complex<double> base);
    bool isNormalized() {return normalized; }
};

class FractalDrawer : public sf::Drawable
{

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    Fractal *fractal;
};

class Mandelbrot : public Fractal
{
public:
    std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base);
    Mandelbrot();
};





#endif