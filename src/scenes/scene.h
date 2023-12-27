#pragma once
#include "../game_engine.h"
#include "../action.h"

class Scene {
protected:
    GameEngine* m_game;
    EntityManager m_entities {};
    int m_currentFrame {0};
    std::map<int, std::string> m_actionMap {};
    // Booleans
    bool m_paused {false};
    bool m_hasEnded {false};
    bool m_sRender {true};
public:
    Scene(GameEngine* game)
        : m_game(game)
    {}
    
    virtual void update() = 0;
    virtual void sDoAction(const Action & action) = 0;
    virtual void sRender() = 0;

    void simulate(int);
    void doAction(const Action & action);
    void registerAction(const Action & action);
};