#pragma once
#include <SFML/Graphics.hpp>
#include "SFML/Graphics/Rect.hpp"
#include "vec2.h"

class AnimatedSprite {
    sf::Sprite m_sprite;
    size_t m_currentFrame;
    std::vector<sf::IntRect>* m_currentFrames;
    size_t m_frameCount;
    size_t m_speed;
    bool m_paused {true};
    bool m_hasEnded {false};
    std::string m_currentAnimation {"None"};
    std::map<std::string, std::vector<sf::IntRect>> m_animations;
    Vec2 m_size;
    Vec2 m_imageSize;
    bool m_repeat {false};
    void onEnd();
public:
    AnimatedSprite();
    ~AnimatedSprite();

    bool load_file(const std::string & path);
    void test_data();

    void update();
    bool hasEnded();
    const Vec2 & getSize() const;
    sf::Sprite & getSprite();
    const std::vector<std::string> getAnimationNames();
    bool play(const std::string & anim_name);
    void pause();
    void setRepeat(bool repeat);
};