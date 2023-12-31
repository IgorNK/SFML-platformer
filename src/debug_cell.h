#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include <iostream>
#include "vec2.h"

class DebugCell : public sf::Drawable, public sf::Transformable {
  sf::VertexArray m_vertices;
  Vec2 m_position;
  Vec2 m_size;
  const sf::Font & m_font;
  sf::Text m_text;
  sf::CircleShape m_shape;
  virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_vertices, states);
    target.draw(m_text, states);
  }
public:
  DebugCell(const Vec2 & size, const Vec2 & position, const sf::Font & font, const sf::Color color = sf::Color::Green)
    : m_position(position)
    , m_size(size)
    , m_font(font)
  { 
    m_text = sf::Text();
    m_text.setFont(m_font);
    m_text.setCharacterSize(10);
    m_text.setString(std::to_string((int)position.x) + ":" + std::to_string((int)position.y));
    m_text.setPosition({m_position.x * m_size.x, m_position.y * m_size.y});

    m_shape = sf::CircleShape(10, 5);
    m_shape.setPosition(m_position.x * m_size.x, m_position.y * m_size.y);

    m_vertices = sf::VertexArray(sf::LineStrip, 5);
    m_vertices[0].position = sf::Vector2f(m_position.x * m_size.x, m_position.y * m_size.y);
    m_vertices[1].position = sf::Vector2f((m_position.x + 1) * m_size.x, m_position.y * m_size.y);
    m_vertices[2].position = sf::Vector2f((m_position.x + 1) * m_size.x, (m_position.y + 1) * m_size.y);
    m_vertices[3].position = sf::Vector2f(m_position.x * m_size.x, (m_position.y + 1) * m_size.y);
    m_vertices[4].position = sf::Vector2f(m_position.x * m_size.x, m_position.y * m_size.y);

    m_vertices[0].color = color;
    m_vertices[1].color = color;
    m_vertices[2].color = color;
    m_vertices[3].color = color;
    m_vertices[4].color = color;
  }

  const Vec2 & getPosition() const {
    return m_position;
  }

  const Vec2 & getSize() const {
    return m_size;
  }
};