#pragma once

#include "level.hpp"
#include "pch.hpp"
#include "utils.hpp"
#include "convex_polygon.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Keyboard.hpp>

using V2f = sf::Vector2f;

struct Particle{
	V2f velocity;
	V2f original_pos;
	float speed;
	float effect_radius;
	sf::CircleShape shape;
	bool isdead = false;

	Particle(V2f a_pos, float a_radius, sf::Color a_color, float a_angle, float a_speed, float a_effect_radius){
		velocity = V2f(1.f, 1.f).normalized().rotatedBy(sf::degrees(a_angle));
		original_pos = a_pos;
		speed = a_speed;
		effect_radius = a_effect_radius;
		shape.setRadius(a_radius);
		auto bounds = shape.getLocalBounds();
		shape.setOrigin(bounds.size/2.f);
		shape.setPosition(a_pos);
		shape.setFillColor(a_color);
	}

	void draw(sf::RenderTarget& target) const{
		target.draw(shape);
	}

	void update(float dt){
		V2f cur_pos = shape.getPosition() + velocity * speed * dt;
		shape.setPosition(cur_pos);
		V2f delta_pos = original_pos - cur_pos;
		if (delta_pos.length() >= effect_radius){
			isdead = true;
		}
	}
	
}; // struct Particle



struct ParticleSystem{
	std::vector<Particle> particles;
	utils::RNG* rng;
	
	ParticleSystem(utils::RNG* a_rng,
           V2f a_center,
           sf::Color a_color = sf::Color::White,
           V2f a_size_range = {1.5f, 3.5f},
           V2f a_speed_range = {100.f, 200.f},
           size_t a_total = 50,
           V2f a_effect_radius_range = {70.f, 200.f}): rng(a_rng) {
		_generate_particles(a_center, a_size_range, a_speed_range, a_total, a_color, a_effect_radius_range);
	}

	void _generate_particles(V2f a_center,
	               V2f a_size_range,
	               V2f a_speed_range,
	               size_t a_total,
	               sf::Color a_color,
	               V2f a_effect_radius_range)	{
		size_t count = 0;
		while (count < a_total){
			particles.emplace_back(
				a_center,
				rng->randFloat(a_size_range.x, a_size_range.y),
				a_color,
				rng->randFloat(0.f, 360.f),
				rng->randFloat(a_speed_range.x, a_speed_range.y),
				rng->randFloat(a_effect_radius_range.x, a_effect_radius_range.y)
			);
			count ++;
		}
		
	}

	void draw(sf::RenderTarget& target) const{
		for (const auto& particle: particles){
			particle.draw(target);
		}
	}

	void update(float dt){
		for (auto& particle : particles){
			particle.update(dt);
		}
		//remove dead particles
		particles.erase(std::remove_if(particles.begin(), particles.end(),
		  	[](const Particle& p){return p.isdead;}),
		  	particles.end());
	}

	bool has_particles() const{
		return particles.size() > 0;
	}
	
	V2f _rand_pos_within_bounds(float x, float y, float w, float h){
		float xpos = rng->randFloat(x, y);
		float ypos = rng->randFloat(x+w, y+h);
		return V2f(xpos, ypos);
	}
	
};// struct ParticleSystem



struct ParticleSystemManager{
	std::vector<ParticleSystem> p_systems;
	utils::RNG* rng;
	
	ParticleSystemManager(utils::RNG* a_rng):rng(a_rng){
	}

	void draw(sf::RenderTarget& target) const{
		for (const auto& ps : p_systems){
			ps.draw(target);
		}
	}

	void update(float dt){
		for (auto& ps : p_systems){
			ps.update(dt);
		}
		//remove dead systems
		p_systems.erase(std::remove_if(p_systems.begin(), p_systems.end(),
		  [](const ParticleSystem& ps){return !ps.has_particles();}),
		    p_systems.end());
	}

	void add_default_particle_system(V2f at){
		p_systems.emplace_back(rng, at);
	}

	void add_particle_system(ParticleSystem system)	{
		p_systems.emplace_back(system);
	}
	
};// struct ParticleSystemManager



struct PolyDeathAnimation{
  static constexpr float animation_speed = 50.f;
  static constexpr float ol_thickness = 5.0f;
	static constexpr float fade_rate = 255.0f / 0.7f;
  //defines how fast alpha fades. 0.7 secs in this case
  uint8_t alpha = 255;
  V2f global_center;
  ConvexPolygon polygon;

  bool is_dead = false;

  PolyDeathAnimation(ConvexPolygon a_polygon):polygon(a_polygon){
    V2f local_center  = polygon.getGeometricCenter();
    global_center = polygon.getTransform().transformPoint(local_center);
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(sf::Color::Black);
    polygon.setOutlineThickness(ol_thickness);
  }

	void update(float dt){
    for (size_t i=0; i<polygon.getPointCount(); i++){
      V2f pt = polygon.getPoint(i);
      V2f dir = (polygon.getTransform().transformPoint(pt) - global_center).normalized();
      pt += (dir * dt * animation_speed);
      polygon.setPoint(i, pt);
    }
    sf::Color c = polygon.getOutlineColor();
    polygon.setOutlineColor({c.r, c.g, c.b, alpha});
    float f_alpha = (float) alpha;
    f_alpha -= fade_rate * dt;
    if (f_alpha <= 0.0f){
      is_dead = true;
      return;
    }
    alpha = (uint8_t)f_alpha;
    return;
  }

  inline void draw(sf::RenderTarget& target) const{
    target.draw(polygon);
  }

}; //struct PolyDeathAnimation0


struct PolyDeathAnimationManager{
  std::vector<PolyDeathAnimation> anims;
  void add_animation(ConvexPolygon polygon){
    anims.emplace_back(polygon);
  }

  void update(float dt){
    for (auto& anim : anims){
      anim.update(dt);
    }
    //remove dead anims
    anims.erase(std::remove_if(anims.begin(), anims.end(),
      [](const PolyDeathAnimation& anim){return anim.is_dead;}),
      anims.end());
  }

  void draw(sf::RenderTarget& target) const{
    for (auto& anim: anims){
      anim.draw(target);
    }
  }
}; //PolyDeathAnimationManager



struct PolySpawnAnimation{
	Level* level;
	sf::Clock clock;
	const float default_time_del = 0.5f;
	size_t cur_max_id = 0;
	float time_del = default_time_del; //delay before drawing next polygon (in secs)
	const float divisor = 1.1; //divide time_del by this amount for next polygon each time. This causes time_del
	// to be reduced after drawing each polygon
	// so the subsequent polygons drawn faster and faster
	bool is_done = false;//whether animation is complete
	
	PolySpawnAnimation(Level* a_level):level(a_level){
	}

	void draw(sf::RenderTarget& target) const{
		for (size_t i=0; i<=cur_max_id; i++){
			target.draw(level->polygons[i]);
		}
	}

	void update(float dt){
		if (is_done) return;
		float et = clock.restart().asSeconds();
		if (et >= time_del){
			cur_max_id += 1;
			time_del /= divisor;
			if (cur_max_id >= level->polygons.size()){
				cur_max_id -= 1;
				is_done = true;
			}
		}
	}

	void reset(Level* a_level){
		time_del = default_time_del;
		is_done = false;
		cur_max_id = 0;
		level = a_level;
		clock.restart();
	}
	
};

