#include "Engine.hpp"

Engine::Engine()
{
    window = new sf::RenderWindow(sf::VideoMode(800,600), "FRTL");
}


Engine* Engine::getInstance()
{
    static Engine instance;
    return &instance;
}

void Engine::handleEvents()
{
    while(window->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            window->close();
        }
    }
}

void Engine::run()
{
    while(window->isOpen())
    {
        handleEvents();
        
        window->clear();
        window->display();
    }
}