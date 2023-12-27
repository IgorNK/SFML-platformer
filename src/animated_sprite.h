#include <SFML/Graphics.hpp>
#include "vec2.h"

class AnimatedSprite {
    sf::Sprite m_sprite;
    size_t m_frameCount;
    size_t m_currentFrame;
    size_t m_speed;
    Vec2 m_size;
    std::vector<sf::IntRect> m_frames;
    std::string m_name;
public:
    AnimatedSprite(const std::string & name = "Default", const sf::Texture & t = sf::Texture(), std::vector<sf::IntRect> frames = {}, Vec2 size = {0.f, 0.f}, size_t speed = 1)
        : m_sprite(t)
        , m_frameCount(frames.size())
        , m_currentFrame(0)
        , m_speed(speed)
        , m_size(size)
        , m_frames(frames)
        , m_name(name)
    { 
        if (m_frames.size() > 0) {
            m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
            m_sprite.setTextureRect(m_frames[0]);
        }
    }
    ~AnimatedSprite() {}

    void update();
    bool hasEnded() const;
    const std::string & getName() const;
    const Vec2 & getSize() const;
    sf::Sprite & getSprite();
};