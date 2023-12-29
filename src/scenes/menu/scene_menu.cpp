#include "scene_menu.h"
#include "../play/scene_play.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* game)
    : Scene(game) 
{
    init();
}

void Scene_Menu::init() {
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_menuStrings = {
        "resources/level1.lvl",
        "resources/level2.lvl"
    };
}

void Scene_Menu::update() {
    sDebug();
}

void Scene_Menu::sDoAction(const Action & action) {
    if (action.type() == "START") {
        if (action.name() == "UP") {
            if (m_selectedMenuIndex > 0) {
                m_selectedMenuIndex = (MenuItem)(m_selectedMenuIndex - 1);
            } else {
                m_selectedMenuIndex = (MenuItem)(m_menuStrings.size() - 1);
            }
        } else if (action.name() == "DOWN") {
            m_selectedMenuIndex = (MenuItem)((m_selectedMenuIndex + 1) % m_menuStrings.size());
        }
        else if (action.name() == "PLAY") {
            const std::string level_path = m_menuStrings[m_selectedMenuIndex];
            m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_menuStrings[m_selectedMenuIndex]));
        } else if (action.name() == "QUIT") {
            onEnd();
        }
    }

}

void Scene_Menu::sRender() {
    sf::RenderWindow & window = m_game->getWindow();
    if (m_sRender) {
        // m_window.draw(shape);
    }

}

void Scene_Menu::sDebug() {
    ImGui::Begin("Platformer Project Debug");
    ImGui::SeparatorText("Main Menu");
    ImGui::End();
}

void Scene_Menu::onEnd() {
    if (m_game->m_game_close_countdown > 0) {
        m_game->m_running = false; 
    } else {
        m_game->m_game_close_countdown = m_game->m_game_close_timeout;
    }
}