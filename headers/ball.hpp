#pragma once

#include "pch.hpp"
#include "convex_polygon.hpp"
#include "particles.hpp"

using V2f = sf::Vector2f;

typedef struct LineIntersectionResult{
  bool intersects = false;
  V2f at;
  V2f line_start; //start of the line which intersects
  V2f line_end;
}LI_Result;


struct BallUpdateResult{
  bool ball_dies = false;
  bool ball_collides = false;
  //TODO: utilize below member to draw particles on collion from Game() instead of passing the particleManager to Ball
  // V2f collision_pos = {0.f, 0.f};
};

inline float distance(V2f A, V2f B){
  /*get distance between pts A and B*/
  float dx = B.x - A.x;
  float dy = B.y - A.y;
  return sqrtf(dx*dx + dy*dy);
}

inline LI_Result lines_intersect(V2f A, V2f B, V2f C, V2f D){
  /*
  determine if line AB intersects line CD
  */
  float x1, y1, x2, y2, x3, y3, x4, y4;
  x1 = A.x;  x2 = B.x;  x3 = C.x;  x4 = D.x;
  y1 = A.y;  y2 = B.y;  y3 = C.y;  y4 = D.y;
  LI_Result result;
  float d = ((x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4));
  if (d == 0){
    return result; //no intersection
  }
  float t_nume = ((x1-x3) * (y3-y4)) - ((y1-y3) * (x3-x4));
  float t = t_nume/d;
  float u_nume = -(((x1-x2) * (y1-y3)) - ((y1-y2) * (x1-x3)));
  float u = u_nume/d;
  if (!((0 <= t && t <= 1) && (0 <= u && u <= 1))){
    return result; //no intersection
  }
  float px = x1 + t * (x2-x1);
  float py = y1 + t * (y2-y1);
  result.intersects = true;
  result.at = {px, py};
  result.line_start = C;
  result.line_end = D;
  return result;
}



struct Ball{
  V2f start_pos;
  V2f end_pos;
  V2f velocity;
  float speed;
  bool isdead = true;
  sf::CircleShape ball_shape;

  Ball(V2f a_start_pos={0, 0}, V2f a_velocity={1, 1}, float radius=11, float a_speed = 740,
       sf::Color a_color = sf::Color(255, 165, 0)){
    start_pos = a_start_pos;
    velocity = a_velocity;
    end_pos = start_pos;
    speed = a_speed;
    ball_shape.setRadius(radius);
    ball_shape.setOrigin(ball_shape.getLocalBounds().size * 0.5f);
    ball_shape.setPosition(end_pos);
    ball_shape.setFillColor(a_color);
  }

  
  BallUpdateResult update(const float dt, Polygons& polygons,
              std::vector<std::array<V2f, 2>>& lines,
              ParticleSystemManager* particle_sys_man = nullptr
            ){
    BallUpdateResult result;
    if (isdead) return result;//since already dead
    end_pos += (velocity * speed * dt);
    ball_shape.setPosition(end_pos);

    result.ball_collides = _handle_polygon_collision(polygons, particle_sys_man);
    result.ball_dies = _handle_line_collision(lines, particle_sys_man);
    return result;
  }

  void draw(sf::RenderWindow& window) const{
    if (isdead) return;
    window.draw(ball_shape);

    //debugging
    /*
    sf::CircleShape shape(2);
    shape.setPosition(start_pos);
    window.draw(shape);
    shape.setPosition(end_pos);
    window.draw(shape);
    */
  }

  //return true if collision happens
  bool _handle_polygon_collision(Polygons& polygons, ParticleSystemManager* particle_sys_man){
    float min_dist = 999999999999.0f;
    int closest_index = -1;
    LI_Result ci_result; //closest intersection result
    for (size_t i=0; i<polygons.size(); i++){
      auto result = _collides_with_polygon(polygons[i]);
      if (result.intersects){
        std::cout << "ball-poly collision at (" << result.at.x << "," << result.at.y << ")\n";
        float dist = distance(result.at, start_pos);
        if (dist < min_dist){
          min_dist = dist;
          closest_index = i;
          ci_result = result;
        }
      }
    }
    if (closest_index != -1){ //collision happend
      auto collided = polygons[closest_index];
      auto particle_color = sf::Color::Black;
      if (collided.is_mortal){
        particle_color = sf::Color::White;
        polygons.erase(polygons.begin()+closest_index);
      }//remove polygon if it is mortal
      _reflect_from_line(ci_result.line_start, ci_result.line_end);
      start_pos = ci_result.at + velocity * 1.0f;
      end_pos = start_pos;
      if (particle_sys_man){
        particle_sys_man->add_particle_system(
              ParticleSystem(particle_sys_man->rng,
                             ci_result.at,
                             particle_color,
                             {1.5f, 3.5f},
                             {100.f, 200.f},
                             CIRCULAR,
                             50,
                             {70.f, 200.f})
        );
      }
      return true;
    }
    return false;
  }

  // return whether ball dies
  bool _handle_line_collision(std::vector<std::array<V2f, 2>>& lines, ParticleSystemManager* particle_sys_man){
    float min_dist = 999999999999.0f;
    int closest_index = -1;
    LI_Result ci_result;
    for (size_t i=0; i<lines.size(); i++){      auto result = lines_intersect(lines[i][0], lines[i][1], start_pos, end_pos);
      if (result.intersects){
        float dist = distance(result.at, start_pos);
        if (dist < min_dist){
          min_dist = dist;
          closest_index = i;
          ci_result = result;
        }
      }
    }
    if (closest_index != -1){//collision happened
      lines.erase(lines.begin() + closest_index);
      isdead = true;
      if (particle_sys_man){
        particle_sys_man->add_particle_system(
              ParticleSystem(particle_sys_man->rng,
                             ci_result.at,
                             ball_shape.getFillColor(),
                             {1.5f, 3.5f},
                             {100.f, 200.f},
                             CIRCULAR,
                             50,
                             {70.f, 200.f})
          );
      }
      return true; //ball dies, game should reset level
    }
    return false; //ball didnt die;
  }
  


  LI_Result _collides_with_polygon(const ConvexPolygon& poly){
    LI_Result result;
    size_t pc = poly.getPointCount();
    for (size_t x=0; x<pc; x++){
      V2f A = poly.getPoint(x);
      V2f B;
      if (x == pc-1){//last vertex
        B = poly.getPoint(0);
      }else{
        B = poly.getPoint(x+1);
      }
      result = lines_intersect(start_pos, end_pos, A, B);
      if (!result.intersects) continue;
      return result;
    }
    return result;
  }


  void _reflect_from_line(V2f A, V2f B){
    /* change the velocity of ball such as if it
       collided with line AB and reflected smoothly
    */
    V2f line_dir = B - A;
    line_dir = line_dir.normalized();
    V2f normal1 = V2f(-line_dir.y, line_dir.x);
    V2f normal2 = V2f(line_dir.y, -line_dir.x);
    V2f normal  = (velocity.dot(normal1) > 0) ? normal1 : normal2;
    float dot_product = velocity.dot(normal);
    velocity = velocity - 2 * dot_product * normal;
    //reflected the velocity of ball
  }

};//struct Ball

