#include "Fractal.hpp"

void FractalDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::VertexArray orbitLines(sf::LineStrip, fractal->orbit.size());
    sf::Vector2u screen = target.getSize();
    for(size_t it=0; it< fractal->orbit.size(); it++)
    {
        orbitLines[it].position = sf::Vector2f(fractal->orbit[it].real() * 200 + (screen.x/2),
                                               fractal->orbit[it].imag() * 200 + (screen.y/2));
        orbitLines[it].color = sf::Color::Red;
    }
    target.draw(orbitLines, states);
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

std::complex<double> Mandlebrot::nextPoint(std::complex<double> point, std::complex<double> base)
{
    return point * point + base;
}