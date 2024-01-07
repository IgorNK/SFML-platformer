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
  Vec2 pos = {0.f, 0.f};
  Vec2 prevPos = {0.f, 0.f};
  Vec2 scale = {1.f, 1.f};
  float angle = 0;
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
  ~CAnimatedSprite() {}
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
  EStaticCollisionDirection direction;
  CStaticCollision() {}
  CStaticCollision(const EStaticCollisionDirection dir) : direction(dir) {}
  ~CStaticCollision() {}
};

struct Overlap {
  float right;
  float left;
  float bottom;
  float top;
};

class CDynamicCollision : public Component {
public:
  float distance{100};
  int maxColliders{12};
  bool touchedGround{false};
  bool touchedCeiling{false};
  EStaticCollisionDirection collidedThisFrame;
  std::unordered_map<size_t, Overlap> prevOverlap{};
  CDynamicCollision() {}
  ~CDynamicCollision() {}
};

class CGravity : public Component {
public:
  float acceleration{0.5f};
  CGravity() {}
  CGravity(const float in_acceleration) : acceleration(in_acceleration) {}
  ~CGravity() {}
};
