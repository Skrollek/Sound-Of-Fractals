#include "Fractal.hpp"
#include "Engine.hpp"

void FractalDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::VertexArray orbitLines(sf::LineStrip, fractal->orbit.size());
    Engine* eng = Engine::getInstance();
    for(size_t it=0; it< fractal->orbit.size(); it++)
    {
        int x,y;
        eng->PtToScreen(fractal->orbit[it].real(), fractal->orbit[it].imag(), x, y);
        orbitLines[it].position = sf::Vector2f((float)x, (float)y);
        orbitLines[it].color = sf::Color::Red;
    }
    target.draw(orbitLines, states);
}

FractalDrawer::FractalDrawer (Fractal* ini)
{
    fractal = ini;
}
void Fractal::calculateOrbit(std::complex<double> point, std::complex<double> base)
{
    orbit.clear();
    for(unsigned i = 0; i < NOsamples; ++i)
    {
        point = nextPoint(point, base);
        orbit.push_back(point);
        if(abs(point) > MAX_POINT_DISTANCE_FROM_ORIGIN) break;
    }
    this->base = orbit[orbit.size() - 1]; 
}

std::complex<double> Mandelbrot::nextPoint(std::complex<double> point, std::complex<double> base)
{
    return point * point + base;
}

Mandelbrot::Mandelbrot()
{
    normalized = true;
    fd = new FractalDrawer(this);
    id = 0;
    NOsamples = MAX_ORBIT_SAMPLES;
}