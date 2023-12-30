#include "scene_play.h"
#include <iostream>

Scene_Play::Scene_Play(GameEngine* game, const std::string & level_path)
    : Scene(game)
    , m_levelPath(level_path)
{ 
    init();
}

void Scene_Play::init() {
	std::cout << "Loading " << m_levelPath << '\n';

}

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
