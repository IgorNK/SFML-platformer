#pragma once
#include "animated_sprite.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

typedef std::multimap<std::string, std::map<std::string, std::string>>
    AssetData;

class Assets {
  std::map<std::string, sf::Texture> m_textures;
  std::map<std::string, AnimatedSprite> m_sprites;
  std::map<std::string, sf::Font> m_fonts;

public:
  Assets() : m_textures{}, m_sprites{}, m_fonts{} {}
  ~Assets() {}

  bool addTexture(const std::string &name, const std::string &path) {
    sf::Texture tex;
    bool result = tex.loadFromFile(path);
    if (result) {
      m_textures[name] = tex;
    }
    return result;
  }

  bool addSprite(const std::string &name, const AnimatedSprite &sprite) {
    const AnimatedSprite spr = sprite;
    m_sprites[name] = spr;
    return true;
  }

  bool addFont(const std::string &name, const std::string &path) {
    sf::Font font;
    bool result = font.loadFromFile(path);
    if (result) {
      m_fonts[name] = font;
    }
    return result;
  }

  const sf::Texture &getTexture(const std::string &name) {
    return m_textures[name];
  }

  const AnimatedSprite &getSprite(const std::string &name) {
    return m_sprites[name];
  }

  const sf::Font &getFont(const std::string &name) { return m_fonts[name]; }

  void load_assets(const std::string &filename) {
    AssetData data = read_file(filename);
    test_data(data);
  }

private:
  const AssetData read_file(const std::string &filename) {
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

  void parse_assets(AssetData &data) {
    parse_fonts(data);
    parse_textures(data);
    parse_animations(data);
  }

  void parse_fonts(AssetData &data) {}

  void parse_textures(AssetData &data) {}

  void parse_animations(AssetData &data) {}

  AssetData parse_tokens(const std::vector<std::string> &tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    AssetData data;
    std::string key, value;
    std::map<std::string, std::string> current_data;
    for (std::string token : tokenstream) {

      std::string::size_type begin = token.find_first_not_of(" ,[\f\t\v");
      if (nextHeading) {
        if (heading != "") {
          data.insert(std::make_pair(heading, current_data));
        }
        nextHeading = false;
        heading = token;
        // std::cout << "Heading: " << heading << "\n";
        continue;
      }

      if (token.find(']') != std::string::npos) {
        nextHeading = true;
        // std::cout << "next heading\n";
      }

      // Skip blank lines
      if (begin == std::string::npos) {
        continue;
      }

      // Extract the key value
      std::string::size_type end = token.find('=', begin);
      key = token.substr(begin, end - begin);

      // (No leading or trailing whitespace allowed)
      key.erase(key.find_last_not_of(" ,]\f\t\v") + 1);

      // No blank keys allowed
      if (key.empty()) {
        continue;
      }

      // Extract the value (no leading or trailing whitespace allowed)
      begin = token.find_first_not_of(" ,]\f\n\r\t\v", end + 1);
      end = token.find_last_not_of(" ,]\f\n\r\t\v") + 1;

      value = token.substr(begin, end - begin);

      // std::cout << "key: " << key << ", value: " << value << '\n';
      current_data[key] = value;
    }
    if (heading != "") {
      data.insert(std::make_pair(heading, current_data));
    }
    return data;
  }

  void test_data(AssetData &data) {
    std::for_each(
        data.begin(), data.end(),
        [](std::pair<std::string, std::map<std::string, std::string>> p) {
          std::cout << "Heading: " << p.first << '\n';
          std::for_each(p.second.begin(), p.second.end(),
                        [](std::pair<std::string, std::string> v) {
                          std::cout << v.first << ": " << v.second << '\n';
                        });
        });
    // std::cout << "====Individual tests====\n";
    // std::cout << "Window resolution and refresh rate: "
    //           << data["Window"]["width"] << "x" << data["Window"]["height"]
    //           << ", " << data["Window"]["refreshRate"] << '\n';
  }
};
