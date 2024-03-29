#pragma once
#include "SFML/Graphics/Rect.hpp"
#include "assets.h"
#include "vec2.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>

class AnimatedSprite {
  sf::Sprite m_sprite;
  unsigned int m_currentFrame{0};
  std::shared_ptr<std::vector<sf::IntRect>> m_currentFrames{nullptr};
  unsigned int m_frameCount;
  unsigned int m_speed{10};
  bool m_paused{true};
  bool m_hasEnded{false};
  std::string m_currentAnimation{"None"};
  std::unordered_map<std::string, std::vector<sf::IntRect>> m_animations;
  Vec2 m_size;
  Vec2 m_imageSize;
  bool m_repeat{false};
  void onEnd();
  std::function<void()> onEndCallback {[](){}};

public:
  AnimatedSprite(){};
  // AnimatedSprite &operator=(AnimatedSprite &o) {
  //   if (this != &o) {
  //     m_currentFrames = std::move(o.m_currentFrames);
  //   }
  //   return *this;
  // }
  // AnimatedSprite(AnimatedSprite &o)
  //     : m_currentFrames(std::move(o.m_currentFrames)) {}
  // AnimatedSprite &operator=(AnimatedSprite &&o) {
  //   if (this != &o) {
  //     m_currentFrames = std::move(o.m_currentFrames);
  //   }
  //   return *this;
  // }
  // AnimatedSprite(AnimatedSprite &&o)
  //     : m_currentFrames(std::move(o.m_currentFrames)) {}
  ~AnimatedSprite(){};

  bool load_file(const std::string &path, Assets &assets);
  void test_data();

  void update();
  bool hasEnded();
  const Vec2 &getSize() const;
  sf::Sprite &getSprite();
  const std::vector<std::string> getAnimationNames();
  bool play(const std::string &anim_name, const int speed);
  void pause();
  void setRepeat(bool repeat);
  void setPosition(const Vec2 &pos);
  void setDirection(bool right);
  void setCallback(std::function<void()> in_callback);
  const std::string & getAnimation() const;
  const unsigned int getSpeed() const;
};
