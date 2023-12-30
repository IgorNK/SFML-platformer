#pragma once
#include "../scene.h"
#include "../../player_config.h"
#include "../../level.h"

class Scene_Play : public Scene {
    std::string m_levelPath {""};
    Entity * m_player = nullptr;
    PlayerConfig m_playerConfig {PlayerConfig()};
    size_t m_grid_size {32};
    sf::VertexArray m_debug_lines;
    sf::View m_view;
    Level m_level;
public:
    Scene_Play(GameEngine* game, const std::string & level_path);
    ~Scene_Play() {}

    void init();
    void update();
    void onEnd();

    // Systems
    void sAnimation();
    void sMovement();
    void sRender();
    void sDoAction(const Action & action);
    void sDebug();
    void sDrawGrid(const size_t grid_size);
    void sCollision();
};