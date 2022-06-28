#ifndef BUTTON
#define BUTTON
#include "pch.hpp"

enum ButtonState 
{
    inactive, 
    hovered,
    pressed
};

class Engine;
class Button;

class ButtonDrawer : public sf::Drawable
{
friend Button;
private:
    sf::Font font;
    Button* button;
    
public:
    void updateColor();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    ButtonDrawer(Button* button);
    std::string getLabel() const;
    void setLabel(std::string newLabel);
    void loadFontFromFile(std::string path);
    void setFont(sf::Font& newFont);

    sf::RectangleShape body;
    sf::Color fontColor;
    sf::Color inactiveColor;
    sf::Color hoverColor;
    sf::Color pressedColor;
    sf::Text  label;
};
class Button
{
friend ButtonDrawer;
private:
    ButtonState state;
    ButtonDrawer* drawer;
    std::function<void()> action;

public:
    void handleEvent(sf::Event& event);
    Button(std::string initialLabel, std::function<void(Engine*, void*)> initialAction, Engine* eng, void* data);
    ButtonDrawer* getButtonDrawer();
};



#endif