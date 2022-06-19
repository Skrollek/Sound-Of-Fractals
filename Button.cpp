#include "Button.hpp"

Button::Button (std::string initialLabel)
{
    this->label = initialLabel;
    state = inactive;
}

void onEvent (sf::RenderWindow window, sf::Mouse mouse, sf::Event event, void* data)
{
    return; // to implement
}

std::string Button::getLabel()
{
    return this->label;
}

void Button::setLabel (std::string newLabel)
{
    this->label = newLabel;
}