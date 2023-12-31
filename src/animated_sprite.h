#pragma once
#include <SFML/Graphics.hpp>
#include "SFML/Graphics/Rect.hpp"
#include "vec2.h"

class AnimatedSprite {
    sf::Sprite m_sprite;
    size_t m_currentFrame;
    size_t m_speed;
    std::string m_currentAnimation;
    std::map<std::string, std::vector<sf::IntRect>> m_animations;
    Vec2 m_size;
    Vec2 m_imageSize;
    std::string m_repeat;
public:
    AnimatedSprite()
        : m_currentFrame(0)
    { }
    ~AnimatedSprite() {}

    bool load_file(const std::string & path);
    void test_data();

    void update();
    bool hasEnded();
    const Vec2 & getSize() const;
    sf::Sprite & getSprite();
};