#pragma once
#include "../scene.h"
#include "../../player_config.h"
#include "../../level.h"
#include "../../tilemap.h"
#include "../../debug_cell.h"

class Scene_Play : public Scene {
    std::string m_levelPath {""};
    sf::Font m_font;
    std::shared_ptr<Entity> m_player = nullptr;
    PlayerConfig m_playerConfig {PlayerConfig()};
    Vec2 m_grid_size;
    Vec2 m_size_pixels;
    std::vector<DebugCell> m_debug_grid;
    sf::View m_view;
    Level m_level;
    TileMap m_tilemap;

    bool m_sDebugGrid {true};
public:
    Scene_Play(GameEngine* game, const std::string & level_path);
    ~Scene_Play() {}

    void init();
    void update();
    void onEnd();
    void spawn_player(const Vec2 & position);

    // Systems
    void sAnimation();
    void sMovement();
    void sRender();
    void sDoAction(const Action & action);
    void sDebug();
    std::vector<DebugCell> spawn_grid(const Vec2 & size, const Vec2 & grid_size);
    void sCollision();
};