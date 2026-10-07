#pragma once

#include "pch.hpp"
#include <SFML/Audio/Music.hpp>


struct GameAudio{
  sf::SoundBuffer sb;
  sf::Sound sound;
  bool ok = false;

  GameAudio(const std::string path):sound(sb){
    ok = sb.loadFromFile(path);
  }
  
};


struct AudioManager{
  std::unordered_map<std::string, GameAudio> sound_map;
  sf::Music bg_music;
  
  AudioManager(){
  }

  bool load_sound(const std::string& path, const std::string& id){
    if (sound_map.find(id) != sound_map.end()){
      std::cout << "E: sound id already present: " << id << "\n";
      return false;
    }
    sound_map.emplace(id, path);
    bool ok = sound_map.at(id).ok;
    if (!ok) std::cout << "E: failed to load sound from: " << path << "\n";
    return ok;
  }

  bool play_sound(const std::string& id){
    if (sound_map.find(id) != sound_map.end()){
      sound_map.at(id).sound.play();
      return true;
    }
    return false;
  }

  bool load_bg_music(const std::string& path, bool should_loop = true){
    bool isok = bg_music.openFromFile(path);
    bg_music.setLooping(should_loop);
    if (!isok){
      std::cout << "Failed to open music from: " << path << "\n";
      return false;
    }
    return true;
  }

  void start_bg_music(void){
    if (bg_music.getStatus() == sf::Music::Status::Playing) return; //already playing
    bg_music.play();
  }

  void stop_bg_music(void){
    bg_music.stop();
  }
  
};//AudioManager
