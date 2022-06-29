#include "Engine.hpp"

Engine::Engine()
{
    if (!shader.loadFromFile("vert.glsl", sf::Shader::Vertex)) 
    {
        std::cerr << "Failed to load vertex shader" << std::endl;
        exit(1);
    }
    if (!shader.loadFromFile("frag.glsl", sf::Shader::Fragment)) 
    {
        std::cerr << "Failed to load fragment shader" << std::endl;
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
    
    Button* resetButton = new Button("Reset\nView", [](Engine* eng, void* data)
                                                {
                                                    eng->cameraX = eng->cameraXDest = 0;
                                                    eng->cameraY = eng->cameraYDest = 0;
                                                    eng->cameraZoom = eng->cameraZoomDest = 100;
                                                    eng->frame = 0;
                                                }, this, NULL);
    buttons.push_back(resetButton);
    Button* fullscreenButton = new Button ("Toggle\nFullscreen", [](Engine* eng, void* data)
                                                                    {
                                                                        bool* tfc = (bool*) data;
                                                                        *tfc = true;
                                                                    }, this, &toggleFullscreen);
    
    buttons.push_back(fullscreenButton);
    static std::pair<bool*, int*> colorFramePair = std::make_pair(&color, &frame);
    Button* useColorButton = new Button ("Start\nUsing\nColor", [](Engine* eng, void* data)
                                                                    {
                                                                        std::pair<bool*, int*>* dt = (std::pair<bool*, int*>*) data;
                                                                        *dt->first = !(*dt->first);
                                                                        *dt->second = 0;
                                                                    }, this, &colorFramePair);

    buttons.push_back(useColorButton);
    Button* takeScreenshotButton = new Button ("Take\nScreenshot", [](Engine* eng, void* data)
                                                                        {
                                                                            bool* sc = (bool*) data;
                                                                            *sc = true;
                                                                        }, this, &screenshot);
    
    buttons.push_back(takeScreenshotButton);                 
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
        else if (event.type == sf::Event::Resized)
        {
            resizeWindow(event.size.width, event.size.height);
            resizeButtons();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            const sf::Keyboard::Key code = event.key.code;
            if (code == sf::Keyboard::F11)
            {
                toggleFullscreen = true;
            }
            else
            {
                buttons[3]->handleEvent(event);
            }
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
            else if(event.mouseButton.button == sf::Mouse::Left)
            {
                leftPressed = true;
                orbit = true;
                ScreenToPt(event.mouseButton.x, event.mouseButton.y, baseX, baseY);
                soundmaker->SetPoint(baseX, baseY);
                mouseX = baseX;
                mouseY = baseY;
            }
            else if(event.mouseButton.button == sf::Mouse::Right)
            {
                orbit = false;
                soundmaker->audio_pause = true;
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if(event.mouseButton.button == sf::Mouse::Middle)
            {
                dragging = false;
            } 
            else if (event.mouseButton.button == sf::Mouse::Left)
            {
                leftPressed = false;
                //std::cerr << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y << std::endl; 
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
            if(leftPressed)
            {
                ScreenToPt(event.mouseMove.x, event.mouseMove.y, baseX, baseY);
                mouseX = baseX;
                mouseY = baseY;
                soundmaker->SetPoint(baseX, baseY);
            }
        }
        for(Button* btn : buttons)
        {
            btn->handleEvent(event);
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
    windowWidth = newWidth;
    windowHeight = newHeight;
    renderTexture.create(windowWidth, windowHeight);
    window.setView(sf::View (sf::FloatRect (0, 0, (float)windowWidth, (float)windowHeight) ) );
    frame = 0;
}

void Engine::selectFractal(int id)
{
    shader.setUniform("iType", id);
    orbit = false;
    frame = 0;
    currentFractalId = id;
    soundmaker->audio_pause = true;

}

void Engine::resizeButtons()
{
    assert(cameraZoom > 0);
    for(unsigned i = 0; i < buttons.size(); ++i)
    {
        buttons[i]->getButtonDrawer()->body.setPosition(0.9 * (float)windowWidth, 
                                                          i * (float)windowHeight / buttons.size());
        buttons[i]->getButtonDrawer()->body.setSize(sf::Vector2f(0.1 * (float)windowWidth,
                                                                (float)windowHeight / buttons.size()));
        buttons[i]->getButtonDrawer()->setFont(font);
        buttons[i]->getButtonDrawer()->label.setPosition(0.9 * (float)windowWidth, 
                                                           i * (float)windowHeight / buttons.size());

        buttons[i]->getButtonDrawer()->fontColor     = sf::Color::Black;
        buttons[i]->getButtonDrawer()->inactiveColor = sf::Color::Blue;
        buttons[i]->getButtonDrawer()->hoverColor    = sf::Color::Green;
        buttons[i]->getButtonDrawer()->pressedColor  = sf::Color::Red;
        
        buttons[i]->getButtonDrawer()->label.setCharacterSize(15);
        buttons[i]->getButtonDrawer()->label.setStyle(sf::Text::Regular);
    }
    // sadly not so simple
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
    bool juliaDrag = false;

    sustain = false;
    color = false;
    orbit = false;
    leftPressed = false;
    dragging = false;


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

    fullscreen = true;
    toggleFullscreen = false;

    changeWindow();
    resizeButtons();
    shader.setUniform("iType", 0);
    shader.setUniform("iCam", sf::Vector2f((float)cameraX, (float)cameraY));
    shader.setUniform("iZoom", (float)cameraZoom);

    soundmaker = new SoundMaker(window.getSystemHandle());
    soundmaker->normalized = fractals[currentFractalId].first->isNormalized();

    while(window.isOpen())
    {
        handleEvents();

        double fpx, fpy, cameraXDelta, cameraYDelta;
        ScreenToPt(cameraXFP, cameraYFP, fpx, fpy);
        cameraZoom = cameraZoom * 0.8 + cameraZoomDest * 0.2;
        ScreenToPt(cameraXFP, cameraYFP, cameraXDelta, cameraYDelta);
        cameraXDest += cameraXDelta - fpx;
        cameraYDest += cameraYDelta - fpy;
        cameraX += cameraXDelta - fpx;
        cameraY += cameraYDelta - fpy;
        cameraX = cameraX * 0.8 + cameraXDest * 0.2;
        cameraY = cameraY * 0.8 + cameraYDest * 0.2;

        const bool hasJulia = (jx < 1e8);
        const bool drawMset = (juliaDrag || !hasJulia);
        const int flags = (drawMset ? 0x01 : 0) | (false ? 0x02 : 0) | (color ? 0x04 : 0);
        sf::Glsl::Vec2 windowRes((float)windowWidth, (float)windowHeight);

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

        sprite.setTexture(renderTexture.getTexture());
        window.clear();
        window.draw(sprite, sf::RenderStates(BlendIgnoreAlpha));

        const double xSpeed = std::abs(cameraX - cameraXDest) * cameraZoomDest;
        const double ySpeed = std::abs(cameraY - cameraYDest) * cameraZoomDest;
        const double zoomSpeed = std::abs(cameraZoom / cameraZoomDest - 1.0);
        if (xSpeed < 0.2 && ySpeed < 0.2 && zoomSpeed < 0.002) {
            frame += 1;
        } else {
            frame = 1;
        }

        for(Button* btn : buttons)
        {
            window.draw(*btn->getButtonDrawer());
        }

        if(orbit)
        {
            fractals[currentFractalId].first->calculateOrbit(std::complex(mouseX, mouseY), std::complex(baseX, baseY));
            window.draw(*fractals[currentFractalId].first->getFractalDrawer());
        }   

        window.display();

        if(toggleFullscreen)
        {
            toggleFullscreen = false;
            fullscreen = !fullscreen;
            changeWindow();
            resizeButtons();
        }

        if(screenshot)
        {
            screenshot = false;
            window.display(); // make sure window is valid
            sf::Texture texture;
            texture.create(window.getSize().x, window.getSize().y);
            texture.update(window);
            texture.copyToImage().saveToFile("screenshot.png");
        }
        
    }
    soundmaker->stop();
}

Fractal* Engine::getCurrentFractal()
{
    return fractals[currentFractalId].first;
}