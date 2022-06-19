#include "Engine.hpp"

Engine::Engine()
{
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
    if(!font.loadFromFile("ASCII.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        exit(1);
    }

    fractals.resize(NUMBER_OF_FRACTALS);
    for(unsigned i = 0; i < NUMBER_OF_FRACTALS; ++i)
    {
        auto f = new Mandelbrot();
        fractals[i] = {f, f->isNormalized()};
    }
}


Engine* Engine::getInstance()
{
    static Engine instance;
    return &instance;
}

void Engine::handleEvents()
{

    while(window.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::MouseWheelMoved)
        {
            cameraZoomDest *= std::pow(1.1f, event.mouseWheel.delta);
            const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            cameraXFP = mousePosition.x;
            cameraYFP = mousePosition.y;
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            if(event.mouseButton.button == sf::Mouse::Middle)
            {
                prevDrag = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                dragging = true;
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if(event.mouseButton.button == sf::Mouse::Middle)
            {
                dragging = false;
            } else if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cerr << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y << std::endl; 
            }
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            if(dragging)
            {
                currDrag = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                cameraXDest += (currDrag.x - prevDrag.x) / cameraZoom;
                cameraYDest += (currDrag.y - prevDrag.y) / cameraZoom;
                prevDrag = currDrag;
                frame = 0;
            }
        }
    }
}

void Engine::changeWindow()
{
    window.close();
    sf::VideoMode screenSize;
    if(fullscreen)
    {
        screenSize = sf::VideoMode::getDesktopMode();
        window.create(screenSize, "FRTL", sf::Style::Fullscreen, settings);
    }
    else
    {
        screenSize = sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 24);
        window.create(screenSize, "FRTL", sf::Style::Resize | sf::Style::Close, settings);
    }
    resizeWindow(screenSize.width, screenSize.height);
    window.setFramerateLimit(MAX_FPS);
    window.setKeyRepeatEnabled(false);
    window.requestFocus();
}

void Engine::resizeWindow(int newWidth, int newHeight)
{
    renderTexture.create(newWidth, newHeight);
    window.setView(sf::View (sf::FloatRect (0, 0, (float)newWidth, (float)newHeight) ) );
    frame = 0;
}

void Engine::selectFractal(int id)
{
    shader.setUniform("iType", id);
    orbit = false;
    frame = 0;
    currentFractalId = id;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

void Engine::run()
{
    const sf::BlendMode BlendAlpha( sf::BlendMode::SrcAlpha, 
                                    sf::BlendMode::OneMinusSrcAlpha, 
                                    sf::BlendMode::Add,
                                    sf::BlendMode::Zero, 
                                    sf::BlendMode::One, 
                                    sf::BlendMode::Add);
    const sf::BlendMode BlendIgnoreAlpha( sf::BlendMode::One, 
                                          sf::BlendMode::Zero, 
                                          sf::BlendMode::Add,
                                          sf::BlendMode::Zero,
                                          sf::BlendMode::One, 
                                          sf::BlendMode::Add);
    bool leftPressed = false;
    bool juliaDrag = false;
    bool takeScreenshot = false;

    sustain = true;
    color = true;
    orbit = false;
    frame = 0;
    windowWidth  = WINDOW_WIDTH;
    windowHeight = WINDOW_HEIGHT;

    cameraX = 0;
    cameraY = 0;
    cameraZoom = 100;
    cameraXFP = 0;
    cameraYFP = 0;
    cameraXDest = cameraX;
    cameraYDest = cameraY;
    cameraZoomDest = cameraZoom;

    jx = 1e8;
    jy = 1e8;

    fractalDrawing.setPosition(0, 0);

    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    changeWindow();

    shader.setUniform("iCam", sf::Vector2f((float)cameraX, (float)cameraY));
    shader.setUniform("iZoom", (float)cameraZoom);

    while(window.isOpen())
    {
        handleEvents();

        double fpx, fpy, cameraXDelta, cameraYDelta;
        ScreenToPt(cameraXFP, cameraYFP, fpx, fpy);
        cameraZoom = cameraZoom * 0.8 + cameraZoomDest * 0.2;
        ScreenToPt(cameraXFP, cameraYFP, cameraXDelta, cameraYDelta);
        cameraXDest += cameraXDelta - fpx;//sgn(cameraXDelta - fpx)*std::min(std::abs(cameraXDelta - fpx), 1.0);
        cameraYDest += cameraYDelta - fpy;//sgn(cameraYDelta - fpy)*std::min(std::abs(cameraYDelta - fpy), 1.0);
        if(std::abs(cameraXDelta - fpx) > 1000 || std::abs(cameraYDelta - fpy) > 1000)
            std::cerr << "CameraX :" << cameraX << " " << cameraXDelta - fpx << " " << "CameraY :" << cameraY << " " << cameraYDelta - fpy << std::endl;
        cameraX += cameraXDelta - fpx;//sgn(cameraXDelta - fpx)*std::min(std::abs(cameraXDelta - fpx), 1.0);
        cameraY += cameraYDelta - fpy;//sgn(cameraYDelta - fpy)*std::min(std::abs(cameraYDelta - fpy), 1.0);
        cameraX = cameraX * 0.8 + cameraXDest * 0.2;
        cameraY = cameraY * 0.8 + cameraYDest * 0.2;

        const bool hasJulia = (jx < 1e8);
        const bool drawMset = (juliaDrag || !hasJulia);
        const bool drawJset = (juliaDrag || hasJulia);
        const int flags = (drawMset ? 0x01 : 0) | (false ? 0x02 : 0) | (color ? 0x04 : 0);
        //std::cerr << cameraZoom << std::endl;
        //std::cin.get();
        const sf::Glsl::Vec2 windowRes((float)windowWidth, (float)windowHeight);
        shader.setUniform("iResolution", windowRes);
        shader.setUniform("iCam", sf::Vector2f((float)cameraX, (float)cameraY));
        shader.setUniform("iZoom", (float)cameraZoom);
        shader.setUniform("iFlags", flags);
        shader.setUniform("iJulia", sf::Vector2f((float)jx, (float)jy));
        shader.setUniform("iIters", MAX_ITERS);
        shader.setUniform("iTime", frame);
        
        states = sf::RenderStates::Default;
        states.blendMode = (frame > 0 ? BlendAlpha : BlendIgnoreAlpha);
        states.shader = &shader;
        fractalDrawing.setSize(windowRes);
        renderTexture.draw(fractalDrawing, states);
        renderTexture.display();

        const double xSpeed = std::abs(cameraX - cameraXDest) * cameraZoomDest;
        const double ySpeed = std::abs(cameraY - cameraYDest) * cameraZoomDest;
        const double zoomSpeed = std::abs(cameraZoom / cameraZoomDest - 1.0);
        if (xSpeed < 0.2 && ySpeed < 0.2 && zoomSpeed < 0.002) {
            frame += 1;
        } else {
            frame = 1;
        }

        sprite.setTexture(renderTexture.getTexture());
        window.clear();
        window.draw(sprite, sf::RenderStates(BlendIgnoreAlpha));

        window.display();
    }
}