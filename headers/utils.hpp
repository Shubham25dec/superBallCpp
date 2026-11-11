#pragma once
#include "pch.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


namespace utils{


using V2f = sf::Vector2f;

typedef struct TouchInfo{
  V2f pos;
  unsigned int fingerid;
  bool active = false;
}TouchInfo;


[[nodiscard]]
inline sf::RectangleShape makeLine(V2f a, V2f b, float thickness, sf::Color color)
{
    V2f d = b - a;
    float length = std::hypot(d.x, d.y);

    sf::RectangleShape rect({length, thickness});
    rect.setOrigin({0.f, thickness / 2.f});
    rect.setPosition(a);
    rect.setRotation(sf::radians(std::atan2(d.y, d.x)));
    rect.setFillColor(color);
    return rect;
}

[[nodiscard]]
inline sf::CircleShape makeCircle(float radius, V2f center, sf::Color color=sf::Color::White){
    sf::CircleShape cs;
    cs.setRadius(radius);
    cs.setPosition(center);
    cs.setOrigin(cs.getLocalBounds().size/2.0f);
    cs.setFillColor(color);
    return cs;
}


struct RNG{
    std::random_device rd;  // Random seed
    std::mt19937 gen; //rn generator

    RNG():gen(rd()){
    }
    
    int randInt(int min, int max){
            std::uniform_int_distribution<> dist(min, max);
            int randomNumber = dist(gen);
            return randomNumber;
    }

    float randFloat(float min=0.0f, float max=1.0f) {
            std::uniform_real_distribution<float> dist(min, max);
            float randomFloat = dist(gen);
            return randomFloat;
    }

    template <typename T> T randChoice(std::vector<T>& list){
            std::uniform_int_distribution<> dist(0, list.size()-1);
            int index = dist(gen);
            return list.at(index);
    }
};


struct FpsVusaliser{
  sf::Clock fps_clock;
  sf::Font* font;
  sf::Text text;
  float timer = 0.0f;
  
  FpsVusaliser(sf::Font* a_font, size_t a_charsize=20): font(a_font), text(*font)
  {
      text.setCharacterSize(a_charsize);
      text.setString("fps: calculating...");
      text.setPosition({10, 10});
      text.setFillColor(sf::Color(255, 15,15));
  }

  // return dt
  float show_fps(sf::RenderWindow& window, V2f at={10.0f, 10.0f}){
      float dt = fps_clock.restart().asSeconds();
      timer += dt;
      if (timer >= 0.5f){
          text.setString("fps: " + std::to_string(1/dt));
          timer = 0.0f;
      }
      text.setPosition(at);
      window.draw(text);
      return dt;
  }
};//Fpsvisualizer



struct TextSystem{
    sf::Font* font;
    sf::Text text;
    size_t initial_size;
    
    TextSystem(sf::Font* a_font, size_t a_charsize=20): font(a_font), text(*font)
    {
        text.setCharacterSize(a_charsize);
        initial_size = a_charsize;
    }

    void show_text(sf::RenderTarget& target, const std::string& msg, V2f at, sf::Color color=sf::Color::White, size_t current_charsize=0){
        text.setPosition(at);
        text.setFillColor(color);
        if (current_charsize != 0){
            text.setCharacterSize(current_charsize);
        }
        text.setString(msg);
        target.draw(text);
        if (current_charsize != 0){
            text.setCharacterSize(initial_size);
        }
    }
};//TextSytem


inline float distance(V2f A, V2f B){
    float dx = A.x - B.x;
    float dy = A.y - B.y;
    return std::sqrtf(dx*dx + dy*dy);
}



//Draw arrow cap at the end_pos of a line
inline void draw_arrow_cap(sf::RenderTarget& target, V2f start_pos, V2f end_pos,
        float barb_length = 20.0f,
        float thickness = 3.0f,
        sf::Color color = sf::Color::Red,
        sf::Angle spread = sf::degrees(45))
{
    V2f dir = end_pos - start_pos;
    if (dir == V2f{0.f, 0.f}) return;

    //vctor pointing back along the line, scaled to barb length
    V2f back = -dir.normalized() * barb_length;

    target.draw(makeLine(end_pos, end_pos + back.rotatedBy(spread),  thickness, color));
    target.draw(makeLine(end_pos, end_pos + back.rotatedBy(-spread), thickness, color));
}


inline void draw_sprite_bounds(sf::RenderWindow& window,
        sf::Sprite& sprite,
        sf::Color color = sf::Color::Red,
        float width = 4.f)
{
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::RectangleShape rect(bounds.size);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(width);
    rect.setOutlineColor(color);
    rect.setPosition(sprite.getPosition());
    window.draw(rect);
}

// scale the sprite such that its width becomes equal
// to target_width. Height is scaled such that it keeps
// the original aspect ratio of its texture
inline void scale_sprite_to_width(sf::Sprite& sprite, float target_width){
    float scaleX = target_width / sprite.getLocalBounds().size.x;
    sprite.setScale({scaleX, scaleX});
}

/*
inline void draw_dotted_line(sf::RenderTarget& target,
            V2f start, V2f end,
            float dot_radius = 4.f,
            size_t dot_count = 10,
            sf::Color color  = sf::Color::Black)
{
        V2f dirreal = (end-start);
        if (dirreal.length() == 0){
            return;
        }
        V2f dir = dirreal.normalized();
        float dot_gap = dirreal.length() / (dot_count - 1);
        sf::CircleShape shape(dot_radius);
        shape.setFillColor(color);
        for (size_t i=0; i<dot_count; i++){
            shape.setPosition(start);
            target.draw(shape);
            start += (dir * dot_gap);
        }
}
*/


};//namespace utils
