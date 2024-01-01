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
  Vec2 velocity = {0.f, 0.f};
  float angle = 0;
  CTransform() {}
  CTransform(const Vec2 &in_pos) : pos(in_pos) {}
  ~CTransform() {}
};

class CBoundingBox : public Component {
public:
  CBoundingBox() {}
  ~CBoundingBox() {}
};

class CAnimatedSprite : public Component {
public:
  AnimatedSprite sprite;
  CAnimatedSprite() {}
  ~CAnimatedSprite() {}
};
