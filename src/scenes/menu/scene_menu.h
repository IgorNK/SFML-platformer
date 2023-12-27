#pragma once
#include "../scene.h"
#include "../../player_config.h"
#include <SFML/Graphics/Text.hpp>

class Scene_Menu : public Scene {
    std::vector<std::string> m_menuStrings {};
    sf::Text m_menuText = sf::Text();
    std::vector<std::string> m_levelPaths {};
    int menuIndex {0};

public:
    Scene_Menu(GameEngine* game)
        : Scene(game) 
    { }
    ~Scene_Menu() {}

    void init();
    void update();

    // Systems
    void sRender();
    void sDebug();
    void sDoAction(const Action & action);
};