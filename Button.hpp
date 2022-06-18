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
    sf::RectangleShape rect;
    
};

class Button
{
friend ButtonDrawer;
private:
    ButtonState state;
    std::function<void()> action;
public:
    void onEvent (sf::RenderWindow window, sf::Mouse mouse, void* data);
};



#endif