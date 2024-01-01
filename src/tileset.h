#pragma once
#include <string>

struct Tileset {
  std::string name;
  std::string texturePath;
  unsigned int tileWidth;
  unsigned int tileHeight;
  unsigned int width;
  unsigned int height;
};