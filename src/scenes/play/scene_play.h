#pragma once
#include "../scene.h"
#include "../../player_config.h"

class Scene_Play : public Scene {
    std::string m_levelPath {""};
    Entity * m_player = nullptr;
    PlayerConfig m_playerConfig {PlayerConfig()};
public:
    Scene_Play(GameEngine* game, const std::string & level_path);
    ~Scene_Play() {}

    void init();
    void update();


    // Systems
    void sAnimation();
    void sMovement();
    void sRender();
    void sDoAction(const Action & action);
    void sDebug();
    void sCollision();
};