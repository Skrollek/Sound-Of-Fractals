#ifndef FRACTAL
#define FRACTAL
#include "pch.hpp"

class Fractal
{
private:

public:

};

class FractalDrawer : public sf::Drawable
{

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    Fractal *fractal;
}



#endif