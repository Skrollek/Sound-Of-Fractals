#include "Engine.hpp"

Engine::Engine()
{
    // to implement
    window = new sf::RenderWindow(sf::VideoMode(800,600), "FRTL");
    window->setFramerateLimit(60);
    if (!shader.loadFromFile("vert.glsl", sf::Shader::Vertex)) 
    {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        exit(1);
    }
    if (!shader.loadFromFile("frag.glsl", sf::Shader::Fragment)) 
    {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        exit(1);
    }
    
    
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
    const int flags = (true ? 0x01 : 0) | (false ? 0x02 : 0) | (false ? 0x04 : 0);
    shader.setUniform("iCam", sf::Vector2f(0,0));
    shader.setUniform("iZoom", 100.0f);
    const sf::Glsl::Vec2 window_res(800, 600);
    shader.setUniform("iResolution", window_res);
    shader.setUniform("iFlags", flags);
    shader.setUniform("iJulia", sf::Vector2f(1e8, 1e8));
    shader.setUniform("iIters", 1200);
    shader.setUniform("iTime", 0);

    sf::RenderTexture renderTexture;
    renderTexture.create(800,600);
    window->setView(sf::View(sf::FloatRect(0, 0, 800, 600)));

    sf::RectangleShape rect;
    rect.setPosition(0, 0);

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    sf::BlendMode BlendAlpha(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add,
                                        sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add);
    sf::BlendMode BlendIgnoreAlpha(sf::BlendMode::One, sf::BlendMode::Zero, sf::BlendMode::Add,
                                     sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add);
    

    while(window->isOpen())
    {
        handleEvents();

        sf::RenderStates states = sf::RenderStates::Default;
        states.blendMode = BlendAlpha;
        states.shader = &shader;
        rect.setSize(window_res);
        renderTexture.draw(rect, states);
        renderTexture.display();

        sf::Sprite sprite(renderTexture.getTexture());
        window->clear();
        window->draw(sprite, sf::RenderStates(BlendIgnoreAlpha));
        
        window->display();
    }
}