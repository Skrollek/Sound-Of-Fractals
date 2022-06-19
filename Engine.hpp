#ifndef ENGINE
#define ENGINE
#include "pch.hpp"
#include "Button.hpp"

class Engine
{
private:
    sf::RenderWindow *window;
    sf::Event event;
    sf::Font font;
    std::vector<Button> buttons;
    sf::Shader shader;
    Engine();
    void handleEvents();
public:
    static Engine* getInstance();
    void run();
};

#endif
