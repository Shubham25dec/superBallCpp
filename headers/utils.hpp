#pragma once
#include "pch.hpp"


namespace utils{
    

[[nodiscard]]
inline sf::RectangleShape makeLine(sf::Vector2f a, sf::Vector2f b, float thickness, sf::Color color)
{
    sf::Vector2f d = b - a;
    float length = std::hypot(d.x, d.y);

    sf::RectangleShape rect({length, thickness});
    rect.setOrigin({0.f, thickness / 2.f});
    rect.setPosition(a);
    rect.setRotation(sf::radians(std::atan2(d.y, d.x)));
    rect.setFillColor(color);
    return rect;
}

[[nodiscard]]
inline sf::CircleShape makeCircle(float radius, sf::Vector2f center, sf::Color color=sf::Color::White){
    sf::CircleShape cs;
    cs.setRadius(radius);
    cs.setPosition(center);
    cs.setOrigin(cs.getLocalBounds().size/2.0f);
    cs.setFillColor(color);
    return cs;
}


typedef struct TouchInfo{
  sf::Vector2i pos;
  unsigned int fingerid;
  bool active = false;
}TouchInfo;


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
  sf::Font font;
  sf::Text text;
  float timer = 0.0f;

  bool font_ok = true;
  
  FpsVusaliser(const std::string& font_path="font.ttf", size_t a_charsize=20): text(font){
      if (!font.openFromFile(font_path)){
          std::cout << "Failed to load font from: " << font_path << "\n";
          font_ok = false;
      }
      text.setCharacterSize(a_charsize);
      text.setString("fps: calculating...");
      text.setPosition({10, 10});
      text.setFillColor(sf::Color(255, 15,15));
  }

  float show_fps(sf::RenderWindow& window, sf::Vector2f at={10.0f, 10.0f}){
      float dt = fps_clock.restart().asSeconds();
      if (!font_ok) return dt;
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
    sf::Text text;
    sf::Font font;
    size_t initial_size;
    bool font_ok = true;
    
    TextSystem(const std::string& font_path="font.ttf", size_t a_charsize=20):text(font){
        if (!font.openFromFile(font_path)){
          std::cout << "Failed to load font from: " << font_path << "\n";
          font_ok = false;
        }
        text.setCharacterSize(a_charsize);
        initial_size = a_charsize;
    }

    void show_text(sf::RenderTarget& target, const std::string& msg, sf::Vector2f at, sf::Color color=sf::Color::White, size_t current_charsize=0){
        if (!font_ok) return;
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


inline float distance(sf::Vector2f A, sf::Vector2f B){
    float dx = A.x - B.x;
    float dy = A.y - B.y;
    return std::sqrtf(dx*dx + dy*dy);
}



using V2f = sf::Vector2f;
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

};//namespace utils
