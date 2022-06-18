#ifndef FRACTAL
#define FRACTAL
#include "pch.hpp"

class FractalDrawer;

class Fractal
{
friend FractalDrawer;
private:
    std::vector <std::complex<double> > orbit;
    std::complex<double> base;
    uint64_t id;
    uint64_t NOsamples;
public:
    virtual std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base) = 0;
    void calculateOrbit(std::complex<double> point, std::complex<double> base);

};

class FractalDrawer : public sf::Drawable
{

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    Fractal *fractal;
};

class Mandlebrot : public Fractal
{
public:
    std::complex<double> nextPoint(std::complex<double> point, std::complex<double> base);
};





#endif