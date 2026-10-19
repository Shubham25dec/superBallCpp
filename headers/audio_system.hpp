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



//Plays 8 notes in increasing order, when the notes are played plays them in reverse and so on..
struct PianoNotePlayer{
  AudioManager* audio_man;
  const size_t MAX_ID = 8;
  const size_t MIN_ID = 1;
  int note_delta = 1; //whether notes go up or down 1 meansnup -1 down
  size_t current = MIN_ID;
  PianoNotePlayer(AudioManager* a_audio_man):audio_man(a_audio_man){
    for (size_t i=1; i<9; i++){
      auto name = std::to_string(i);
      audio_man->load_sound("sounds/"+name+".ogg", name);
    }
  }

  void play_next_note(){
    auto id = std::to_string(current);
    audio_man->play_sound(id);
    current += note_delta;
    if (current > MAX_ID){
      current -= 1;
      note_delta *= -1;
    }else if (current < MIN_ID){
      current += 1;
      note_delta *= -1;
    }
  }

  void reset_note(){
    current = 1;
  }
  
}; //PianoNotePlayer
