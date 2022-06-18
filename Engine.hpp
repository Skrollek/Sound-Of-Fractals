#ifndef ENGINE
#define ENGINE
class Engine
{
private:
    static Engine *instance;
public:
    Engine* getInstance()
    {
        if(instance == nullptr)
            instance = new Engine;
        return instance;
    }

    int run()
    {
        
    }
};


#endif