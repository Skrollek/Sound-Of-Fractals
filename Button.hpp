#ifndef BUTTON
#define BUTTON
#include "pch.hpp"

enum ButtonState 
{
    inactive, 
    hovered,
    pressed
};

class ButtonDrawer : public sf::Drawable
{
private:
    sf::RectangleShape button;
    sf::Text label;
public:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

class Button
{
friend ButtonDrawer;
private:
    ButtonState state;
    std::function<void()> action;
    std::string label;
public:
    void onEvent (sf::RenderWindow window, sf::Mouse mouse, void* data);
    std::string getLabel();
    void setLabel(std::string newLabel);
    Button(std::string initialLabel);

};



#endif