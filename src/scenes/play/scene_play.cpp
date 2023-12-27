#include "scene_play.h"
#include <iostream>

void Scene_Play::update() {
    sDebug();
}

void Scene_Play::sDoAction(const Action & action) {

}

void Scene_Play::sRender() {
    sf::RenderWindow & window = m_game->getWindow();
    if (m_sRender) {
        // m_window.draw(shape);
    }

}

void Scene_Play::sDebug() {

    ImGui::Begin("Platformer Project Debug");
    ImGui::SeparatorText("Gameplay scene");
    ImGui::End();
}
