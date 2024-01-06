#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>

typedef std::multimap<std::string, std::unordered_map<std::string, std::string>>
    AssetData;

class Assets {
  std::unordered_map<std::string, sf::Texture> m_textures;
  std::unordered_map<std::string, sf::Font> m_fonts;

public:
  Assets() : m_textures{}, m_fonts{} {}
  ~Assets() {}

  bool addTexture(const std::string &name, const std::string &path) {
    sf::Texture tex;
    bool result = tex.loadFromFile(path);
    if (result) {
      m_textures[name] = tex;
    }
    return result;
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
    std::cout << "Trying to take texture " << name << std::endl;
    if (m_textures.find(name) == m_textures.end()) {
      std::cerr << "Texture " << name << " isn't in the assets data"
                << std::endl;
    }
    return m_textures[name];
  }

  const sf::Font &getFont(const std::string &name) {
    std::cout << "Trying to take font " << name << std::endl;
    if (m_fonts.find(name) == m_fonts.end()) {
      std::cerr << "Font " << name << " isn't in the assets data" << std::endl;
    }
    return m_fonts[name];
  }

  void load_assets(const std::string &filename) {
    AssetData data = read_file(filename);
    parse_assets(data);
    // test_data(data);
  }

  void test_assets() {
    std::cout << "====Assets test=====" << std::endl;
    for (const auto &[name, value] : m_textures) {
      std::cout << "Texture " << name << ", " << value.getSize().x << ":"
                << value.getSize().y << std::endl;
    }
    for (const auto &[name, value] : m_fonts) {
      std::cout << "Font " << name << std::endl;
    }
    std::cout << "====================" << std::endl;
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
  }

  void parse_fonts(AssetData &data) {
    for (auto &[header, values] : data) {
      if (header == "Font") {
        addFont(values["name"], values["path"]);
      }
    }
  }

  void parse_textures(AssetData &data) {
    for (auto &[header, values] : data) {
      if (header == "Texture") {
        addTexture(values["name"], values["path"]);
      }
    }
  }

  AssetData parse_tokens(const std::vector<std::string> &tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    AssetData data;
    std::string key, value;
    std::unordered_map<std::string, std::string> current_data;
    for (std::string token : tokenstream) {

      std::string::size_type begin = token.find_first_not_of(" ,[\"\f\t\v");
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
      key.erase(key.find_last_not_of(" ,]\"\f\t\v") + 1);

      // No blank keys allowed
      if (key.empty()) {
        continue;
      }

      // Extract the value (no leading or trailing whitespace allowed)
      begin = token.find_first_not_of(" ,]\"\f\n\r\t\v", end + 1);
      end = token.find_last_not_of(" ,]\"\f\n\r\t\v") + 1;

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
        [](std::pair<std::string, std::unordered_map<std::string, std::string>> p) {
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
