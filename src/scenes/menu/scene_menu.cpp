#include "scene_menu.h"
#include <iostream>

void Scene_Menu::update() {
    sDebug();
}

void Scene_Menu::sDoAction(const Action & action) {

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