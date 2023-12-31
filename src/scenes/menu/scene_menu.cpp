#include "scene_menu.h"
#include "../play/scene_play.h"
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

Scene_Menu::Scene_Menu(GameEngine* game)
    : Scene(game) 
{
    init();
}

void Scene_Menu::init() {
    m_menuStrings = {
        "resources/level1.lvl",
        "resources/level2.lvl"
    };
    
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

    if (!m_font.loadFromFile("resources/ThaleahFat.ttf")) {
        std::cerr << "Failed to load font resources/ThaleahFat.ttf \n";
        return;
    }

    for (int i = 0; i < m_menuStrings.size(); ++i) {
        const std::string & item = m_menuStrings[i];
        sf::Text text;
        text.setFont(m_font);
        text.setString(item);
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::Black);
        text.setStyle(sf::Text::Bold);
        text.setPosition(100, 150 + 60 * i);
        m_menuItems.push_back(text);
    };

    m_menuTitle = sf::Text("CHOOSE LEVEL", m_font, 72);
    m_menuTitle.setFillColor(sf::Color::Black);
    m_menuTitle.setPosition(20, 20);
    m_menuHint = sf::Text("UP:W DOWN:S PLAY:D   BACK:ESC", m_font, 24);
    m_menuHint.setFillColor(sf::Color::Black);
    m_menuHint.setPosition(20, m_game->getWindow().getSize().y - 50);


    m_initialized = true;
}

void Scene_Menu::update() {
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
    window.clear(sf::Color(60, 180, 255));
    if (m_initialized && m_sRender) {
        for (int i = 0; i < m_menuItems.size(); ++i) {
            sf::Text & text = m_menuItems[i];
            if (i == m_selectedMenuIndex) {
                text.setFillColor(sf::Color::White);
                text.setStyle(sf::Text::Bold | sf::Text::Underlined);
            } else {
                text.setFillColor(sf::Color::Black);
                text.setStyle(sf::Text::Bold);
            }
            window.draw(text);
        }
        window.draw(m_menuTitle);
        window.draw(m_menuHint);
    }
    sDebug();


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