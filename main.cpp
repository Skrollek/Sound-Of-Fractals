#include "Engine.hpp"

int main()
{
    if (!sf::Shader::isAvailable()) 
    {
        std::cerr << "Graphics card does not support shaders" << std::endl;
        return 1;
    }

    Engine::getInstance()->run();

    return 0;
}