#pragma once
#include "animated_sprite.h"
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
  CBoundingBox() {}
  CBoundingBox(const sf::IntRect & in_rect)
    : rect(in_rect) 
  { }
  CBoundingBox(const int width, const int height)
    : rect(sf::IntRect(0, 0, width, height))
  { }
  CBoundingBox(const int x, const int y, const int width, const int height)
    : rect(sf::IntRect(x, y, width, height))
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
  CStaticCollision(){}
  ~CStaticCollision(){}
};

class CDynamicCollision : public Component {
public:
  CDynamicCollision(){}
  ~CDynamicCollision(){}
};
