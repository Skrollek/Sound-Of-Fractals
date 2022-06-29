#ifndef ENGINE
#define ENGINE
#include "pch.hpp"
#include "Button.hpp"
#include "Fractal.hpp"
#include "SoundMaker.hpp"

class SoundMaker;

class Engine
{
friend Button;
friend FractalDrawer;
private:

    bool fullscreen;
    bool toggleFullscreen;
    bool sustain;
    bool color;
    bool orbit;
    bool dragging;
    bool leftPressed;
    bool screenshot;
    int frame;

    double cameraX, cameraY;
    double cameraZoom;
    int cameraXFP;
    int cameraYFP;
    double cameraXDest;
    double cameraYDest;
    double cameraZoomDest;
    double mouseX, mouseY;
    double baseX, baseY;

    double jx;
    double jy;

    std::vector<Button*> buttons;

    SoundMaker* soundmaker;

    std::vector<std::pair<Fractal*, bool>> fractals;
    unsigned currentFractalId;

    sf::RenderWindow window;
    int windowWidth;
    int windowHeight;
    sf::Event event;
    
    sf::Font font;
    sf::RenderTexture renderTexture;
    sf::RectangleShape fractalDrawing;
    sf::RenderStates states;
    sf::Sprite sprite;
    sf::ContextSettings settings;
    sf::Shader shader;

    sf::Vector2i prevDrag;
    sf::Vector2i currDrag;
    
    Engine();
    void handleEvents();
    void resizeWindow(int newWidth, int newHeight);
    void changeWindow();
    void selectFractal(int id);
    void resizeButtons();

    void ScreenToPt(int x, int y, double& px, double& py) 
    {
        px = double(x - windowWidth  / 2) / cameraZoom - cameraX;
        py = double(y - windowHeight / 2) / cameraZoom - cameraY;
    }
    void PtToScreen(double px, double py, int& x, int& y) 
    {
        x = int(cameraZoom * (px + cameraX)) + windowWidth  / 2;
        y = int(cameraZoom * (py + cameraY)) + windowHeight / 2;
    }

public:
    static Engine* getInstance();
    void run();
    Fractal* getCurrentFractal();
};

#endif
