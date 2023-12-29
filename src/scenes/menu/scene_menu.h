#pragma once
#include "../scene.h"
#include "../../player_config.h"
#include <SFML/Graphics/Text.hpp>

enum MenuItem {
    MENU_LEVEL_1,
    MENU_LEVEL_2
};

class Scene_Menu : public Scene {
    std::vector<std::string> m_menuStrings {};
    MenuItem m_selectedMenuIndex {MenuItem::MENU_LEVEL_1};
    sf::Text m_menuText = sf::Text();
    std::vector<std::string> m_levelPaths {};
    int menuIndex {0};

public:
    Scene_Menu(GameEngine* game);
    ~Scene_Menu() {}

    void init();
    void update();
    void onEnd();

    // Systems
    void sRender();
    void sDebug();
    void sDoAction(const Action & action);
};