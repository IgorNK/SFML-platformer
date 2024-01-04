#pragma once
#include "SFML/Graphics/Rect.hpp"
#include "animated_sprite.h"
#include "tileset.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <optional>
#include <vector>

enum SNAP_POSITION {
  SNAP_CENTER,
  SNAP_TOP_LEFT,
  SNAP_TOP_RIGHT,
  SNAP_BOTTOM_RIGHT,
  SNAP_BOTTOM_LEFT
};

struct EntityData {
  int id;
  std::string name;
  std::string type;
  int x;
  int y;
};

class Level {
  Vec2 m_dimensions; // size in cells
  Tileset m_tileset;
  std::vector<int> m_tiles;
  std::vector<EntityData> m_entities;

  void parse_tokens(const std::vector<std::string> &tokenstream) {
    bool nextHeading = true;
    std::string heading = "";
    std::string key, value;
    EntityData new_entity;
    bool first_entity = true;
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
        key.erase(key.find_last_not_of(" ,]\"\f\t\v") + 1);
      }

      // Extract the value (no leading or trailing whitespace allowed)
      begin = token.find_first_not_of(" ,]\"\f\n\r\t\v", end + 1);
      end = token.find_last_not_of(" ,]\"\f\n\r\t\v") + 1;
      value = token.substr(begin, end - begin);
      value.erase(value.find_last_not_of(" ,]\"\f\t\v") + 1);

      // No blank keys allowed
      if (heading == "Level" && key == "tiles") {
        size_t pos = 0;
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
      if (heading == "Entities") {
        if (key == "id") {
          // std::cout << "key: " << key << ", value: " << value << std::endl;
          int id = std::stoi(value);
          if (auto e =
                  std::find_if(m_entities.begin(), m_entities.end(),
                               [&](EntityData ent) { return ent.id == id; });
              e == m_entities.end()) {
            // this is a new id
            if (first_entity) {
              // std::cout << "This entity is first. Skip pushing to array." <<
              // std::endl;
              first_entity = false;
            } else {
              // std::cout << "Id " << id << " is a new id. Pushing created
              // entity into array." << std::endl;
              m_entities.push_back(new_entity);
            }
            new_entity = {};
            new_entity.id = id;
            continue;
          } else {
            // std::cout << "This id already exists, for some reason." <<
            // std::endl;
            continue;
          }
        } else if (key == "name") {
          // std::cout << "Name: " << value << std::endl;
          new_entity.name = value;
        } else if (key == "type") {
          // std::cout << "Type: " << value << std::endl;
          new_entity.type = value;
        } else if (key == "x") {
          // std::cout << "x: " << value << std::endl;
          new_entity.x = std::stoi(value);
        } else if (key == "y") {
          // std::cout << "y: " << value << std::endl;
          new_entity.y = std::stoi(value);
        }
      }
    }
    // std::cout << "Pushing in last entity." << std::endl;
    m_entities.push_back(new_entity);
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
  bool load_file(const std::string &filename) {
    if (read_file(filename) != 0) {
      std::cerr << "Failed to read level file " << filename << std::endl;
      return false;
    }
    return true;
  }

  void test_data() const {
    std::cout << "Testing entities in m_entities:" << std::endl;
    for (const EntityData &entity : m_entities) {
      std::cout << "id: " << entity.id << std::endl;
      std::cout << "name: " << entity.name << std::endl;
      std::cout << "type: " << entity.type << std::endl;
      std::cout << "x: " << entity.x << std::endl;
      std::cout << "y: " << entity.y << std::endl;
    }
  }

  const Vec2 snap_to_grid(const Vec2 &pos,
                          const SNAP_POSITION snap = SNAP_BOTTOM_LEFT) const {
    Vec2 grid_coords = pos_to_grid(pos);
    return grid_to_pos(grid_coords, snap);
  }

  const Vec2 pos_to_grid(const Vec2 &pos) const {
    return Vec2({
        std::floor((pos.x + 1) /
                   m_tileset.tileWidth), // +1 for objects snapped to grid
        std::floor((pos.y - 1) /
                   m_tileset.tileHeight) /// -1 for objects snapped to grid
    });
  }

  const Vec2 grid_to_pos(const Vec2 &coords,
                         const SNAP_POSITION snap = SNAP_BOTTOM_LEFT) const {
    Vec2 pos = {
        coords.x * m_tileset.tileWidth,
        coords.y * m_tileset.tileHeight,
    };
    switch (snap) {
    case SNAP_TOP_LEFT: {
      break;
    }
    case SNAP_TOP_RIGHT: {
      pos.x += m_tileset.tileWidth;
      break;
    }
    case SNAP_BOTTOM_RIGHT: {
      pos.x += m_tileset.tileWidth;
      pos.y += m_tileset.tileHeight;
      break;
    }
    case SNAP_BOTTOM_LEFT: {
      pos.y += m_tileset.tileHeight;
      break;
    }
    case SNAP_CENTER: {
      pos.x += m_tileset.tileWidth / 2.f;
      pos.y += m_tileset.tileHeight / 2.f;
      break;
    }
    }
    return pos;
  }

  const std::vector<int> getNonZeroTiles() const {
    std::vector<int> tiles;
    for (int i = 0; i < m_tiles.size(); ++i) {
      if (m_tiles[i] != 0) {
        tiles.push_back(i);
      }
    }
    return tiles;
  }

  const int getTileAtCoords(const int x, const int y) const {
    return m_tiles[getIndexAtCoords(x, y)];
  }

  const sf::IntRect getBoundsAtIndex(const int index) const {
    const Vec2 coords = getCoordsAtIndex(index);
    const Vec2 pos = grid_to_pos(coords, SNAP_TOP_LEFT);
    return sf::IntRect{(int)pos.x, (int)pos.y, (int)m_tileset.tileWidth,
                       (int)m_tileset.tileHeight};
  }

  const Vec2 getCoordsAtIndex(const int index) const {
    if (index > m_tiles.size() - 1) {
      throw std::runtime_error("getCoordsAtIndex: tile index too high: " +
                               std::to_string(index));
    }
    int y = std::floor(((float)index) / m_dimensions.x);
    int x = (index) % (int)m_dimensions.x;
    // std::cout << "index: " << index << ": " << x << "," << y << std::endl;
    if (x >= m_dimensions.x || y >= m_dimensions.y) {
      throw std::runtime_error("Calculated position at index " +
                               std::to_string(index) + " is out of bounds: [" +
                               std::to_string(x) + ":" + std::to_string(y) +
                               "]");
    }
    return Vec2(x, y);
  }

  const int getIndexAtCoords(const int x, const int y) const {
    const int index = y * m_dimensions.x + x;
    if (index > m_tiles.size() - 1) {
      throw std::runtime_error("getIndexCoords: index at requested coords [" + std::to_string(x) + ":" + std::to_string(y) + " is too high: " + std::to_string(index));
    }
    return index;
  }

  std::vector<int> getNeighbours(const int index,
                                 bool include_diagonals = false) const {
    if (index > m_tiles.size() - 1) {
      throw std::runtime_error("getNeighbours pre-condition: index too high: " + std::to_string(index) + " / " + std::to_string(m_tiles.size() - 1));
    }
    std::vector<int> neighbours;
    // top
    if (index - m_dimensions.x > 0) {
      int n_i = index - m_dimensions.x;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution top: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // top right
    if (include_diagonals 
      && (index - m_dimensions.x > 0) 
      && (index % (int)m_dimensions.x < m_dimensions.x - 1) 
      && (index < m_tiles.size() - 1)) {
      int n_i = index - m_dimensions.x + 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution top right: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // right
    if ((index % (int)m_dimensions.x < m_dimensions.x - 1) 
      && (index < m_tiles.size() - 1)) {
      int n_i = index + 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution right: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // bottom right
    if (include_diagonals 
      && (index % (int)m_dimensions.x < m_dimensions.x - 1) 
      && (index < m_tiles.size() - 1) 
      && (index + m_dimensions.x < m_tiles.size())) {
      int n_i = index + m_dimensions.x + 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution bottom right: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // bottom
    if (index + m_dimensions.x < m_tiles.size()) {
      int n_i = index + m_dimensions.x;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution bottom: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // bottom left
    if (include_diagonals 
      && (index + m_dimensions.x < m_tiles.size()) 
      && (index % (int)m_dimensions.x > 1 && index > 0)) {
      int n_i = index + m_dimensions.x - 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution bottom left: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // left
    if (index % (int)m_dimensions.x > 1 && index > 0) {
      int n_i = index - 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution left: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    // top left
    if (include_diagonals 
      && (index % (int)m_dimensions.x > 1) 
      && (index > 0) 
      && (index - m_dimensions.x > 0)) {
      int n_i = index - m_dimensions.x - 1;
      if (n_i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours execution top left: index too high: " + std::to_string(n_i) + " / " + std::to_string(m_tiles.size() - 1));
      }
      neighbours.push_back(n_i);
    }
    for (const int i : neighbours) {
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("getNeighbours result: tile index too high: " +
                                 std::to_string(i));
      }
    }
    return neighbours;
  }

  bool contains(const std::vector<int> &map, const int index) const {
    return (std::find(map.begin(), map.end(), index) != map.end());
  }

  bool contains(const std::vector<int> &map, const int x, const int y) const {
    int index = getIndexAtCoords(x, y);
    return contains(map, index);
  }

  bool contains(const std::vector<int> &map,
                const std::vector<int> &indices) const {
    for (const int index : indices) {
      if (!contains(map, index)) {
        return false;
      }
    }
    return true;
  }

  sf::IntRect findShapeBounds(const std::vector<int> &indices) const {
    std::vector<Vec2> coords;
    // std::cout << "Finding bounds of cells: \n";
    for (int i : indices) {
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("findShapeBounds: tile index too high: " +
                                 std::to_string(i));
      }
      Vec2 c = getCoordsAtIndex(i);
      // std::cout << i << " [" << c.x << ":" << c.y << "], ";
      coords.push_back(c);
    }
    // std::cout << std::endl;
    int min_x = INT_MAX;
    int min_y = INT_MAX;
    int max_x = 0;
    int max_y = 0;
    for (Vec2 c : coords) {
      if (c.x > max_x) {
        max_x = c.x;
      }
      if (c.x < min_x) {
        min_x = c.x;
      }
      if (c.y > max_y) {
        max_y = c.y;
      }
      if (c.y < min_y) {
        min_y = c.y;
      }
    }
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;
    // std::cout << "Bounds of ";
    // for (auto i : indices) {
    //   const Vec2 c = getCoordsAtIndex(i);
    //   std::cout << c.x << ":" << c.y << "; ";
    // }
    // std::cout << std::endl
    //           << min_x << ":" << min_y << "," << width << ":" << height
    //           << std::endl;
    return sf::IntRect(min_x, min_y, width, height);
  }

  enum SLICE_DIRECTION { SLICE_UP, SLICE_RIGHT, SLICE_DOWN, SLICE_LEFT };

  std::vector<int> slice(const std::vector<int> &indices, int index,
                         SLICE_DIRECTION dir) const {
    std::vector<Vec2> coords;
    // std::cout << "Coords before slice:\n";
    for (int i : indices) {
      const Vec2 c = getCoordsAtIndex(i);
      coords.push_back(c);
      // std::cout << i << " [" << c.x << ":" << c.y << "], ";
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("slice pre-condition: Tile index too high: " +
                                 std::to_string(i));
      }
    }
    // std::cout << std::endl;
    // std::cout << "Indices size: " << indices.size()
              // << ", vectors: " << coords.size() << std::endl;
    if (indices.size() != coords.size()) {
      throw std::runtime_error(
          "slice coords conversion: wrong number of items! " +
          std::to_string(indices.size()) +
          " != " + std::to_string(coords.size()));
    }
    Vec2 hole_pos = getCoordsAtIndex(index);
    std::vector<Vec2> sliced{};
    switch (dir) {
    case SLICE_UP: {
      std::copy_if(coords.begin(), coords.end(), std::back_inserter(sliced),
                   [&](Vec2 &vec) {
                     if (vec.y < hole_pos.y) {
                       const int index =
                           getIndexAtCoords((int)vec.x, (int)vec.y);
                       if (index > m_tiles.size() - 1) {
                         throw std::runtime_error(
                             "slice up execution: copied index is too high: " +
                             std::to_string(index) + " / " +
                             std::to_string(m_tiles.size() - 1));
                       }
                       return true;
                     }
                     return false;
                   });
      break;
    }
    case SLICE_RIGHT: {
      std::copy_if(coords.begin(), coords.end(), std::back_inserter(sliced),
                   [&](Vec2 &vec) {
                     if (vec.x > hole_pos.x) {
                       const int index =
                           getIndexAtCoords((int)vec.x, (int)vec.y);
                       if (index > m_tiles.size() - 1) {
                         throw std::runtime_error(
                             "slice right execution: copied index at coords " +
                             std::to_string((int)vec.x) + ":" +
                             std::to_string((int)vec.y) +
                             " is too high: " + std::to_string(index) + " / " +
                             std::to_string(m_tiles.size() - 1));
                       }
                       return true;
                     }
                     return false;
                   });
      break;
    }
    case SLICE_DOWN: {
      std::copy_if(
          coords.begin(), coords.end(), std::back_inserter(sliced),
          [&](Vec2 &vec) {
            if (vec.y > hole_pos.y) {
              const int index = getIndexAtCoords((int)vec.x, (int)vec.y);
              if (index > m_tiles.size() - 1) {
                throw std::runtime_error(
                    "slice down execution: copied index is too high: " +
                    std::to_string(index) + " / " +
                    std::to_string(m_tiles.size() - 1));
              }
              return true;
            }
            return false;
          });
      break;
    }
    case SLICE_LEFT: {
      std::copy_if(
          coords.begin(), coords.end(), std::back_inserter(sliced),
          [&](Vec2 &vec) {
            if (vec.x < hole_pos.x) {
              const int index = getIndexAtCoords((int)vec.x, (int)vec.y);
              if (index > m_tiles.size() - 1) {
                throw std::runtime_error(
                    "slice left execution: copied index is too high: " +
                    std::to_string(index) + " / " +
                    std::to_string(m_tiles.size() - 1));
              }
              return true;
            }
            return false;
          });
      break;
    }
    }
    std::vector<int> sliced_idx;
    // std::cout << "coords after slice: \n";
    for (Vec2 &v : sliced) {
      const int index = getIndexAtCoords((int)v.x, (int)v.y);
      // std::cout << index << " [" << v.x << ":" << v.y << "]; ";
      if (index > m_tiles.size() - 1) {
        throw std::runtime_error(
            "slice result: tile index too high: " + std::to_string(index) +
            "/ " + std::to_string(m_tiles.size() - 1));
      }
      sliced_idx.push_back(index);
    }
    // std::cout << std::endl;
    // const sf::IntRect bounds = findShapeBounds(indices);
    // std::cout << "Slice bounds " << bounds.left << ":" << bounds.top << ";"
    //           << bounds.width << ":" << bounds.height << " from " << pos.x
    //           << ":" << pos.y << "in direction " << dir
    //           << ". Size before slice: " << indices.size()
    //           << ", after: " << sliced_idx.size() << std::endl;

    return sliced_idx;
  }

  int findFirstCell(const std::vector<int> &indices, bool is_hole) const {
    const sf::IntRect bounds = findShapeBounds(indices);
    // std::cout << "Supplied indices to find hole in: " << std::endl;
    for (const int i : indices) {
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("findFirstHole pre-condition: Tile index too high: " +
                                 std::to_string(i));
      }
      // std::cout << i << ", ";
    }
    // std::cout << std::endl;
    for (int i = bounds.left; i < bounds.left + bounds.width; ++i) {
      for (int j = bounds.top; j < bounds.top + bounds.height; ++j) {
        int index = getIndexAtCoords(i, j);
        if (!contains(indices, index) == is_hole) {
          // std::cout << "Hole at " << index << std::endl;
          if (index > m_tiles.size() - 1) {
            throw std::runtime_error(
                "findFirstHole result: Tile index too high: " +
                std::to_string(index));
          }
          return index;
        }
      }
    }
    return -1;
  }

  int findLastCellInRow(const std::vector<int> &indices, bool is_hole, unsigned int row = 0) const {
    const sf::IntRect bounds = findShapeBounds(indices);
    std::cout << "find last cell in row supplied indices: ";
    for (const int i : indices) {
      std::cout << i << ", ";
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("findLastCellInRow pre-condition: Tile index too high: " +
                                 std::to_string(i));
      }
    }
    std::cout << std::endl;
    if (indices.size() == 0) {
      throw std::runtime_error("findLastCellInRow pre-condition: empty indices");
    }
    int cell = indices[0];
    for (int i = bounds.left; i < bounds.left + bounds.width; ++i) {
      int j = bounds.top + row;
      int index = getIndexAtCoords(i, j);
      if (!contains(indices, index) == is_hole) {
        if (index > m_tiles.size() - 1) {
          throw std::runtime_error(
              "findLastCellInRow result: Tile index too high: " +
              std::to_string(index));
        }
        cell = index;
        std::cout << "cell " << index << " [" << i << ":" << j << "] is in indices.\n";
      } else {
        break;
      }
    }
    return cell;
  }

  int findLastCellInColumn(const std::vector<int> &indices, bool is_hole, unsigned int column = 0) const {
    const sf::IntRect bounds = findShapeBounds(indices);
    for (const int i : indices) {
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error("findLastCellInColumn pre-condition: Tile index too high: " +
                                 std::to_string(i));
      }
    }
    if (indices.size() == 0) {
      throw std::runtime_error("findLastCellInRow pre-condition: empty indices");
    }
    int cell = indices[0];
    for (int j = bounds.top; j < bounds.top + bounds.height; ++j) {
      int i = bounds.left + column;
      int index = getIndexAtCoords(i, j);
      if (!contains(indices, index) == is_hole) {
        if (index > m_tiles.size() - 1) {
          throw std::runtime_error(
              "findLastCellInColumn result: Tile index too high: " +
              std::to_string(index));
        }
        cell = index;
      } else {
        break;
      }
    }
    return cell;
  }

  void subtract(std::vector<int> &minuend,
                const std::vector<int> &subtrahend) const {
    for (const int i : subtrahend) {
      minuend.erase(std::remove(minuend.begin(), minuend.end(), i),
                    minuend.end());
    }
  }

  std::vector<std::vector<int>>
  getRectIslands(const std::vector<int> &in_indices) const {
    static int iter = 0;
    std::vector<int> indices = in_indices;
    // Debug check max index
    int max = 0;
    for (const int i : in_indices) {
      if (i > m_tiles.size() - 1) {
        throw std::runtime_error(
            "getRectIslands pre-condition: tile index too high: " +
            std::to_string(i));
      }
      if (i > max) {
        max = i;
      }
    }
    Vec2 coords = getCoordsAtIndex(max);
    // std::cout << "Max index in supplied indices: " << max << ", " << coords.x
    //           << ":" << coords.y << std::endl;
    // End of debug check
    std::vector<std::vector<int>> joined;
    while (indices.size() > 0) {
      std::vector<int> section = indices;
      std::cout << "Let's find a hole in: " << std::endl;
      for (const int i : section) {
        if (i > m_tiles.size() - 1) {
          throw std::runtime_error("getRectIslands: tile index too high: " +
                                   std::to_string(i));
        }
        std::cout << i << ", ";
      }
      std::cout << std::endl;
      int hole = findFirstCell(section, true);
      // std::cout << "section size before hole loop: " << section.size()
      //           << std::endl;
      while (hole > 0) {
        iter++;
        // Split the shape based on hole's location
        const sf::IntRect bounds = findShapeBounds(section);
        const Vec2 hole_coords = getCoordsAtIndex(hole);
        // std::cout << "Section size: " << section.size() << ". Hole at "
        //           << hole_coords.x << ":" << hole_coords.y << std::endl;
        std::vector<int> sub_section;
        int center_x = std::floor(bounds.left + bounds.width / 2);
        int center_y = std::floor(bounds.top + bounds.height / 2);
        int dx = hole_coords.x - center_x;
        int dy = hole_coords.y - center_y;
        // If the hole is at the edge
        // dx < 0 - hole is on the left side, dx > 0 - on the right side
        // dy < 0 - hole is on the upper side, dy > 0 - hole is on the lower
        // side
        const sf::IntRect sectionBounds = findShapeBounds(section);
        if (sectionBounds.width > 1 || sectionBounds.height > 1) {
          if (dx < 0 && dy < 0) {
            // Left upper side
            if (sectionBounds.width > 1 && std::abs(dx) >= std::abs(dy)) {
              std::cout << "Hole is in the LEFT UPPER side, slicing to the RIGHT\n";
              // int last_hole = findLastCellInRow(section, true, hole_coords.y - sectionBounds.top);
              int last_hole = findLastCellInRow(section, true);
              Vec2 last_hole_coords = getCoordsAtIndex(last_hole);
              std::cout << "Hole is " << hole_coords.x << ":" << hole_coords.y << ". Last one is " << last_hole_coords.x << ":" << last_hole_coords.y << std::endl;
              sub_section = slice(section, last_hole, SLICE_RIGHT);
            } else if (sectionBounds.height > 1) {
              std::cout << "Hole is in the LEFT UPPER side, slicing to the BOTTOM\n";
              // int last_hole = findLastCellInColumn(section, true, hole_coords.x - sectionBounds.left);
              int last_hole = findLastCellInColumn(section, true);
              Vec2 last_hole_coords = getCoordsAtIndex(last_hole);
              std::cout << "Hole is " << hole_coords.x << ":" << hole_coords.y << ". Last one is " << last_hole_coords.x << ":" << last_hole_coords.y << std::endl;
              sub_section = slice(section, last_hole, SLICE_DOWN);
            }
          } else if (dx >= 0 && dy < 0) {
            // Right upper side
            if (sectionBounds.width > 1 && std::abs(dx) >= std::abs(dy)) {
              std::cout << "Hole is in the RIGHT UPPER side, slicing to the LEFT\n";
              sub_section = slice(section, hole, SLICE_LEFT);
            } else if (sectionBounds.height > 1) {
              std::cout << "Hole is in the RIGHT UPPER side, slicing to the BOTTOM\n";
              // int last_hole = findLastCellInColumn(section, true, hole_coords.x - sectionBounds.left);
              int last_hole = findLastCellInColumn(section, true);
              Vec2 last_hole_coords = getCoordsAtIndex(last_hole);
              std::cout << "Hole is " << hole_coords.x << ":" << hole_coords.y << ". Last one is " << last_hole_coords.x << ":" << last_hole_coords.y << std::endl;
              sub_section = slice(section, last_hole, SLICE_DOWN);
            }
          } else if (dx >= 0 && dy >= 0) {
            // Right lower side
            if (sectionBounds.width > 1 && std::abs(dx) >= std::abs(dy)) {
              std::cout << "Hole is in the RIGHT LOWER side, slicing to the LEFT\n";
              sub_section = slice(section, hole, SLICE_LEFT);
            } else if (sectionBounds.height > 1) {
              std::cout << "Hole is in the RIGHT LOWER side, slicing to the TOP\n";
              sub_section = slice(section, hole, SLICE_UP);
            }
          } else if (dx < 0 && dy >= 0) {
            // Left lower side
            if (sectionBounds.width > 1 && std::abs(dx) >= std::abs(dy)) {
              std::cout << "Hole is in the LEFT LOWER side, slicing to the RIGHT\n";
              // int last_hole = findLastCellInRow(section, true, hole_coords.y - sectionBounds.top);
              int last_hole = findLastCellInRow(section, true);
              Vec2 last_hole_coords = getCoordsAtIndex(last_hole);
              std::cout << "Hole is " << hole_coords.x << ":" << hole_coords.y << ". Last one is " << last_hole_coords.x << ":" << last_hole_coords.y << std::endl;
              sub_section = slice(section, last_hole, SLICE_RIGHT);
            } else if (sectionBounds.height > 1) {
              std::cout << "Hole is in the LEFT LOWER side, slicing to the TOP\n";
              sub_section = slice(section, hole, SLICE_UP);
            }
          } else {
            std::cerr << "Unexpected behaviour, dx:" << dx << ", dy:" << dy
                      << ", section width:" << sectionBounds.width
                      << ", height:" << sectionBounds.height << std::endl;
          }
        } else {
          std::cout << "Section is 1 tile in size. Hole is: " << hole_coords.x
                    << ":" << hole_coords.y;
          std::cout << "; Section contains: ";
          for (const auto i : section) {
            std::cout << i << ",";
          }
          std::cout << std::endl;
        }
        section.clear();
        section = sub_section;
        for (int i : section) {
          if (i > m_tiles.size() - 1) {
            throw std::runtime_error(
                "getRectIslands mid-execution: index in section is to high: " +
                std::to_string(i));
          }
        }
        hole = findFirstCell(section, true);
      }
      // Win condition: found a section with no holes
      // std::cout << "Found section with no holes: \n";
      // for (auto &e : section) {
      //   Vec2 coords = getCoordsAtIndex(e);
      //   std::cout << coords.x << ":" << coords.y << "; ";
      // }
      // std::cout << std::endl;
      subtract(indices, section);
      joined.push_back(section);
    }
    std::cout << "iterations: " << iter << std::endl;
    return joined;
  }

  std::vector<sf::IntRect>
  getJoinedBounds(const std::vector<std::vector<int>> &islands) const {
    std::vector<sf::IntRect> joined_bounds;
    for (const std::vector<int> &island : islands) {
      sf::IntRect bounds = findShapeBounds(island);
      joined_bounds.push_back(bounds);
      // std::cout << "Found shape: ";
      for (const auto i : island) {
        if (i > m_tiles.size() - 1) {
          throw std::runtime_error("getJoinedBounds: tile index too high: " +
                                   std::to_string(i));
        }
        // std::cout << i << ", ";
      }
      // std::cout << "Bounds: " << bounds.left << ":" << bounds.top << ","
      //           << bounds.width << ":" << bounds.height << std::endl;
    }
    return joined_bounds;
  }

  std::vector<int> getEdges() const {
    std::vector<int> edges;
    for (int i = 0; i < m_tiles.size(); ++i) {
      if (m_tiles[i] == 0) {
        continue;
      }
      const std::vector<int> neighbours = getNeighbours(i, false);
      bool edge = false;
      for (int index : neighbours) {
        if (index > m_tiles.size() - 1) {
          throw std::runtime_error("getEdges: tile index too high: " +
                                   std::to_string(i));
        }
        // std::cout << "Tile " << index;
        if (m_tiles[index] == 0) {
          // std::cout << " is the edge.\n";
          edge = true;
          break;
        } else {
          // std::cout << " is not the edge.\n";
        }
      }
      if (edge) {
        edges.push_back(i);
      }
    }
    // std::cout << "Found " << edges.size() << "edges: \n";
    // for (const int i : edges) {
    //   std::cout << i << ", ";
    // }
    return edges;
  }

  std::vector<sf::IntRect> getEdgeBounds() const {
    std::vector<sf::IntRect> edge_bounds;
    const std::vector<int> edges = getEdges();
    // const std::vector<int> edges = getNonZeroTiles();
    const std::vector<std::vector<int>> edgeIslands = getRectIslands(edges);
    for (const std::vector<int> &island : edgeIslands) {
      const sf::IntRect islandDims = findShapeBounds(island);
      // std::cout << "Found island " << islandDims.left << ":" << islandDims.top
      //           << "," << islandDims.width << ":" << islandDims.height
      //           << std::endl;
      const sf::IntRect bounds = {
          (int)(islandDims.left * m_tileset.tileWidth),
          (int)(islandDims.top * m_tileset.tileHeight),
          (int)(islandDims.width * m_tileset.tileWidth),
          (int)(islandDims.height * m_tileset.tileHeight)};
      // std::cout << "Created shape " << bounds.left << ":" << bounds.top << ","
      //           << bounds.width << ":" << bounds.height << std::endl;
      edge_bounds.push_back(bounds);
    }
    return edge_bounds;
  }

  bool is_collider(const int x, const int y) const {
    return m_tiles[getIndexAtCoords(x, y)] != 0;
  }

  struct Rect {
    int start_x;
    int start_y;
    int end_x;
    int end_y;
  };

  void sortVertically(std::vector<Rect> & rects) const {
    std::sort(rects.begin(), rects.end(), 
      [](const Rect & a, const Rect & b) { 
        return a.start_y < b.start_y;
       });
  }

// Tile-merging algorithm from https://love2d.org/wiki/TileMerging, merges everything in columns
  std::vector<sf::IntRect> getCombinedColliders(const std::vector<int> & indices) const {
    std::vector<Rect> rectangles = {};
    sf::IntRect bounds = findShapeBounds(indices);

    for (int x = bounds.left; x < bounds.left + bounds.height - 1; ++x) {
      int start_y = -1;
      int end_y = -1;

      for (int y = bounds.top; y < bounds.top + bounds.height - 1; ++y) {
        if (contains(indices, x, y)) {
          if (start_y < 0) {
            start_y = y;
          }
          end_y = y;
        } else if (start_y >= 0) {
          std::vector<Rect> overlaps = {};
          for (const Rect & r : rectangles) {
            if ((r.end_x == x - 1)
              && (start_y < r.start_y)
              && (end_y >= r.end_y)) {
                overlaps.push_back(r);
              }
          }
          sortVertically(overlaps);

          for (Rect & r : overlaps) {
            if (start_y < r.start_y) {
              const Rect new_rect = {
                x,
                start_y,
                x,
                r.start_y - 1
              };
              rectangles.push_back(new_rect);
              start_y = r.start_y;
            }

            if (start_y == r.start_y) {
              r.end_x = r.end_x + 1;

              if (end_y == r.end_y) {
                start_y = -1;
                end_y = -1;
              } else if (end_y > r.end_y) {
                start_y = r.end_y + 1;
              }
            }
          }

          if (start_y >= 0) {
            const Rect new_rect = {
              x,
              start_y,
              x,
              end_y
            };
            rectangles.push_back(new_rect);

            start_y = -1;
            end_y = -1;
          }
        }
      }

      if (start_y >= 0) {
        const Rect new_rect = {
          x,
          start_y,
          x,
          end_y
        };
        rectangles.push_back(new_rect);

        start_y = -1;
        end_y = -1;
      }
    }

    std::vector<sf::IntRect> int_rects = {};
    for (const Rect r : rectangles) {
      int_rects.push_back(sf::IntRect(
        r.start_x * m_tileset.tileWidth,
        r.start_y * m_tileset.tileHeight,
        r.end_x * m_tileset.tileWidth - r.start_x * m_tileset.tileWidth + m_tileset.tileWidth,
        r.end_y * m_tileset.tileHeight - r.start_y * m_tileset.tileHeight + m_tileset.tileHeight
      ));
    }
    return int_rects;
  }

  const Vec2 &getDimensions() const { return m_dimensions; }

  const Tileset &getTileset() const { return m_tileset; }

  const std::vector<int> &getTiles() const { return m_tiles; }

  const std::vector<EntityData> &getEntities() const { return m_entities; }
};
