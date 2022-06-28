#ifndef FRACTAL
#define FRACTAL
#include "pch.hpp"
#define NUMBER_OF_FRACTALS 1
// this class should be static singleton
class Engine;
class FractalDrawer;

class Fractal
{
friend FractalDrawer;
protected:
    FractalDrawer *fd;
    std::vector <std::complex<double> > orbit;
    std::complex<double> base;
    uint64_t id;
    uint64_t NOsamples;
    bool normalized;
public:
    virtual std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base) = 0;
    void calculateOrbit(std::complex<double> point, std::complex<double> base);
    bool isNormalized() { return normalized; }
    FractalDrawer* getFractalDrawer() { return fd; }
};

class FractalDrawer : public sf::Drawable
{
public:
    FractalDrawer(Fractal* ini);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
protected:
    Fractal *fractal;
};

class Mandelbrot : public Fractal
{
public:
    std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base);
    Mandelbrot();
};





#endif