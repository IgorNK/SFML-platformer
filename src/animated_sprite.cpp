#include "animated_sprite.h"

void AnimatedSprite::update() {
    
}

bool AnimatedSprite::hasEnded() const {
    return m_currentFrame >= m_frameCount - 1;
}

const std::string & AnimatedSprite::getName() const {
    return m_name;
}

const Vec2 & AnimatedSprite::getSize() const {
    return m_size;
}

sf::Sprite & AnimatedSprite::getSprite() {
    return m_sprite;
}