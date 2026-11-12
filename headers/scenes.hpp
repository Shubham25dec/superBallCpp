#pragma once
#include "pch.hpp"
#include "level.hpp"
#include "rounded_rectangle.hpp"
#include "utils.hpp"
#include "asset_manager.hpp"


namespace scenes{

using V2f = sf::Vector2f;

inline bool __return_on_exit_event__(const std::optional<sf::Event>& event)
{
    if (event->is<sf::Event::Closed>()) return true;
    if (const auto* key = event->getIf<sf::Event::KeyPressed>()){
      if (key->code == sf::Keyboard::Key::Escape){
        return true;
      }
    }
    return false;
}


//return true if event was sf::TouchBegan, else false.
//updates the touch_info struct if its not active
inline bool __on_touch_begin_event__(const std::optional<sf::Event>& event, utils::TouchInfo& touch_info, sf::RenderWindow& window)
{
  if (const auto* touch = event->getIf<sf::Event::TouchBegan>()){
      if (touch_info.active) return true;
      touch_info.active = true;
      touch_info.fingerid = touch->finger;
      touch_info.pos = window.mapPixelToCoords(touch->position);
      return true;
    }
    return false;
}


// return true if touch_end event of same finger occurs.
// resets the touch_info struct.
inline bool __on_touch_end_event__(const std::optional<sf::Event>& event, utils::TouchInfo& touch_info)
{
  if (const auto* touch = event->getIf<sf::Event::TouchEnded>()){
    if (touch->finger != touch_info.fingerid)
      return false; // not the same finger!!
    touch_info.active = false;
    return true;
  }
  return false;
}


// return true if current finger moved, false otherwise
// updates the touch_info position struct to current touch pos
inline bool __on_touch_move_event__(const std::optional<sf::Event>& event, utils::TouchInfo& touch_info, sf::RenderWindow& window)
{
  if (const auto* touch = event->getIf<sf::Event::TouchMoved>()){
    if (touch->finger != touch_info.fingerid){
      return false;
    }
    touch_info.pos = window.mapPixelToCoords(touch->position);
    return true;
  }
  return false;
}


// Level selection screen
// return -1 if no level selected else levelno
// TODO: scrolling page if too many levels
inline int level_selection(sf::RenderWindow& window, sf::Font* font, sf::Color bg_color={85, 85, 100})
{
  const V2f winsize = (V2f) window.getSize();
  const size_t level_count = gameLevels.size();
  const sf::Color BUTTON_COLOR1 = {125, 125, 200};
  const sf::Color BUTTON_COLOR2 = {0, 45, 45};
  
  const size_t col_count = 5,
         row_count = 10,
         top_pad   = 100, // padding from top
         side_pad  = 10, // padding from left & right
         ib_pad    = 10; // inter-button paddig

  const float button_size = ((winsize.x - side_pad*2 - ((col_count-1) * ib_pad))) / col_count; //button is supposed to be sqaure shape
  
  printf("W:%f, H:%f, levels:%zu, button_size:%f\n", winsize.x, winsize.y, level_count, button_size);
  
  RoundedRectangleShape button({button_size, button_size}, 10);
  button.setFillColor({0, 125, 125}); // cyan

  sf::Text button_text(*font, "0");
  button_text.setFillColor({0, 0, 0}); // black
  button_text.setCharacterSize(button_size/3.f);
    
  size_t current_no = 0;

  //lambda func to update button
  auto update_button = [&button, &button_size] (size_t col, size_t row) {
    const float x = side_pad
              + ( (col!=0) ? ib_pad : 0) * col
              + button_size * col;
    const float y = top_pad
              + ( (row!=0) ? ib_pad : 0) * row
              + button_size * row;
    button.setPosition({x, y});
  };

  // lambda func to update text
  auto update_text = [&current_no, &button_text, &button] (){
    button_text.setString(std::to_string(current_no+1));
    auto bounds = button_text.getLocalBounds();
    button_text.setOrigin(bounds.position + bounds.size/2.f);
    button_text.setPosition(button.getPosition()+button.getLocalBounds().size/2.f);
  };


  int return_value = -1;
  utils::TouchInfo touch_info;
  while (1){
    while (const std::optional event = window.pollEvent()){
      if (__return_on_exit_event__(event)){
        return return_value;
      }
      __on_touch_begin_event__(event, touch_info, window); //sets touch_info struct on sf::TouchBegan event
      
      if (__on_touch_end_event__(event, touch_info)){
        if (return_value != -1){
          return return_value;
        }
      }
      __on_touch_move_event__(event, touch_info, window);

      //NOTE: Temporary block below
      if (const auto* touch = event->getIf<sf::Event::MouseButtonPressed>()){
        if (touch_info.active) continue;
        touch_info.active = true;
        touch_info.pos = window.mapPixelToCoords(touch->position);
      }
      if (event->is<sf::Event::MouseButtonReleased>()){
        if (return_value != -1){
          return return_value;
        }
        touch_info = {};
      }
      if (const auto* touch = event->getIf<sf::Event::MouseMoved>()){
        touch_info.pos = window.mapPixelToCoords(touch->position);
      }
      // NOTE: Temporary block above
  
    }

    window.clear(bg_color);
    current_no = 0;
    return_value = -1;
    bool should_break = false;
    for (size_t row = 0; row < row_count; row++){
      for (size_t col = 0; col < col_count; col++){
        update_button(col, row);
        if (touch_info.active && button.getGlobalBounds().contains(touch_info.pos)){
          return_value = current_no;
          button.setFillColor(BUTTON_COLOR2);
        }else{
          button.setFillColor(BUTTON_COLOR1);
        }
        update_text();
        window.draw(button);
        window.draw(button_text);
        current_no++ ;
        if (current_no >= level_count){
          should_break = true;
          break;
        }
      }
      if (should_break) break;
    }

    window.display();
  } //mainloop

  return return_value;
}// level_selection




inline const std::string about_string = R"(
----------------------------------------------
             Design & Programming
                 Shubham Negi

      Developed on Android, for Android!

 While learning graphics programming with C++
and SFML (sfml-dev.org), I came across a game
 on the Play Store called "Okay?". Later that
  day, I found myself developing a new game:
            my own version of it.

            Here I present to you
                  SuperBall

Your feedback and suggestions are most welcome
         shubhamsnegi.25dec@gmail.com
----------------------------------------------

)";

inline void show_about(sf::RenderWindow& window, sf::Font& font, sf::Color bg_color={26, 26, 65})
{
  V2f win_size = (V2f) window.getSize();
  constexpr float padXY = 20.f;
  sf::Text about_text(font, about_string, win_size.y*0.06f);
  float scaleX = (win_size.x - padXY) / about_text.getLocalBounds().size.x;
  about_text.setScale({scaleX, scaleX});
  // Scaled to fit width (with padding)
  about_text.setFillColor({180, 185, 185});
  const size_t max_index = about_string.length() - 1;
  size_t current_index = 0;
  //lambda to update the about text
  auto update_about_text = [&about_text, &win_size, &current_index](){
    printf("%zu->%c \n", current_index, about_string[current_index]);
    about_text.setString(about_string.substr(0, current_index)+" |");
    const float t_h = about_text.getGlobalBounds().size.y; // height of rendered text
    V2f pos = {padXY/2.f, (win_size.y - t_h)/2.f};
    about_text.setPosition(pos);
  };

  sf::Clock clock;
  const float next_char_delay = 0.02f; // seconds
  float et = 0.0f;
  
  #pragma message("Todo: animate about text ")
  #pragma message("wip : show_about function")

  while (1){
    while (const std::optional event = window.pollEvent()){
      if (__return_on_exit_event__(event)){
        return ;
      }
    }
    et += clock.getElapsedTime().asSeconds();
    if ((current_index < max_index) &&
        (et >= next_char_delay)){
      current_index = current_index < max_index ?current_index+1 : current_index;
      update_about_text();
    }

    window.clear(bg_color);
    window.draw(about_text);
    window.display();
  }
}


typedef enum {
  APP_EXIT,
  ABOUT,
  SETTINGS,
  PLAY
}MainMenuChoice;
//main menu screen
// TODO: ADD settings, about buttons
inline MainMenuChoice main_menu(sf::RenderWindow& window, AssetManager* asset_man, sf::Color bg_color={85, 85, 100})
{
  V2f win_size = (V2f) window.getSize();
  utils::TouchInfo touch_info;
  sf::Clock clock;

  sf::Sprite about_button(asset_man->get_texture_ref("about_button"));
  const float abwidth = win_size.x * 0.1f;
  const float abpadxy = win_size.x * 0.01f; //padding
  utils::scale_sprite_to_width(about_button, abwidth);
  const float abposX = win_size.x - abwidth - abpadxy;
  const float abposY = 0.0f + 0.0f + abpadxy;
  about_button.setPosition({abposX, abposY});
  
  sf::Sprite start_button(asset_man->get_texture_ref("start_button"));
  utils::scale_sprite_to_width(start_button, (win_size.x * 0.5f));

  const auto sbsize = start_button.getGlobalBounds().size;
  const float sbposX = (win_size.x - sbsize.x) / 2.f;
  const float sbposY = (win_size.y - sbsize.y) / 2.f;
  start_button.setPosition({sbposX, sbposY});
  const float freq = 5.f; // how fast button does updown
  const float delY = 0.005f; // how far button moves from original pos (relative to winsize.y)
  
  //mainloop
  while (1){
    // Animate start button
    float y = sinf(clock.getElapsedTime().asSeconds()*freq) * (win_size.y * delY);
    V2f pos = {sbposX, sbposY+y};
    start_button.setPosition(pos);
    
    while (const std::optional event = window.pollEvent()){
      if (__return_on_exit_event__(event)) return APP_EXIT;
      __on_touch_begin_event__(event, touch_info, window);
      
      bool touch_ended = __on_touch_end_event__(event, touch_info);
      if (touch_ended && start_button.getGlobalBounds().contains(touch_info.pos)){
        return PLAY;
      }
      if (touch_ended && about_button.getGlobalBounds().contains(touch_info.pos)){
        show_about(window, asset_man->font);
      }
      __on_touch_move_event__(event, touch_info, window);
    }

    window.clear(bg_color);

    window.draw(start_button);
    utils::draw_sprite_bounds(window, start_button);
    window.draw(about_button);
    utils::draw_sprite_bounds(window, about_button);
    
    window.display();
  }
  
}



}//namespace scenes

