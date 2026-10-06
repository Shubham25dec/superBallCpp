#pragma once

#include "pch.hpp"
#include "utils.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>

namespace ui{


//TODO: USE Textures for button instead
//
//
//
//
//
// //TODO: REWRITE TH3 STRUCT

struct TouchButton{
  sf::RectangleShape shape;
  utils::TextSystem text_man;

  TouchButton(
     const std::string& text,
     sf::Vector2f pos    = {100, 100},
     sf::Vector2f size   = {100, 100},
     sf::Color    color  = sf::Color::White,
     sf::Color text_color= sf::Color::Black
   )
  {
    shape.setFillColor(color);
    shape.setSize(size);
    shape.setPosition(pos);
    
    text_man.text.setCharacterSize(60);
    text_man.text.setString(text);
    text_man.text.setPosition(pos);
    text_man.text.setFillColor(text_color);
    auto bounds = text_man.text.getLocalBounds();
    text_man.text.setScale({size.x/bounds.size.x, size.y/bounds.size.x});
  }

  void draw(sf::RenderTarget& target){
    target.draw(shape);
    target.draw(text_man.text);
  }

  bool is_touched(sf::Vector2f touch_pos){
    if (shape.getGlobalBounds().contains(touch_pos)){
      return true;
    }
    return false;
  }
    
};


};//namespace ui
