#pragma once

#include "pch.hpp"
#include "utils.hpp"

typedef bool ToggleOpt;
#define OFF  false
#define ON   true

struct Settings{
  ToggleOpt music = OFF;
  ToggleOpt sound = ON;
  ToggleOpt particles = OFF;
  ToggleOpt show_fps = ON;
  ToggleOpt aim_line = ON;
  ToggleOpt shape_shadows = ON;

  //window //TODO: decide how to implement such a setting while playing the game.
  // one option specific to MSAA could be to draw everything on a separate texture with AA
  // and then drawing that texture to window
  // that way if this setting changes mid game
  // we just need to reassign the texture
  //
  // TODO: think of more settings maybe some ui related stuff
  // 
  ToggleOpt MSAA=OFF; //anti-aliasing

};



inline void settings_screen(sf::RenderWindow& window, Settings& settings, const sf::Font& font){
  sf::Text text(font);
  sf::RectangleShape rect;
  text.setCharacterSize(40);
  float padx = 20.f;
  float pady = 10.f;
  text.setString("shape shadows");
  float text_x = 10.f;
  float rect_x = text.getLocalBounds().size.x + padx + text_x;
  sf::Vector2f rect_size = {40.f, 40.f};
  rect.setSize(rect_size);
  rect.setOutlineColor(sf::Color::White);
  rect.setOutlineThickness(4);
  
  const size_t SETTING_COUNT = 7;
  std::string names[SETTING_COUNT] = {"music", "sound", "particles", "show fps", "aim line", "shape shadows", "MSAA" };
  ToggleOpt* options[SETTING_COUNT] = {&settings.music, &settings.sound, &settings.particles, &settings.show_fps, &settings.aim_line, &settings.shape_shadows, &settings.MSAA};
  
  utils::TouchInfo touch_info;
  while (1){
    while (const std::optional event = window.pollEvent()){
      if (event->is<sf::Event::Closed>()){
        return;
      }
      if (const auto* key_press = event->getIf<sf::Event::KeyPressed>()){
        return;
      }
      if (const auto* touch = event->getIf<sf::Event::TouchBegan>()){
        if (touch_info.active) continue;
        touch_info = {touch->position, touch->finger, true};
      }
      if (const auto* touch = event->getIf<sf::Event::TouchEnded>()){
        if (touch_info.fingerid == touch->finger){//ensure same finger
          touch_info.active = false;
        }
      }
    }

    //Rendering
    window.clear(sf::Color(85, 85, 85));

    float rect_y = 10.f;
    rect.setPosition({text_x, text_x});
    text.setString("--Settings--");
    text.setPosition({text_x + rect_size.x, text_x});
    rect.setFillColor(sf::Color::Cyan);
    window.draw(rect); //Back button;
    window.draw(text);
    if (touch_info.active && rect.getGlobalBounds().contains(sf::Vector2f(touch_info.pos))){//back button pressed
      return;
    }
    rect_y += rect_size.y + pady;
    for (size_t i=0; i<SETTING_COUNT; i++){
      text.setString(names[i]);
      text.setPosition({text_x, rect_y});
      rect.setPosition({rect_x, rect_y});
      ToggleOpt current = *options[i];
      sf::Color color = (current==ON)? sf::Color::Green : sf::Color::Transparent;
      rect.setFillColor(color);
      window.draw(text);
      window.draw(rect);
      if (touch_info.active && rect.getGlobalBounds().contains(sf::Vector2f(touch_info.pos))){
          *options[i] = (current==ON)? OFF : ON;
          touch_info.active  = false;
      }
      rect_y += rect_size.y + pady;
    }
    window.display();
    
  }
}
