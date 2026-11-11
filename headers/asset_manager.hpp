#pragma once
#include "pch.hpp"
#include <cstdint>

// default texture data to load when an image is missing
const std::uint8_t Default_Texture_Data[] = {
  0xE1, 0x62, 0xE0, 0xff,  0x00, 0x00, 0x00, 0xff,
  0x00, 0x00, 0x00, 0xff,  0xE1, 0x62, 0xE0, 0xff
}; // 2x2 default pink-black checkerboard texture

inline bool load_default_texture(sf::Texture& texture)
{
  if (!texture.resize({2, 2})) return false;
  texture.update(Default_Texture_Data);
  texture.setSmooth(false);
  return true;
}

using AssetMap = std::unordered_map<std::string, sf::Texture> ;

struct AssetManager{
  AssetManager(const std::string& font_path){
    if (!font.openFromFile(font_path)){
      fprintf(stderr, "Fatal: Failed to load required font from path: %s \n", font_path.c_str());
      exit(1);
    }
    if (!load_default_texture(default_texture)){
      fprintf(stderr, "Fatal: Failed to load default texture during initialization of AssetManager\n");
      exit(1);
    }
    default_texture.setSmooth(false);
  }
  
  // (doesn't exit on error)
  void load_texture(const std::string& path, const std::string& id, bool smooth=true){
    if (asset_map.find(id) != asset_map.end()){
      fprintf(stderr, "Error: Texture with id %s already exists. Ignoring this call!\n", id.c_str());
      return;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(path)){
      fprintf(stderr, "Error: Failed to load texture from path: %s\n", path.c_str());
      return;
    }
    texture.setSmooth(smooth);
    asset_map[id] = std::move(texture);
  }

  // safe (uses default texture on error)
  sf::Texture& get_texture_ref(const std::string& id){
    if (asset_map.find(id) == asset_map.end()){
      fprintf(stderr, "Error: No texture with id '%s' was loaded.\n", id.c_str());
      printf("Using default texture for id: '%s'", id.c_str());
      return default_texture;
    }
    return asset_map.at(id);
  }
  
private:
  AssetMap asset_map;
  sf::Texture default_texture;

public:
  sf::Font font;
  
}; // AssetManager
