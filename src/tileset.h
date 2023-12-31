#pragma once
#include <string>

struct Tileset {
  std::string name;
  std::string texturePath;
  size_t tileWidth;
  size_t tileHeight;
  size_t width;
  size_t height;
};