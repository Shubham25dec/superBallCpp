#pragma once
#include "pch.hpp"

/*

The AssetManager and its bound method are not
 error safe methods! Meaning it will exit if
 there was an error while loading (and/or accessing
 a texture that was not loaded).
 Unlike the AudioManager (defined in audio_system.hpp),
  which does not exit if an error occurs while
  loading, playing, a sound!
*/

using AssetMap = std::unordered_map<std::string, sf::Texture> ;

struct AssetManager{
  AssetManager(const std::string& font_path){
    if (!font.openFromFile(font_path)){
      fprintf(stderr, "Fatal: Failed to load font from path: %s \n", font_path.c_str());
      exit(1);
    }
  }

  void load_texture(const std::string& path, const std::string& id, bool smooth=true){
    if (asset_map.find(id) != asset_map.end()){
      fprintf(stderr, "Error: Texture with id %s already exists. Ignoring this call!\n", id.c_str());
      return;
    }
    sf::Texture texture;
    if (!texture.loadFromFile(path)){
      fprintf(stderr, "Fatal: Failed to load texture from path: %s\n", path.c_str());
      exit(1);
    }
    texture.setSmooth(smooth);
    asset_map[id] = std::move(texture);
  }

  sf::Texture& get_texture_ref(const std::string& id){
    if (asset_map.find(id) == asset_map.end()){
      fprintf(stderr, "Fatal: No texture with id '%s' was loaded.\n", id.c_str());
      exit(1);
    }
    return asset_map.at(id);
  }
  
private:
  AssetMap asset_map;

public:
  sf::Font font;
  
}; // AssetManager
