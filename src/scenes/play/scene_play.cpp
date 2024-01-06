#include "scene_play.h"
#include "../../component.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <map>
#include "../../physics.h"

Scene_Play::Scene_Play(GameEngine *game, const std::string &level_path)
    : Scene(game), m_levelPath(level_path) {
  init();
}

void Scene_Play::update() {
  m_entities.update();
  sUserInput();
  sInputHandling();
  sMovement();
  sCollision();
  sCamera();
  sDebug();
  ++m_frameCount;
}

void Scene_Play::init() {
  std::cout << "Loading " << m_levelPath << '\n';

  const std::string font_path = "resources/ShareTech-Regular.ttf";
  if (!m_font.loadFromFile(font_path)) {
    std::cerr << "Failed to load font " << font_path << std::endl;
    return;
  }


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
  spawn_collision(m_level);

  m_debug_grid = spawn_grid(m_size_pixels, m_grid_size);

  register_input();

  
  m_view = sf::View(sf::Vector2f(0, 0), (sf::Vector2f)m_game->getWindow().getSize());
  m_game->getWindow().setView(m_view);
}

void Scene_Play::sCamera() {
  if (!m_player) {
    return;
  }
  CTransform & xform = m_player->getComponent<CTransform>();
  if (xform.has) {
    Vec2 levelSize = {
      m_level.getDimensions().x * m_level.getTileset().tileWidth,
      m_level.getDimensions().y * m_level.getTileset().tileHeight
    };
    Vec2 playerPos = xform.pos;
    sf::Vector2f viewSize = m_view.getSize();
    if (playerPos.x - viewSize.x / 2 < 0) {
      playerPos.x = viewSize.x / 2;
    }
    if (playerPos.x + viewSize.x / 2 > levelSize.x) {
      playerPos.x = levelSize.x - viewSize.x / 2;
    }
    if (playerPos.y - viewSize.y / 2 < 0) {
      playerPos.y = viewSize.y / 2;
    }
    if (playerPos.y + viewSize.y / 2 > levelSize.y) {
      playerPos.y = levelSize.y - viewSize.y / 2;
    }
    m_view.setCenter(sf::Vector2f(playerPos.x, playerPos.y));
  }
}

void Scene_Play::register_input() {
  registerAction(sf::Keyboard::Up, "UP");
  registerAction(sf::Keyboard::Down, "USE");
  registerAction(sf::Keyboard::Left, "LEFT");
  registerAction(sf::Keyboard::Right, "RIGHT");
  registerAction(sf::Keyboard::Z, "SHOOT");
  registerAction(sf::Keyboard::X, "JUMP");
  registerAction(sf::Keyboard::Escape, "MENU");
}

void Scene_Play::sDoAction(const Action &action) {

  if (action.type() == "END") {
    if (action.name() == "LEFT") {
    } else if (action.name() == "RIGHT") {
    } else if (action.name() == "UP") {
    } else if (action.name() == "USE") {
    } else if (action.name() == "JUMP") {
    }
  } else if (action.type() == "START") {
    if (action.name() == "LEFT") {
    } else if (action.name() == "RIGHT") {
    } else if (action.name() == "UP") {
    } else if (action.name() == "USE") {
    } else if (action.name() == "JUMP") {
    } else if (action.name() == "MENU") {
      onEnd();
    }
  }
}

void Scene_Play::sUserInput() {
  if (m_player && m_player->hasComponent<CInput>()) {
    m_player->getComponent<CInput>().prevAxis =
        m_player->getComponent<CInput>().axis;
    m_player->getComponent<CInput>().axis = Vec2{0, 0};
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    if (m_player && m_player->hasComponent<CInput>()) {
      m_player->getComponent<CInput>().axis.y = -1;
    }
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    if (m_player && m_player->hasComponent<CInput>()) {
      m_player->getComponent<CInput>().axis.y = 1;
    }
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    if (m_player && m_player->hasComponent<CInput>()) {
      m_player->getComponent<CInput>().axis.x = -1;
    }
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    if (m_player && m_player->hasComponent<CInput>()) {
      m_player->getComponent<CInput>().axis.x = 1;
    }
  }

  if (m_player && m_player->hasComponent<CInput>()) {
    CInput &p_input = m_player->getComponent<CInput>();
    if ((p_input.prevAxis.x > 0 && p_input.axis.x < 0) ||
        (p_input.prevAxis.x < 0 && p_input.axis.x > 0)) {
      p_input.directionChanged = true;
    } else {
      p_input.directionChanged = false;
    }
  }
}

void Scene_Play::sInputHandling() {
  if (m_player && m_player->hasComponent<CInput>() &&
      m_player->hasComponent<CVelocity>()) {
    CInput &input = m_player->getComponent<CInput>();
    CVelocity &vel = m_player->getComponent<CVelocity>();
    // Zeroing
    if (std::abs(vel.velocity.x) <= vel.margin) {
      vel.velocity.x = 0;
    }
    if (std::abs(vel.velocity.y) <= vel.margin) {
      vel.velocity.y = 0;
    }
    // Friction
    if (vel.velocity.x > vel.margin) {
      vel.velocity.x -= vel.deceleration;
    }
    if (vel.velocity.x < -vel.margin) {
      vel.velocity.x += vel.deceleration;
    }
    if (vel.velocity.y > vel.margin) {
      vel.velocity.y -= vel.deceleration;
    }
    if (vel.velocity.y < -vel.margin) {
      vel.velocity.y += vel.deceleration;
    }

    // Clamping
    if (vel.velocity.x > vel.maxSpeed) {
      vel.velocity.x = vel.maxSpeed;
    }
    if (vel.velocity.x < -vel.maxSpeed) {
      vel.velocity.x = -vel.maxSpeed;
    }
    if (vel.velocity.y > vel.maxSpeed) {
      vel.velocity.y = vel.maxSpeed;
    }
    if (vel.velocity.y < -vel.maxSpeed) {
      vel.velocity.y = -vel.maxSpeed;
    }

    // Velocity
    if ((std::abs(input.axis.x) > 0 || std::abs(input.axis.y) > 0) &&
        vel.currentSpeed < vel.maxSpeed && vel.currentSpeed > -vel.maxSpeed) {
      vel.currentSpeed += vel.acceleration;
    }

    if (vel.currentSpeed > vel.maxSpeed) {
      vel.currentSpeed = vel.maxSpeed;
    }
    if (vel.currentSpeed < -vel.maxSpeed) {
      vel.currentSpeed = -vel.maxSpeed;
    }

    if (input.directionChanged || input.axis.length() == 0) {
      vel.currentSpeed = 0;
    }

    vel.velocity += input.axis * vel.currentSpeed;
  }
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
      spawn_player({(float)e_data.x, (float)e_data.y}, true);
    }
  }
}

void Scene_Play::spawn_player(Vec2 position, bool snap_to_grid) {
  std::cout << "spawning player entity.\n";
  if (snap_to_grid) {
    position = m_level.snap_to_grid(position, SNAP_CENTER);
  }
  m_player = m_entities.add_entity(Tag::Player);
  m_player->addComponent<CTransform>(position);
  m_player->addComponent<CVelocity>();
  m_player->addComponent<CAnimatedSprite>();
  AnimatedSprite &p_sprite = m_player->getComponent<CAnimatedSprite>().sprite;
  p_sprite = AnimatedSprite();
  p_sprite.load_file("resources/VirtualGuy.atlas", m_game->getAssets());
  p_sprite.setRepeat(true);
  p_sprite.play("Idle", 10);
  m_player->addComponent<CBoundingBox>(p_sprite.getSize().x,
                                       p_sprite.getSize().y);
  m_player->addComponent<CDynamicCollision>();
  m_player->addComponent<CInput>();
}

void Scene_Play::spawn_collision(const Level &level) {
  Vec2 cell_size = {(float)level.getTileset().tileWidth,
                    (float)level.getTileset().tileHeight};
  int width = level.getDimensions().x;
  int height = level.getDimensions().y;
  const std::vector<sf::IntRect> edges = level.getEdgeBounds();
  // const std::vector<int> edge_cells = level.getEdges();
  // const std::vector<sf::IntRect> edges = level.getCombinedColliders(edge_cells);
  for (const sf::IntRect &rect : edges) {
    const auto collision = m_entities.add_entity(Tag::StaticCollision);
    collision->addComponent<CBoundingBox>(rect);
    collision->addComponent<CStaticCollision>();
  }
}

void Scene_Play::onEnd() {
  m_game->getWindow().setView(m_game->getWindow().getDefaultView());
  m_game->changeScene("MENU");
}

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
  if (m_sDebugCollision) {
    for (const auto entity : m_entities.get_entities()) {
      const CBoundingBox &bbox = entity->getComponent<CBoundingBox>();
      const CTransform &xform = entity->getComponent<CTransform>();
      Vec2 pos = {(float)bbox.rect.left, (float)bbox.rect.top};
      if (!bbox.has) {
        continue;
      }
      if (xform.has) {
        pos.x += xform.pos.x - (float)bbox.rect.width / 2;
        pos.y += xform.pos.y - (float)bbox.rect.height / 2;
      }
      sf::RectangleShape s_box(
          {(float)bbox.rect.width, (float)bbox.rect.height});
      s_box.setFillColor(sf::Color(0, 255, 0, 2));
      s_box.setOutlineColor(sf::Color(0, 255, 0));
      s_box.setOutlineThickness(1);
      s_box.setPosition({pos.x, pos.y});
      window.draw(s_box);
    }
  }
  window.setView(m_view);
}

void Scene_Play::sMovement() {
  for (const auto entity : m_entities.get_entities()) {
    CTransform &xform = entity->getComponent<CTransform>();
    const CVelocity &vel = entity->getComponent<CVelocity>();
    if (!xform.has || !vel.has) {
      continue;
    }
    xform.prevPos = xform.pos;
    xform.pos += vel.velocity;
  }
}

void Scene_Play::sCollision() {
  for (const std::shared_ptr<Entity> p : m_entities.get_entities(Tag::Player)) {
    CDynamicCollision & p_col = p->getComponent<CDynamicCollision>();
    const CBoundingBox & p_bbox = p->getComponent<CBoundingBox>();
    CTransform & p_xform = p->getComponent<CTransform>();
    if (!p_col.has || !p_bbox.has || !p_xform.has) {
      std::cerr << "Player doesn't have required collision\n";
      continue;
    }

    std::map<float, std::tuple<size_t, Overlap, sf::IntRect, CStaticCollision>> static_overlaps;

    // Collision detection:
    for (const std::shared_ptr<Entity> wall : m_entities.get_entities(Tag::StaticCollision)) {
      const CStaticCollision & w_col = wall->getComponent<CStaticCollision>();
      const CBoundingBox & w_bbox = wall->getComponent<CBoundingBox>();
      if (!w_col.has || !w_bbox.has) {
        std::cerr << "Static collision doesn't have required components\n";
        continue;
      }
      int w_radius = std::max(w_bbox.halfSize.x, w_bbox.halfSize.y);
      Vec2 w_center = {w_bbox.rect.left + w_bbox.halfSize.x, w_bbox.rect.top + w_bbox.halfSize.y};
      int p_radius = std::max(p_bbox.halfSize.x, p_bbox.halfSize.y);
      float distance = p_xform.pos.distance_to(w_center) - w_radius - p_radius;

      float x_a = w_bbox.rect.left + w_bbox.rect.width - (p_xform.pos.x - p_bbox.halfSize.x); // Player to the right
      float x_b = p_xform.pos.x + p_bbox.halfSize.x - w_bbox.rect.left; // player to the left
      float y_a = w_bbox.rect.top  + w_bbox.rect.height - ( p_xform.pos.y - p_bbox.halfSize.y); // player below
      float y_b = p_xform.pos.y + p_bbox.halfSize.y - w_bbox.rect.top; // player on top
      
      Overlap overlap = {
        x_a,
        x_b,
        y_a,
        y_b
      };

      if (distance < p_col.distance) {
        if (static_overlaps.find(distance) != static_overlaps.end()) {
          std::cerr << "Overlap with equal distance already exists!\n"; 
        }
        static_overlaps[distance] = std::make_tuple(wall->id(), overlap, w_bbox.rect, w_col);
      }
    }

    resolve_collision(p_col, p_bbox, p_xform, static_overlaps);

    for (const std::shared_ptr<Entity> enemy : m_entities.get_entities(Tag::Enemy)) {
      const CStaticCollision & e_col = enemy->getComponent<CStaticCollision>();
      const CBoundingBox & e_bbox = enemy->getComponent<CBoundingBox>();
      if (!e_col.has || !e_bbox.has) {
        continue;
      }

    }
  }
}

void Scene_Play::sDebug() {
  ImGui::Begin("Platformer Project Debug");
  ImGui::SeparatorText("Gameplay scene");
  ImGui::Checkbox("Render map", &m_sRenderMap);
  ImGui::Checkbox("Render grid", &m_sDebugGrid);
  ImGui::Checkbox("Render collision", &m_sDebugCollision);
  ImGui::BeginChild("Player", {0, 0},
                    ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
  if (m_player) {
    CTransform &xform = m_player->getComponent<CTransform>();
    CVelocity &vel = m_player->getComponent<CVelocity>();
    CInput &input = m_player->getComponent<CInput>();
    if (input.has) {
      float axis[2] = {input.axis.x, input.axis.y};
      float prevAxis[2] = {input.prevAxis.x, input.prevAxis.y};
      ImGui::SeparatorText("Player input");
      ImGui::InputFloat2("axis", axis);
      ImGui::InputFloat2("prevAxis", prevAxis);
      ImGui::Checkbox("directionChanged", &input.directionChanged);
    }
    if (xform.has) {
      float pos[2] = {xform.pos.x, xform.pos.y};
      float prevPos[2] = {xform.prevPos.x, xform.prevPos.y};
      ImGui::SeparatorText("Player transform");
      ImGui::InputFloat2("pos", pos);
      ImGui::InputFloat2("prevPos", prevPos);
    }
    if (vel.has) {
      float velocity[2] = {vel.velocity.x, vel.velocity.y};
      ImGui::SeparatorText("Player velocity");
      ImGui::InputFloat2("velocity", velocity);
      ImGui::InputFloat("maxSpeed", &vel.maxSpeed, 0.01f, 0.1f);
      ImGui::InputFloat("currentSpeed", &vel.currentSpeed);
      ImGui::InputFloat("acceleration", &vel.acceleration, 0.01f, 0.1f);
      ImGui::InputFloat("deceleration", &vel.deceleration, 0.01f, 0.1f);
      ImGui::InputFloat("margin", &vel.margin, 0.01f, 0.1f);
    }
  }
  ImGui::EndChild();
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
  return cells;
}
