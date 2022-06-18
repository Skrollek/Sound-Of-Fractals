#ifndef ENGINE
#define ENGINE
#include "pch.hpp"
#include "Button.hpp"

class Engine
{
private:
    sf::RenderWindow *window;
    sf::Event event;
    std::vector<Button> buttons;
    Engine();
    void handleEvents();
public:
    static Engine* getInstance();
    void run();
};

#endif
