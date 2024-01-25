#pragma once
#include "animated_sprite.h"
#include "flags.h"
#include "vec2.h"

class Component {
public:
  bool has = false;
};

class CTransform : public Component {
public:
  enum Direction {
    LEFT,
    RIGHT,
  };
  Vec2 pos = {0.f, 0.f};
  Vec2 prevPos = {0.f, 0.f};
  Vec2 scale = {1.f, 1.f};
  float angle = 0;
  Direction direction{Direction::RIGHT};
  CTransform() {}
  CTransform(const Vec2 &in_pos) : pos(in_pos) {}
  ~CTransform() {}
};

class CVelocity : public Component {
public:
  Vec2 velocity = {0, 0};
  float maxXSpeed = 1.5f;
  float maxYSpeed = 6.0f;
  float currentSpeed = 0.f;
  float acceleration = 1.0f;
  float deceleration = 0.2f;
  float margin = 0.1f;
  float jumpForce = 3.0f;
  bool midJump{false};
  bool canJump{true};
  CVelocity() {}
  CVelocity(const float x, const float y) 
    : velocity(Vec2(x, y))
    , maxXSpeed(std::abs(x))
    , maxYSpeed(std::max(std::abs(y), maxYSpeed)) {}
  CVelocity(const float x, const float y, bool kinematic) 
    : velocity(Vec2(x, y))
    , maxXSpeed(std::abs(x))
    , maxYSpeed(std::max(std::abs(y), maxYSpeed)) {
    if (kinematic) {
      deceleration = 0.f;
    }
  }
  ~CVelocity() {}
};

class CBoundingBox : public Component {
public:
  sf::IntRect rect{0, 0, 32, 32};
  Vec2 halfSize{16, 16};
  CBoundingBox() {}
  CBoundingBox(const sf::IntRect &in_rect)
      : rect(in_rect),
        halfSize((float)in_rect.width / 2, (float)in_rect.height / 2) {}
  CBoundingBox(const int width, const int height)
      : rect({0, 0, width, height}),
        halfSize((float)width / 2, (float)height / 2) {}
  CBoundingBox(const int x, const int y, const int width, const int height)
      : rect({x, y, width, height}),
        halfSize((float)width / 2, (float)height / 2) {}
  ~CBoundingBox() {}
};

class CAnimatedSprite : public Component {
public:
  AnimatedSprite sprite;
  CAnimatedSprite() {}
  CAnimatedSprite(const AnimatedSprite & in_sprite) : sprite(in_sprite) {}
  ~CAnimatedSprite() {}
};

class CAnimationState: public Component {
public:
  enum AnimState {
    ANIM_STATE_IDLE,
    ANIM_STATE_JUMP,
    ANIM_STATE_FALL,
    ANIM_STATE_RUN
  };
  AnimState state { AnimState::ANIM_STATE_IDLE };
  AnimState prevState { AnimState::ANIM_STATE_IDLE };
  CAnimationState() {}
  ~CAnimationState() {}
};

class CInput : public Component {
public:
  Vec2 axis{0, 0};
  Vec2 prevAxis{0, 0};
  bool directionChanged{false};
  bool jump{false};
  int jumpDuration{10};
  int jumpCountdown{0};
  CInput() {}
  ~CInput() {}
};

class CStaticCollision : public Component {
public:
  enum CollisionLayer {
    PLAYER,
    ENEMIES,
    WORLD,
  };
  CollisionLayer layer {CollisionLayer::WORLD};
  EStaticCollisionDirection direction;
  std::unordered_map<size_t, StaticCollisionDirection> collidedThisFrame;
  bool processed {false};
  CStaticCollision() {}
  CStaticCollision(const EStaticCollisionDirection dir) : direction(dir) {}
  CStaticCollision(const EStaticCollisionDirection dir, const CollisionLayer in_layer) : direction(dir), layer(in_layer) {}
  CStaticCollision(const CollisionLayer in_layer)
    : layer(in_layer) {
      EStaticCollisionDirection dir;
      dir.set(StaticCollisionDirection::top, true);
      dir.set(StaticCollisionDirection::right, true);
      dir.set(StaticCollisionDirection::bottom, true);
      dir.set(StaticCollisionDirection::left, true);
      direction = dir;
  }
  ~CStaticCollision() {}
};

struct Overlap {
  float right;
  float left;
  float bottom;
  float top;
};

class CCharacterController : public Component {
public:
  float distance{640000}; // Distance from which to grab collision to process
  int maxColliders{12}; // Maximum colliders to process
  bool touchedGround{false};
  bool touchedCeiling{false};
  EStaticCollisionDirection collidedThisFrame;
  std::unordered_map<size_t, Overlap> prevOverlap{};
  CCharacterController() {}
  ~CCharacterController() {}
};

class CGravity : public Component {
public:
  float acceleration{0.5f};
  CGravity() {}
  CGravity(const float in_acceleration) : acceleration(in_acceleration) {}
  ~CGravity() {}
};

class CHealth : public Component {
public:
  int hp {10};
  CHealth() {}
  CHealth(const int in_hp) : hp(in_hp) {}
  ~CHealth() {}
};

class CDeathReaction : public Component {
public:
  std::string animation {""};
  unsigned int speed {10};
  std::vector<AnimatedSprite> spawn_prefabs {};
  CDeathReaction() {}
  CDeathReaction(const std::string & anim_name, const int anim_speed, const std::vector<AnimatedSprite> & prefabs) 
    : animation(anim_name)
    , speed(anim_speed)
    , spawn_prefabs(prefabs) {}
  ~CDeathReaction() {}
};

class CHitReaction : public Component {
public:
  std::string animation {""};
  unsigned int speed {10};
  CHitReaction() {}
  CHitReaction(const std::string & anim_name, const int anim_speed) 
    : animation(anim_name)
    , speed(anim_speed) {}
  ~CHitReaction() {}
};

class CLifespan : public Component {
public:
  int life {100};
  int countdown {100};
  CLifespan() {}
  CLifespan(const int in_life) : life(in_life), countdown(in_life) {}
  ~CLifespan() {}
};

class CWeapon : public Component {
public:
  AnimatedSprite bullet;
  int damage {1};
  int fireRate {20};
  int countdown {0};
  Vec2 velocity {10, 0};
  int gravity {0};
  bool kinematic {true};
  int offset {0};
  CWeapon() {}
  CWeapon(
    const AnimatedSprite & bullet_prefab, 
    const int in_damage, 
    const int in_rate, 
    const Vec2 in_velocity, 
    const int in_gravity, 
    const int in_offset, 
    const bool in_kinematic
  )
    : bullet(bullet_prefab)
    , damage(in_damage)
    , fireRate(in_rate)
    , velocity(in_velocity)
    , gravity(in_gravity)
    , offset(in_offset)
    , kinematic(in_kinematic) {}
  ~CWeapon() {}
};

class CBullet : public Component {
public:
  int damage {1};
  CBullet() {}
  CBullet(const int in_damage) : damage(in_damage) {}
  ~CBullet() {}
};
