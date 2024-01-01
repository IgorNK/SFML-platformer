#pragma once
#include "../../debug_cell.h"
#include "../../level.h"
#include "../../player_config.h"
#include "../../tilemap.h"
#include "../scene.h"

class Scene_Play : public Scene {
  std::string m_levelPath{""};
  sf::Font m_font;
  std::shared_ptr<Entity> m_player = nullptr;
  PlayerConfig m_playerConfig{PlayerConfig()};
  Vec2 m_grid_size;
  Vec2 m_size_pixels;
  std::vector<DebugCell> m_debug_grid;
  sf::View m_view;
  Level m_level;
  TileMap m_tilemap;
  size_t m_frameCount;

  bool m_sDebugCollision{false};
  bool m_sDebugGrid{false};
  bool m_sRenderMap{true};

  void register_input();
  void init();
  void onEnd();
  void spawn_player(Vec2 position, bool snap_to_grid = true);
  void spawn_entities(const Level &level);
  void spawn_collision(const Level &level);

public:
  Scene_Play(GameEngine *game, const std::string &level_path);
  ~Scene_Play() {}

  void update();

  // Systems
  void sMovement();
  void sUserInput();
  void sInputHandling();
  void sRender();
  void sDoAction(const Action &action);
  void sDebug();
  std::vector<DebugCell> spawn_grid(const Vec2 &size, const Vec2 &grid_size);
  void sCollision();
};
