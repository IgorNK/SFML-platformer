#pragma once
#include <string>
#include <map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include "animated_sprite.h"

class Assets {
    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, AnimatedSprite> m_sprites;
    std::map<std::string, sf::Font> m_fonts;
public:
    Assets()
        : m_textures{}
        , m_sprites{}
        , m_fonts{}
    {}
    ~Assets() {}

    bool addTexture(const std::string & name, const std::string & path) {
        sf::Texture tex;
        bool result = tex.loadFromFile(path);
        if (result) {
            m_textures[name] = tex;
        }
        return result;
    }

    bool addSprite(const std::string & name, const AnimatedSprite & sprite) {
        const AnimatedSprite spr = sprite;
        m_sprites[name] = spr;
        return true;
    }

    bool addFont(const std::string & name, const std::string & path) {
        sf::Font font;
        bool result = font.loadFromFile(path);
        if (result) {
            m_fonts[name] = font;
        }
        return result;
    }
    
    const sf::Texture & getTexture(const std::string & name) {
        return m_textures[name];
    }

    const AnimatedSprite & getSprite(const std::string & name) {
        return m_sprites[name];
    }

    const sf::Font & getFont(const std::string & name) {
        return m_fonts[name];
    }
};