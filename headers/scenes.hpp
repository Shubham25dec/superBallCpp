#pragma once
#include "pch.hpp"
#include "level.hpp"
#include "rounded_rectangle.hpp"
#include "utils.hpp"

namespace scenes{

using V2f = sf::Vector2f;

// Level selection screen
// return -1 if no level selected else levelno
inline int level_selection(sf::RenderWindow& window, sf::Font& font, sf::Color bg_color={85, 85, 100})
{
  const V2f winsize = (V2f) window.getSize();
  const size_t level_count = gameLevels.size();
  const sf::Color BUTTON_COLOR1 = {125, 125, 200};
  const sf::Color BUTTON_COLOR2 = {0, 45, 45};
  
  const size_t col_count = 5,
         row_count = 10,
         top_pad   = 10, // padding from top
         side_pad  = 10, // padding from left & right
         ib_pad    = 10; // inter-button paddig

  const float button_size = ((winsize.x - side_pad*2 - (col_count * ib_pad))) / col_count; //button is supposed to be sqaure shape
  
  printf("W:%f, H:%f, levels:%zu, button_size:%f\n", winsize.x, winsize.y, level_count, button_size);
  
  RoundedRectangleShape button({button_size, button_size}, 10);
  button.setFillColor({0, 125, 125}); // cyan

  sf::Text button_text(font, "0");
  button_text.setFillColor({0, 0, 0}); // black
  button_text.setCharacterSize(button_size/3.f);
    
  size_t current_no = 0;

  auto update_button = [&button, &button_size, &BUTTON_COLOR1, &BUTTON_COLOR2] (size_t col, size_t row, bool is_selected=false) {
    const float x = side_pad
              + ((col!=0 || col!=col_count) ? ib_pad : 0) * col
              + button_size * col;
    const float y = top_pad
              + ((row!=0 || row!=row_count) ? ib_pad : 0) * row
              + button_size * row;
    button.setPosition({x, y});
    if (is_selected)
      button.setFillColor(BUTTON_COLOR2);
    else
      button.setFillColor(BUTTON_COLOR1);
  };

  auto update_text = [&current_no, &button_text, &button] (){
    button_text.setString(std::to_string(current_no));
    auto bounds = button_text.getLocalBounds();
    button_text.setOrigin(bounds.position + bounds.size/2.f);
    button_text.setPosition(button.getPosition()+button.getLocalBounds().size/2.f);
  };


  int return_value = -1;
  utils::TouchInfo touch_info;
  while (1){
    while (const std::optional event = window.pollEvent()){
      if (event->is<sf::Event::Closed>()) return return_value;
      if (const auto* touch = event->getIf<sf::Event::TouchBegan>()){
        if (touch_info.active) continue;
        touch_info.active = true;
        touch_info.fingerid = touch->finger;
        touch_info.pos = window.mapPixelToCoords(touch->position);
      }
      if (const auto* touch = event->getIf<sf::Event::TouchEnded>()){
        if (touch->finger != touch_info.fingerid) continue;
        if (return_value != -1){
          return return_value;
        }
        touch_info = {};
      }
      if (const auto* touch = event->getIf<sf::Event::TouchMoved>()){
        if (touch->finger != touch_info.fingerid) continue;
        touch_info.pos = window.mapPixelToCoords(touch->position);
      }
    }

    window.clear(bg_color);
    current_no = 0;
    return_value = -1;
    bool should_break = false;
    for (size_t row = 0; row < row_count; row++){
      for (size_t col = 0; col < col_count; col++){
        update_button(col, row, (return_value == (int)current_no));
        if (touch_info.active && button.getGlobalBounds().contains(touch_info.pos)){
          return_value = current_no;
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

  
}//scenes

