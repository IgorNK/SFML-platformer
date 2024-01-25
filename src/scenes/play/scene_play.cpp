#include "scene_play.h"
#include "../../component.h"
#include "../../physics.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <map>

Scene_Play::Scene_Play(GameEngine *game, const std::string &level_path)
    : Scene(game), m_levelPath(level_path) {
  init();
}

void Scene_Play::update() {
  m_entities.update();
  sUserInput();
  sGravity();
  sInputHandling();
  sMovement();
  sAnimation();
  sCollision();
  sHitReaction();
  sCamera();
  sCountdowns();
  sLifespan();
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

  m_view =
      sf::View(sf::Vector2f(0, 0), (sf::Vector2f)m_game->getWindow().getSize());
  m_game->getWindow().setView(m_view);
}

void Scene_Play::sCamera() {
  if (!m_player) {
    return;
  }
  CTransform &xform = m_player->getComponent<CTransform>();
  if (xform.has) {
    Vec2 levelSize = {
        m_level.getDimensions().x * m_level.getTileset().tileWidth,
        m_level.getDimensions().y * m_level.getTileset().tileHeight};
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
      if (m_player && m_player->getComponent<CInput>().has) {
        m_player->getComponent<CInput>().jump = false;
      }
    }
  } else if (action.type() == "START") {
    if (action.name() == "LEFT") {
    } else if (action.name() == "RIGHT") {
    } else if (action.name() == "UP") {
    } else if (action.name() == "USE") {
    } else if (action.name() == "JUMP") {
      std::cout << "jump start\n";
      if (m_player && m_player->getComponent<CInput>().has) {
        m_player->getComponent<CInput>().jump = true;
        std::cout << "jump start communicated to CInput\n";
      }
    } else if (action.name() == "MENU") {
      onEnd();
    } else if (action.name() == "SHOOT") {
      onShoot(m_player);
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
    if ((p_input.prevAxis.x >= 0 && p_input.axis.x < 0) ||
        (p_input.prevAxis.x <= 0 && p_input.axis.x > 0)) {
      p_input.directionChanged = true;
    } else {
      p_input.directionChanged = false;
    }
  }
}

void Scene_Play::sInputHandling() {
  for (const std::shared_ptr<Entity> e : m_entities.get_entities()) {
    const CCharacterController &col = e->getComponent<CCharacterController>();
    CVelocity &vel = e->getComponent<CVelocity>();
    if (!col.has || !vel.has) {
      continue;
    }
    CInput &input = e->getComponent<CInput>();
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
    // std::clamp(vel.velocity.x, -vel.maxXSpeed, vel.maxXSpeed);
    // std::clamp(vel.velocity.y, -vel.maxYSpeed, vel.maxYSpeed);
    if (vel.velocity.x > vel.maxXSpeed) {
      vel.velocity.x = vel.maxXSpeed;
    }
    if (vel.velocity.x < -vel.maxXSpeed) {
      vel.velocity.x = -vel.maxXSpeed;
    }
    if (vel.velocity.y > vel.maxYSpeed) {
      vel.velocity.y = vel.maxYSpeed;
    }
    if (vel.velocity.y < -vel.maxYSpeed) {
      vel.velocity.y = -vel.maxYSpeed;
    }

    // Velocity
    if (input.has) {
      if ((std::abs(input.axis.x) > 0 || std::abs(input.axis.y) > 0) &&
          vel.currentSpeed < vel.maxYSpeed && vel.currentSpeed > -vel.maxYSpeed) {
        vel.currentSpeed += vel.acceleration;
      }
      if ((std::abs(input.axis.x) > 0) &&
          vel.currentSpeed<vel.maxXSpeed & vel.currentSpeed> - vel.maxXSpeed) {
        vel.currentSpeed += vel.acceleration;
      }

      if (vel.currentSpeed > vel.maxXSpeed) {
        vel.currentSpeed = vel.maxXSpeed;
      }
      if (vel.currentSpeed < -vel.maxXSpeed) {
        vel.currentSpeed = -vel.maxXSpeed;
      }

      if (input.directionChanged || input.axis.length() == 0) {
        vel.currentSpeed = 0;
      }

      vel.velocity += input.axis * vel.currentSpeed;

    }

    // Jumping
    if (col.touchedGround) {
      vel.midJump = false;
      if (vel.canJump && input.jump) {
        vel.canJump = false;
        if (input.has) {
          input.jumpCountdown = input.jumpDuration;
        }
      }
    }

    if (input.has) {
      if (!input.jump) {
        if (!col.touchedGround) {
          vel.midJump = true;
        } else {
          vel.canJump = true;
        }
      }
      if (input.jumpCountdown >= 0) {
        --input.jumpCountdown;
      }
    }

    if (col.touchedCeiling) {
      vel.midJump = true;
      vel.velocity.y = 0;
    }

    if (input.has) {
      if (input.jump && !vel.midJump && input.jumpCountdown >= 0) {
        vel.velocity.y -= vel.jumpForce;
      }
    }
  }
}

void Scene_Play::sGravity() {
  for (const std::shared_ptr<Entity> e : m_entities.get_entities()) {
    const CGravity &gravity = e->getComponent<CGravity>();
    const CCharacterController &controller = e->getComponent<CCharacterController>();
    CVelocity &velocity = e->getComponent<CVelocity>();
    if (!velocity.has || !gravity.has) {
      continue;
    }
    if (controller.has && controller.touchedGround) {
      velocity.velocity.y = 0;
      continue;
    }
    velocity.velocity.y += gravity.acceleration;
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
    if (e_data.name == "Box") {
      std::cout << "Spawning box at: " << e_data.x << ":" << e_data.y << std::endl;
      spawn_box({(float)e_data.x, (float)e_data.y}, true);
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
  m_player->addComponent<CAnimationState>();
  m_player->addComponent<CBoundingBox>(p_sprite.getSize().x,
                                       p_sprite.getSize().y);
  m_player->addComponent<CCharacterController>();
  m_player->addComponent<CStaticCollision>(CStaticCollision::CollisionLayer::PLAYER);
  // std::cout << "PLAYER LAYER: " << m_player->getComponent<CStaticCollision>().layer << std::endl;
  m_player->addComponent<CInput>();
  m_player->addComponent<CGravity>();
  AnimatedSprite bullet_sprite = AnimatedSprite();
  bullet_sprite.load_file("resources/Projectiles.atlas", m_game->getAssets());
  bullet_sprite.setRepeat(true);
  bullet_sprite.play("SpikeIdle", 10);
  m_player->addComponent<CWeapon>(
    bullet_sprite, // Prefab
    1, // damage
    20, // fire rate
    Vec2(10, 0), // velocity
    0, // gravity
    10, // offset
    true // kinematic
  );
}

void Scene_Play::spawn_box(Vec2 position, bool snap_to_grid) {
  std::cout << "spawning box entity.\n";
  if (snap_to_grid) {
    position = m_level.snap_to_grid(position, SNAP_CENTER);
  }
  const std::shared_ptr<Entity> box = m_entities.add_entity(Tag::StaticCollision);
  box->addComponent<CTransform>(position);
  box->addComponent<CAnimatedSprite>();
  AnimatedSprite &b_sprite = box->getComponent<CAnimatedSprite>().sprite;
  b_sprite = AnimatedSprite();
  b_sprite.load_file("resources/Box.atlas", m_game->getAssets());
  b_sprite.setRepeat(false);
  b_sprite.play("Idle", 10);
  box->addComponent<CBoundingBox>(b_sprite.getSize().x,
                                       b_sprite.getSize().y);
  box->addComponent<CStaticCollision>(CStaticCollision::CollisionLayer::WORLD);
  box->addComponent<CHitReaction>("Hit", 3);
  std::cout << "BOX ID: " << box->id() << std::endl;
  AnimatedSprite box_part_a = AnimatedSprite(b_sprite);
  AnimatedSprite box_part_b = AnimatedSprite(b_sprite);
  AnimatedSprite box_part_c = AnimatedSprite(b_sprite);
  AnimatedSprite box_part_d = AnimatedSprite(b_sprite);
  box_part_a.play("DestroyedPartA", 10);
  box_part_b.play("DestroyedPartB", 10);
  box_part_c.play("DestroyedPartC", 10);
  box_part_d.play("DestroyedPartD", 10);
  const std::vector<AnimatedSprite> parts = {
    box_part_a,
    box_part_b,
    box_part_c,
    box_part_d
  };
  box->addComponent<CDeathReaction>("Destroyed", 10, parts);
  box->addComponent<CHealth>(2);
}

void Scene_Play::spawn_collision(const Level &level) {
  Vec2 cell_size = {(float)level.getTileset().tileWidth,
                    (float)level.getTileset().tileHeight};
  int width = level.getDimensions().x;
  int height = level.getDimensions().y;
  const std::vector<sf::IntRect> edges = level.getEdgeBounds();
  // const std::vector<int> edge_cells = level.getEdges();
  // const std::vector<sf::IntRect> edges =
  // level.getCombinedColliders(edge_cells);
  for (const sf::IntRect &rect : edges) {
    const auto collision = m_entities.add_entity(Tag::StaticCollision);
    collision->addComponent<CTransform>(Vec2(rect.left + rect.width / 2, rect.top + rect.height / 2));
    const sf::IntRect &norm_rect = {0, 0, rect.width, rect.height};
    collision->addComponent<CBoundingBox>(norm_rect);
    collision->addComponent<CStaticCollision>(
        m_level.getCollisionDirection(rect));
  }
}

void Scene_Play::onShoot(const std::shared_ptr<Entity> entity) {
  CWeapon & weapon = entity->getComponent<CWeapon>();
  CTransform & xform = entity->getComponent<CTransform>();
  CStaticCollision & col = entity->getComponent<CStaticCollision>();
  if (!weapon.has || !xform.has || !col.has) {
    return;
  }
  if (weapon.countdown <= 0) {
    const std::shared_ptr<Entity> bullet = m_entities.add_entity(Tag::Bullet);
    int direction = 1;
    if (xform.direction == CTransform::Direction::LEFT) {
      direction = -1;
    }
    Vec2 pos = {xform.pos.x + weapon.offset * direction, xform.pos.y};
    bullet->addComponent<CTransform>(pos);
    bullet->addComponent<CAnimatedSprite>(weapon.bullet);
    bullet->addComponent<CCharacterController>();
    bullet->addComponent<CStaticCollision>(col.layer);
    bullet->addComponent<CBoundingBox>(weapon.bullet.getSize().x, weapon.bullet.getSize().y);
    bullet->addComponent<CLifespan>(100);
    // std::cout << "Shoot velocity x: " << weapon.velocity.x * direction << std::endl;
    bullet->addComponent<CVelocity>(weapon.velocity.x * direction, weapon.velocity.y, weapon.kinematic);
    if (weapon.gravity > 0) {
      bullet->addComponent<CGravity>(weapon.gravity);
    }
    bullet->addComponent<CBullet>(weapon.damage);
    bullet->addComponent<CHealth>(1);
    weapon.countdown = weapon.fireRate;
  }
}

void Scene_Play::onEnd() {
  m_game->getWindow().setView(m_game->getWindow().getDefaultView());
  m_game->changeScene("MENU");
}

void Scene_Play::sAnimation() {
  if (!m_sAnimation) { return; }
  
  for (const std::shared_ptr<Entity> p : m_entities.get_entities(Tag::Player)) {
    CTransform &p_xform = p->getComponent<CTransform>();
    CAnimatedSprite &p_sprite = p->getComponent<CAnimatedSprite>();
    CAnimationState &p_state = p->getComponent<CAnimationState>();
    CCharacterController &p_col = p->getComponent<CCharacterController>();
    CVelocity &p_vel = m_player->getComponent<CVelocity>();
    CInput &p_input = m_player->getComponent<CInput>();
    if (
      !p_xform.has
      || !p_sprite.has 
      || !p_state.has 
      || !p_col.has 
      || !p_vel.has 
      || !p_input.has
    ) { return; }

    p_state.prevState = p_state.state;
    if (!p_col.touchedGround) {
      if (p_vel.velocity.y < 0) {
        p_state.state = CAnimationState::AnimState::ANIM_STATE_JUMP;
      } else {
        p_state.state = CAnimationState::AnimState::ANIM_STATE_FALL;
      }
    } else {
      if (p_input.axis.x != 0) {
        p_state.state = CAnimationState::AnimState::ANIM_STATE_RUN;
      } else {
        p_state.state = CAnimationState::AnimState::ANIM_STATE_IDLE;
      }
    }

    if (p_state.prevState != p_state.state) {
      switch (p_state.state) {
        case (CAnimationState::AnimState::ANIM_STATE_IDLE): {
          p_sprite.sprite.play("Idle", 10);
          p_sprite.sprite.setRepeat(true);
          break;
        }
        case (CAnimationState::AnimState::ANIM_STATE_JUMP): {
          p_sprite.sprite.play("Jump", 10);
          p_sprite.sprite.setRepeat(false);
          break;
        }
        case (CAnimationState::AnimState::ANIM_STATE_FALL): {
          p_sprite.sprite.play("Fall", 10);
          p_sprite.sprite.setRepeat(false);
          break;
        }
        case (CAnimationState::AnimState::ANIM_STATE_RUN): {
          p_sprite.sprite.play("Run", 2);
          p_sprite.sprite.setRepeat(true);
          break;
        }
      }
    }

    if (p_input.directionChanged) {
      if (p_input.axis.x > 0) {
        p_sprite.sprite.setDirection(true);
        p_xform.direction = CTransform::Direction::RIGHT;
      } else if (p_input.axis.x < 0) {
        p_sprite.sprite.setDirection(false);
        p_xform.direction = CTransform::Direction::LEFT;
      }
    }
    
  }
}

void Scene_Play::sRender() {
  sf::RenderWindow &window = m_game->getWindow();

  if (m_sRender) {
    for (const auto &entity : m_entities.get_entities()) {
      CAnimatedSprite &anim_sprite = entity->getComponent<CAnimatedSprite>();
      CTransform &transform = entity->getComponent<CTransform>();
      if (!anim_sprite.has || !transform.has) {
        continue;
      }
      if (m_frameCount % anim_sprite.sprite.getSpeed() == 0) {
        anim_sprite.sprite.update();
      }
      anim_sprite.sprite.setPosition(transform.pos);
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
      const CStaticCollision &col = entity->getComponent<CStaticCollision>();
      const CCharacterController &controller = entity->getComponent<CCharacterController>();
      sf::Color outlineColor = sf::Color(0, 255, 0);
      if (col.has && col.processed) {
        outlineColor = sf::Color(255, 100, 0);
      }
      if (controller.has) {
        outlineColor = sf::Color(100, 0, 255);
      }
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
      s_box.setFillColor(sf::Color(0, 255, 0, 20));
      s_box.setOutlineColor(outlineColor);
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
  for (const std::shared_ptr<Entity> e : m_entities.get_entities()) {
    CCharacterController &controller = e->getComponent<CCharacterController>();
    if (!controller.has) {
      continue;
    }
    CStaticCollision &col = e->getComponent<CStaticCollision>();
    const CBoundingBox &bbox = e->getComponent<CBoundingBox>();
    CTransform &xform = e->getComponent<CTransform>();
    if (!controller.has || !bbox.has || !xform.has) {
      continue;
    }

    std::map<float, std::tuple<size_t, Overlap, std::shared_ptr<Entity>>>
        static_overlaps;

    // Collision detection:
    for (const std::shared_ptr<Entity> wall :
         m_entities.get_entities()) {
      CStaticCollision &w_col = wall->getComponent<CStaticCollision>();
      const CTransform &w_xform = wall->getComponent<CTransform>();
      const CBoundingBox &w_bbox = wall->getComponent<CBoundingBox>();
      if (!w_col.has || !w_bbox.has || !w_xform.has) {
        std::cerr << "Static collision doesn't have required components\n";
        continue;
      }
      if (col.layer == w_col.layer) {
        // std::cout << "Player layer: " << col.layer << "; wall layer: " << w_col.layer << " id: " << wall->id() << std::endl;
        continue;
      }
      w_col.processed = false;
      // std::cout << "PROCESSING COLLISION " << wall->id() << std::endl;

      int w_radius = std::max(w_bbox.halfSize.x, w_bbox.halfSize.y);
      Vec2 w_center = {w_xform.pos.x + w_bbox.rect.left + w_bbox.halfSize.x,
                       w_xform.pos.y + w_bbox.rect.top + w_bbox.halfSize.y};
      int radius = std::max(bbox.halfSize.x, bbox.halfSize.y);
      float distance = xform.pos.distance_to(w_center) - w_radius - radius;

      float x_a = w_xform.pos.x + w_bbox.halfSize.x -
                  (xform.pos.x - bbox.halfSize.x); // Player to the right
      float x_b = xform.pos.x + bbox.halfSize.x -
                  (w_xform.pos.x - w_bbox.halfSize.x); // player to the left
      float y_a = w_xform.pos.y + w_bbox.halfSize.y -
                  (xform.pos.y - bbox.halfSize.y); // player below
      float y_b = xform.pos.y + bbox.halfSize.y -
                  (w_xform.pos.y - w_bbox.halfSize.y); // player on top

      Overlap overlap = {x_a, x_b, y_a, y_b};

      if (distance < controller.distance) {
        if (static_overlaps.find(distance) != static_overlaps.end()) {
          // std::cerr << "Overlap with equal distance already exists: "
          //           << distance << "\n";
          while (static_overlaps.find(distance) != static_overlaps.end()) {
            float rand_dist = (float)(rand()) / (float)RAND_MAX;
            distance += rand_dist;
          }
          // std::cerr << "Adjusting by random value: " << distance << "\n";
        }
        static_overlaps[distance] =
            std::make_tuple(wall->id(), overlap, wall);
      }
    }

    // std::cout << "before resolve collisions" << std::endl;
    resolve_collision(e, static_overlaps);
    // std::cout << "after resolve collisions" << std::endl;
  }
}

void Scene_Play::sHitReaction() {
  for (const std::shared_ptr<Entity> e : m_entities.get_entities()) {
    const CStaticCollision & static_col = e->getComponent<CStaticCollision>();
    if (!static_col.has) {
      continue;
    }
    for (auto p : static_col.collidedThisFrame) {
      const std::shared_ptr<Entity> collided = m_entities.get_entity(p.first);
      if (!collided) {
        continue;
      }
      if (p.second == StaticCollisionDirection::bottom) {
        if (collided->tag() == Tag::Player) {
          onHit(e, 1);
        }
      }
      if (collided && collided->tag() == Tag::Bullet) {
        const CBullet bullet = collided->getComponent<CBullet>();
        if (bullet.has) {
          onHit(e, bullet.damage);
          onHit(collided, 1);
        }
      }
    }
  }
}

void Scene_Play::onHit(std::shared_ptr<Entity> entity, const int damage) {
  CAnimatedSprite &sprite = entity->getComponent<CAnimatedSprite>();
  CHitReaction &hr = entity->getComponent<CHitReaction>();
  if (sprite.has && hr.has) {
    const std::string prev_animation = sprite.sprite.getAnimation();
    const unsigned int prev_speed = sprite.sprite.getSpeed();
    sprite.sprite.play(hr.animation, hr.speed);
    sprite.sprite.setRepeat(false);
    sprite.sprite.setCallback([&sprite, prev_animation, prev_speed](){
      std::cout << "CALLBACK, ASSIGN " << prev_animation << std::endl;
      sprite.sprite.play(prev_animation, prev_speed);
    });
  }
  CHealth &health = entity->getComponent<CHealth>();
  if (health.has) {
    health.hp -= damage;
    if (health.hp <= 0) {
      onDeath(entity);
    }
  }
}

void Scene_Play::onDeath(std::shared_ptr<Entity> entity) {
  CAnimatedSprite &sprite = entity->getComponent<CAnimatedSprite>();
  CDeathReaction &dr = entity->getComponent<CDeathReaction>();
  CTransform &xform = entity->getComponent<CTransform>();
  if (!xform.has) {
    return;
  }
  const std::weak_ptr<Entity> entity_ptr = entity;
  const std::function<void()> deathCallback = [entity_ptr, this](){
    std::cout << "CALLBACK, ON DEATH" << std::endl;
    const std::shared_ptr<Entity> entity = entity_ptr.lock();
    if (!entity) {
      return;
    }
    const CDeathReaction &dr = entity->getComponent<CDeathReaction>();
    const CTransform &xform = entity->getComponent<CTransform>();
    if (dr.has) {
      for (const AnimatedSprite & sprite : dr.spawn_prefabs) {
        const std::shared_ptr<Entity> p = m_entities.add_entity(Tag::Particle);
        p->addComponent<CTransform>(xform.pos);
        p->addComponent<CAnimatedSprite>(sprite);
        p->addComponent<CCharacterController>();
        p->addComponent<CStaticCollision>(CStaticCollision::CollisionLayer::PLAYER);
        p->addComponent<CBoundingBox>(sprite.getSize().x, sprite.getSize().y);
        p->addComponent<CLifespan>(100);
        const float rand_x = sprite.getSize().x * ((float)rand() / (float)RAND_MAX - 0.5);
        const float rand_y = -sprite.getSize().y * (float)rand() / (float)RAND_MAX;
        p->addComponent<CVelocity>(rand_x, rand_y);
        p->addComponent<CGravity>(1.f);
        std::cout << "spawned id: " << p->id() << std::endl;
      }
    }
    entity->destroy();
  };
  if (sprite.has && dr.has) {
    sprite.sprite.play(dr.animation, dr.speed);
    sprite.sprite.setRepeat(false);
    sprite.sprite.setCallback(deathCallback);
  } else {
    deathCallback();
  }
}

void Scene_Play::sLifespan() {
  for (const std::shared_ptr<Entity> e : m_entities.get_entities()) {
    CLifespan & ls = e->getComponent<CLifespan>();
    if (!ls.has) {
      continue;
    }

    if (ls.countdown <= 0) {
      e->destroy();
    }

    CAnimatedSprite & sprite = e->getComponent<CAnimatedSprite>();
    if (sprite.has) {
      sf::Sprite & s = sprite.sprite.getSprite();
      const sf::Color prevColor = s.getColor();
      const float alpha = std::abs(255 * ((float)ls.countdown / ls.life));
      s.setColor(sf::Color(prevColor.r, prevColor.g, prevColor.b, alpha));
    }

    --ls.countdown;
  }
}

void Scene_Play::sCountdowns() {
  for (const std::shared_ptr<Entity> e: m_entities.get_entities()) {
    CLifespan & ls = e->getComponent<CLifespan>();
    if (ls.has) {
      --ls.countdown;
    }
    CWeapon &w = e->getComponent<CWeapon>();
    if (w.has) {
      --w.countdown;
    }
  }
}

void Scene_Play::sDebug() {
  ImGui::Begin("Platformer Project Debug");
  ImGui::SeparatorText("Gameplay scene");
  ImGui::Checkbox("Render map", &m_sRenderMap);
  ImGui::Checkbox("Render grid", &m_sDebugGrid);
  ImGui::Checkbox("Render collision", &m_sDebugCollision);
  if (ImGui::BeginTabBar("Selection")) {
    if (ImGui::BeginTabItem("Player")) {
      ImGui::BeginChild("Player", {0, 0},
                        ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
      if (m_player) {
        CTransform &xform = m_player->getComponent<CTransform>();
        CVelocity &vel = m_player->getComponent<CVelocity>();
        CInput &input = m_player->getComponent<CInput>();
        CGravity &grav = m_player->getComponent<CGravity>();
        CCharacterController &col = m_player->getComponent<CCharacterController>();
        if (grav.has) {
          static bool grav_on = true;
          static float prev_acc = grav.acceleration;
          ImGui::SeparatorText("Player graivty");
          if (ImGui::Checkbox("On", &grav_on)) {
            if (grav_on) {
              grav.acceleration = prev_acc;
            } else {
              grav.acceleration = 0;
            }
          }
          ImGui::InputFloat("acceleration", &grav.acceleration);
        }
        if (input.has) {
          float axis[2] = {input.axis.x, input.axis.y};
          float prevAxis[2] = {input.prevAxis.x, input.prevAxis.y};
          ImGui::SeparatorText("Player input");
          ImGui::InputFloat2("axis", axis);
          ImGui::InputFloat2("prevAxis", prevAxis);
          ImGui::Checkbox("directionChanged", &input.directionChanged);
          ImGui::Checkbox("jump", &input.jump);
          ImGui::InputInt("jumpDuration", &input.jumpDuration);
          ImGui::InputInt("jumpCountdown", &input.jumpCountdown);
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
          ImGui::InputFloat("maxXSpeed", &vel.maxXSpeed, 0.01f, 0.1f);
          ImGui::InputFloat("maxYSpeed", &vel.maxYSpeed, 0.01f, 0.1f);
          ImGui::InputFloat("currentSpeed", &vel.currentSpeed);
          ImGui::InputFloat("acceleration", &vel.acceleration, 0.01f, 0.1f);
          ImGui::InputFloat("deceleration", &vel.deceleration, 0.01f, 0.1f);
          ImGui::InputFloat("margin", &vel.margin, 0.01f, 0.1f);
          ImGui::InputFloat("jumpForce", &vel.jumpForce, 0.01f, 0.1f);
          ImGui::Checkbox("midJump", &vel.midJump);
          ImGui::Checkbox("canJump", &vel.canJump);
        }
        if (col.has) {
          ImGui::SeparatorText("Player CharacterController");
          ImGui::Checkbox("touchedGround", &col.touchedGround);
        }
      }
      ImGui::EndChild();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Entities")) {
      ImGui::BeginChild("Entity list", {400.f, 0}, true);
      static int entity_idx = -1;
      static int entity_collection = -1;
      if (ImGui::BeginTabBar("Entity collections", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("All")) {
          if (ImGui::BeginTable("Entity table", 1)) {
            const Entities &entities = m_entities.get_entities();
            for (int i = 0; i < entities.size(); ++i) {
              const bool is_selected = (entity_idx == i);
              const std::shared_ptr<Entity> entity = entities[i];
              const std::string tag_name = name_tags[entity->tag()];
              const int entity_id = entity->id();
              ImGui::TableNextColumn();
              char bufdel[32];
              sprintf(bufdel, "D ##del%d", i);
              if (ImGui::Button(bufdel)) {
                entity->destroy();
              }
              ImGui::SameLine();
              char bufname[32];
              sprintf(bufname, "%s (%d)##entity%d", tag_name.c_str(), entity_id,
                      entity_id);
              if (ImGui::Selectable(bufname, is_selected)) {
                entity_idx = i;
                entity_collection = -1;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndTable();
          }
          ImGui::EndTabItem();
        }
        for (auto &tag_r : tag_names) {
          const std::string tag_name = tag_r.first;
          const Tag tag = tag_r.second;
          if (ImGui::BeginTabItem(tag_name.c_str())) {
            if (ImGui::BeginTable("Entity table", 1)) {
              const Entities &entities = m_entities.get_entities(tag);
              for (int i = 0; i < entities.size(); ++i) {
                const bool is_selected = (entity_idx == i);
                const std::shared_ptr<Entity> entity = entities[i];
                const std::string tag_name = name_tags[entity->tag()];
                const int entity_id = entity->id();
                ImGui::TableNextColumn();
                char bufdel[32];
                sprintf(bufdel, "D ##del%d", i);
                if (ImGui::Button(bufdel)) {
                  entity->destroy();
                }
                ImGui::SameLine();
                char bufname[32];
                sprintf(bufname, "%s (%d)##entity%d", tag_name.c_str(),
                        entity_id, entity_id);
                if (ImGui::Selectable(bufname, is_selected)) {
                  entity_idx = i;
                  entity_collection = (int)tag;
                }
                if (is_selected) {
                  ImGui::SetItemDefaultFocus();
                }
              }
              ImGui::EndTable();
            }
            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      ImGui::EndChild();

      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
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
