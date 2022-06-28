#include "Button.hpp"
#include "Engine.hpp"

void ButtonDrawer::updateColor()
{
    if(button->state == hovered)
        this->body.setFillColor(hoverColor);
    else if(button->state == pressed)
        this->body.setFillColor(pressedColor);
    else 
        this->body.setFillColor(inactiveColor);
}

void ButtonDrawer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(body,states);
    target.draw(label,states);
}

std::string ButtonDrawer::getLabel() const
{
    return label.getString();
}

void ButtonDrawer::setLabel (std::string newLabel)
{
    label.setString(newLabel);
}

void ButtonDrawer::loadFontFromFile (std::string path)
{
    if(!font.loadFromFile(path))
    {
        std::cerr << "UNABLE TO LOAD FONT FROM FILE: " + path << std::endl; // endl usage is justified because I need to be sure that this message was flushed
    }
    label.setFont(font);
}

void ButtonDrawer::setFont(sf::Font& newFont)
{
    font = newFont;
    label.setFont(font);
}

ButtonDrawer::ButtonDrawer (Button* button)
{
    this->button = button;
}

Button::Button (std::string initialLabel, std::function<void(Engine*, void*)> initialAction, Engine* eng, void* data)
{
    drawer = new ButtonDrawer(this);
    drawer->setLabel(initialLabel);
    state = inactive;
    action = std::bind(initialAction, eng, data);
}

void Button::handleEvent(sf::Event& event)
{
    state = inactive;
    sf::Vector2f btnPos  = drawer->body.getPosition();
    sf::Vector2f btnSize = drawer->body.getSize();
    double x,y;
    x = sf::Mouse::getPosition(Engine::getInstance()->window).x;
    y = sf::Mouse::getPosition(Engine::getInstance()->window).y;
    if(x >= btnPos.x  && x <= btnPos.x + btnSize.x 
    && y >= btnPos.y  && y <= btnPos.y + btnSize.y)
    {
        state = hovered;
    }
    if(event.type == sf::Event::MouseButtonReleased
    && event.mouseButton.button == sf::Mouse::Left 
    && state == hovered)
    {
        state = pressed;
        drawer->updateColor();
        action();
    }
    drawer->updateColor();
}

ButtonDrawer* Button::getButtonDrawer()
{
    return drawer;
}

