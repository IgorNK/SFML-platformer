#pragma once
#include "animated_sprite.h"
#include "vec2.h"
#include "flags.h"

class Component {
public:
  bool has = false;
};

class CTransform : public Component {
public:
  Vec2 pos = {0.f, 0.f};
  Vec2 prevPos = {0.f, 0.f};
  bool skipPrevPosition = false;
  Vec2 scale = {1.f, 1.f};
  float angle = 0;
  CTransform() {}
  CTransform(const Vec2 &in_pos) : pos(in_pos) {}
  ~CTransform() {}
};

class CVelocity : public Component {
public:
  Vec2 velocity = {0, 0};
  float maxSpeed = 1.5f;
  float currentSpeed = 0.f;
  float acceleration = 1.0f;
  float deceleration = 0.2f;
  float margin = 0.1f;
  CVelocity() {}
  ~CVelocity() {}
};

class CBoundingBox : public Component {
public:
  sf::IntRect rect {0, 0, 32, 32};
  Vec2 halfSize {16, 16};
  CBoundingBox() {}
  CBoundingBox(const sf::IntRect & in_rect)
    : rect(in_rect)
    , halfSize(in_rect.width / 2, in_rect.height / 2)
  { }
  CBoundingBox(const int width, const int height)
    : rect({0, 0, width, height})
    , halfSize(width / 2, height / 2)
  { }
  CBoundingBox(const int x, const int y, const int width, const int height)
    : rect({x, y, width, height})
    , halfSize(width / 2, height / 2)
  { }
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
  Vec2 axis {0, 0};
  Vec2 prevAxis {0, 0};
  bool directionChanged {false};
  CInput() {}
  ~CInput() {}
};


class CStaticCollision : public Component {
public:
  EStaticCollisionDirection direction;
  CStaticCollision(){}
  ~CStaticCollision(){}
};

struct Overlap {
  float right;
  float left;
  float bottom;
  float top;
};

class CDynamicCollision : public Component {
public:
  float distance {100};
  int maxColliders {12};
  std::unordered_map<size_t, Overlap> prevOverlap {};
  CDynamicCollision(){}
  ~CDynamicCollision(){}
};
