#include "scene_play.h"
#include "../../component.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

Scene_Play::Scene_Play(GameEngine *game, const std::string &level_path)
    : Scene(game), m_levelPath(level_path) {
  init();
}

void Scene_Play::init() {
  std::cout << "Loading " << m_levelPath << '\n';

  const std::string font_path = "resources/ShareTech-Regular.ttf";
  if (!m_font.loadFromFile(font_path)) {
    std::cerr << "Failed to load font " << font_path << std::endl;
    return;
  }

  // m_view = sf::View();
  // m_game->getWindow().setView(m_view);

  if (!m_level.load_file(m_levelPath)) {
    std::cerr << "Scene failed to initialize, couldn't load " << m_levelPath
              << std::endl;
    return;
  }

  if (!m_tilemap.load(m_level.getTileset(), m_level.getTiles(),
                      m_level.getDimensions().x, m_level.getDimensions().y)) {
    std::cerr << "Failed to load tilemap." << std::endl;
    return;
  }

  m_size_pixels =
      Vec2({m_level.getDimensions().x * m_level.getTileset().tileWidth,
            m_level.getDimensions().y * m_level.getTileset().tileHeight});

  m_grid_size = Vec2({(float)m_level.getTileset().tileWidth,
                      (float)m_level.getTileset().tileHeight});

  std::cout << "Initialized level of size: " << m_size_pixels.x << ":"
            << m_size_pixels.y << ", grid size: " << m_grid_size.x << ":"
            << m_grid_size.y << std::endl;

  spawn_entities(m_level);

  m_debug_grid = spawn_grid(m_size_pixels, m_grid_size);
}

void Scene_Play::spawn_entities(const Level &level) {
  for (const EntityData &e_data : level.getEntities()) {
    std::cout << "Processing spawn of " << e_data.name << std::endl;
    if (e_data.x == 0 || e_data.y == 0) {
      std::cerr << "Possibly invalid spawn coords: " << e_data.x << ":"
                << e_data.y << std::endl;
    }
    if (e_data.type != "EntitySpawn") {
      continue;
    }
    if (e_data.name == "PlayerStart") {
      std::cout << "Spawning player at: " << e_data.x << ":" << e_data.y
                << std::endl;
      spawn_player({(float)e_data.x, (float)e_data.y});
    }
  }
}

void Scene_Play::spawn_player(const Vec2 &position) {
  std::cout << "spawning player entity.\n";
  m_player = m_entities.add_entity(Tag::Player);
  m_player->addComponent<CTransform>(position);
  m_player->addComponent<CAnimatedSprite>();
  AnimatedSprite &p_sprite = m_player->getComponent<CAnimatedSprite>().sprite;
  p_sprite = AnimatedSprite();
  p_sprite.load_file("resources/VirtualGuy.atlas", m_game->getAssets());
  p_sprite.setRepeat(true);
  p_sprite.play("Idle", 10);
}

void Scene_Play::onEnd() {
  m_game->getWindow().setView(m_game->getWindow().getDefaultView());
}

void Scene_Play::update() {
  m_entities.update();
  sDebug();
  ++m_frameCount;
}

void Scene_Play::sDoAction(const Action &action) {}

void Scene_Play::sRender() {
  sf::RenderWindow &window = m_game->getWindow();
  if (m_sRender) {
    // std::cout << "In scene render loop\n";
    for (const auto &entity : m_entities.get_entities()) {
      // std::cout << "entity\n";
      CAnimatedSprite &anim_sprite = entity->getComponent<CAnimatedSprite>();
      CTransform &transform = entity->getComponent<CTransform>();
      if (!anim_sprite.has || !transform.has) {
        // std::cout << "entity doesn't have a sprite and transform\n";
        continue;
      }
      if (m_frameCount % anim_sprite.sprite.getSpeed() == 0) {
        // std::cout << "should update sprite\n";
        anim_sprite.sprite.update();
      }
      anim_sprite.sprite.setPosition(transform.pos);
      // std::cout << "drawing sprite at pos: " << transform.pos.x << ":"
      //           << transform.pos.y << '\n';
      window.draw(anim_sprite.sprite.getSprite());
    }
    if (m_sRenderMap) {
      window.draw(m_tilemap);
    }
  }
  if (m_sDebugGrid) {
    for (const DebugCell &cell : m_debug_grid) {
      window.draw(cell);
    }
  }
}

void Scene_Play::sDebug() {
  ImGui::Begin("Platformer Project Debug");
  ImGui::SeparatorText("Gameplay scene");
  ImGui::End();
}

std::vector<DebugCell> Scene_Play::spawn_grid(const Vec2 &size,
                                              const Vec2 &grid_size) {
  const int n_x = std::ceil(size.x / grid_size.x);
  const int n_y = std::ceil(size.y / grid_size.y);
  std::vector<DebugCell> cells;
  if (n_x <= 0 || n_y <= 0) {
    return {};
  }
  for (int i = 0; i < n_x; ++i) {
    for (int j = 0; j < n_y; ++j) {
      const DebugCell cell(grid_size, Vec2(i, j), m_font);
      cells.push_back(cell);
    }
  }
  // for (const auto & cell : cells) {
  //     std::cout << cell.getPosition().x * cell.getSize().x << ":" <<
  //     cell.getPosition().y * cell.getSize().y << " - " <<
  //     cell.getPosition().x * cell.getSize().x + cell.getSize().x << ":" <<
  //     cell.getPosition().y * cell.getSize().y + cell.getSize().y <<
  //     std::endl;
  // }
  return cells;
}
