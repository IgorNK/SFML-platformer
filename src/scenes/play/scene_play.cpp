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
    m_view = sf::View();
    m_game->getWindow().setView(m_view);
    m_level.load_file(m_levelPath);
}

void Scene_Play::onEnd() {
    m_game->getWindow().setView(m_game->getWindow().getDefaultView());
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
    sDrawGrid(m_grid_size);
    ImGui::Begin("Platformer Project Debug");
    ImGui::SeparatorText("Gameplay scene");
    ImGui::End();
}

void Scene_Play::sDrawGrid(const size_t grid_size) {
    
}
