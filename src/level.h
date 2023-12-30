#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include "static_collider.h"
#include "animated_sprite.h"

class Level {
    sf::Texture m_texture;
    Vec2 m_size_tiles;
    Vec2 m_size_pixels;
    Vec2 m_tile_size;
    std::vector<sf::Sprite> m_tiles;
    std::vector<int> m_tile_IDs;
    std::vector<StaticCollider> m_colliders;

  void parse_tokens(const std::vector<std::string> &tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    std::string key, value;
    for (std::string token : tokenstream) {

      std::string::size_type begin = token.find_first_not_of(" ,[\f\t\v");
      if (nextHeading) {
        nextHeading = false;
        heading = token;
        std::cout << "Heading: " << heading << "\n";
        continue;
      }

      if (token.find(']') != std::string::npos) {
        nextHeading = true;
        std::cout << "next heading\n";
        continue;
      }

      // Skip blank lines
      if (begin == std::string::npos) {
        continue;
      }

      // Extract the key value
      std::string::size_type end = token.find('=', begin);
      if (end != std::string::npos) {
        key = token.substr(begin, end - begin);
        // (No leading or trailing whitespace allowed)
        key.erase(key.find_last_not_of(" ,]\f\t\v") + 1);
      }

      // Extract the value (no leading or trailing whitespace allowed)
      begin = token.find_first_not_of(" ,]\f\n\r\t\v", end + 1);
      end = token.find_last_not_of(" ,]\f\n\r\t\v") + 1;
      value = token.substr(begin, end - begin);
      value.erase(value.find_last_not_of(" ,]\f\t\v") + 1);
      
      // No blank keys allowed
      if (heading == "Level" && key == "tiles") {
        size_t pos= 0;
        while ((pos = value.find(',')) != std::string::npos) {
            const int int_val = std::stoi(value.substr(0, pos));
            std::cout << int_val << ',';
            m_tile_IDs.push_back(int_val);
            value.erase(0, pos + 1);
        }
        if (!value.empty()) {
            const int int_val = std::stoi(value.substr(0, pos));
            std::cout << int_val << ',' << std::endl;
            m_tile_IDs.push_back(int_val);
        }
      }
      if (heading == "Level" && key == "size") {
        std::string::size_type delim = value.find(',', 0);
        const int x_val = std::stoi(value.substr(0, delim));
        const int y_val = std::stoi(value.substr(delim + 1, value.size()));
        std::cout << "size: " << x_val << ":" << y_val << '\n';
      }
      if (heading == "Tileset") {
        if (key == "tileWidth") {
          m_tile_size.x = std::stoi(value);
        }
        if (key == "tileHeight") {
          m_tile_size.y = std::stoi(value);
        }
      }
    }

    for (int i = 0; i < m_tile_IDs.size(); ++i) {
        std::cout << m_tile_IDs[i] << std::endl;
    }
  }

  void read_file(const std::string &filename) {
    // May throw, should be enclosed in try / catch expression
    std::vector<std::string> tokenstream{};
    std::string word;

    std::ifstream ifs(filename, std::ifstream::in);
    if (!ifs) {
      throw std::runtime_error("Could not read file");
    }
    while (ifs >> word) {
      tokenstream.push_back(word);
    }

    return parse_tokens(tokenstream);
  }


public:
    void load_file(const std::string & filename) {
        read_file(filename);
    }
};