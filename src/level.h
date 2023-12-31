#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "animated_sprite.h"
#include "tileset.h"

class Level {
    Vec2 m_dimensions;
    Tileset m_tileset;
    std::vector<int> m_tiles;

  void parse_tokens(const std::vector<std::string> &tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    std::string key, value;
    for (std::string token : tokenstream) {

      std::string::size_type begin = token.find_first_not_of(" ,[\f\t\v");
      if (nextHeading) {
        nextHeading = false;
        heading = token;
        continue;
      }

      if (token.find(']') != std::string::npos) {
        nextHeading = true;
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
            m_tiles.push_back(int_val);
            value.erase(0, pos + 1);
        }
        if (!value.empty()) {
            const int int_val = std::stoi(value.substr(0, pos));
            m_tiles.push_back(int_val);
        }
      }
      if (heading == "Level" && key == "size") {
        std::string::size_type delim = value.find(',', 0);
        const int x_val = std::stoi(value.substr(0, delim));
        const int y_val = std::stoi(value.substr(delim + 1, value.size()));
        m_dimensions.x = x_val;
        m_dimensions.y = y_val;
      }
      if (heading == "Tileset") {
        if (key == "name") {
          m_tileset.name = value;
        } else if (key == "texturePath") {
          m_tileset.texturePath = value;
        } else if (key == "tileWidth") {
          m_tileset.tileWidth = std::stoi(value);
        } else if (key == "tileHeight") {
          m_tileset.tileHeight = std::stoi(value);
        } else if (key == "width") {
          m_tileset.width = std::stoi(value);
        } else if (key == "height") {
          m_tileset.height = std::stoi(value);
        }
      }
    }
  }

  int read_file(const std::string &filename) {
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

    try {
      parse_tokens(tokenstream);
    } catch (const std::invalid_argument &ia) {
      std::cerr << "Invalid argument: " << ia.what() << std::endl;
      return -1;
    }

    catch (const std::out_of_range &oor) {
      std::cerr << "Out of Range error: " << oor.what() << std::endl;
      return -2;
    }

    catch (const std::exception &e) {
      std::cerr << "Undefined error: " << e.what() << std::endl;
      return -3;
    }
    return 0;
  }


public:
    bool load_file(const std::string & filename) {
        if (read_file(filename) != 0) {
          std::cerr << "Failed to read level file " << filename << std::endl;
          return false;
        }
        return true;
    }

    const Vec2 & getDimensions() const {
      return m_dimensions;
    }

    const Tileset & getTileset() const {
      return m_tileset;
    }

    const std::vector<int> & getTiles() const {
      return m_tiles;
    }
};