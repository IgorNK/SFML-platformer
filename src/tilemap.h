#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <vector>
#include "tileset.h"
#include "vec2.h"

class TileMap : public sf::Drawable, public sf::Transformable {
  sf::Texture m_texture;
  sf::VertexArray m_vertices;
  virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &m_texture;
    target.draw(m_vertices, states);
  }
public:
  bool load(const Tileset & tileset, const std::vector<int> & tiles, unsigned int width, unsigned int height) {
    if (!m_texture.loadFromFile(tileset.texturePath)) {
      return false;
    }

    m_vertices.setPrimitiveType(sf::Triangles);
    m_vertices.resize(width * height * 6);

    for (unsigned int i = 0; i < width; ++i) {
      for (unsigned int j = 0; j < height; ++j) {
        const int index = i + j * width;
        const int tileNumber = tiles[index] - 1;
        if (tileNumber < 0) {
          continue;
        }

        int tu = tileNumber % (tileset.width / tileset.tileWidth);
        int tv = tileNumber / (tileset.width / tileset.tileWidth);

        sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

        triangles[0].position = sf::Vector2f(i * tileset.tileWidth, j * tileset.tileHeight);
        triangles[1].position = sf::Vector2f((i + 1) * tileset.tileWidth, j * tileset.tileHeight);
        triangles[2].position = sf::Vector2f(i * tileset.tileWidth, (j + 1) * tileset.tileHeight);
        triangles[3].position = sf::Vector2f(i * tileset.tileWidth, (j + 1) * tileset.tileHeight);
        triangles[4].position = sf::Vector2f((i + 1) * tileset.tileWidth, j * tileset.tileHeight);
        triangles[5].position = sf::Vector2f((i + 1) * tileset.tileWidth, (j + 1) * tileset.tileHeight);

        triangles[0].texCoords = sf::Vector2f(tu * tileset.tileWidth, tv * tileset.tileHeight);
        triangles[1].texCoords = sf::Vector2f((tu  + 1) * tileset.tileWidth, tv * tileset.tileHeight);
        triangles[2].texCoords = sf::Vector2f(tu * tileset.tileWidth, (tv + 1)  * tileset.tileHeight);
        triangles[3].texCoords = sf::Vector2f(tu * tileset.tileWidth, (tv + 1)  * tileset.tileHeight);
        triangles[4].texCoords = sf::Vector2f((tu  + 1) * tileset.tileWidth, tv * tileset.tileHeight);
        triangles[5].texCoords = sf::Vector2f((tu  + 1) * tileset.tileWidth, (tv + 1) * tileset.tileHeight);
      }
    }
    
    return true;
  }
};