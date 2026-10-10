#pragma once

#include "pch.hpp"
#include "utils.hpp"
#include "convex_polygon.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>


using V2f = sf::Vector2f;

enum ExplosionType{
	CHAOTIC,
	CIRCULAR
};

struct Particle{
	V2f velocity;
	V2f original_pos;
	float speed;
	float effect_radius;
	sf::CircleShape shape;

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

	//return true if particle should stay alive
	bool update(float dt){
		V2f cur_pos = shape.getPosition() + velocity * speed * dt;
		shape.setPosition(cur_pos);
		V2f delta_pos = original_pos - cur_pos;
		return delta_pos.length() < effect_radius;
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
           ExplosionType a_type = CIRCULAR,
           size_t a_total = 50,
           V2f a_effect_radius_range = {70.f, 200.f}): rng(a_rng) {
		_generate_particles(a_center, a_size_range, a_speed_range, a_type, a_total, a_color, a_effect_radius_range);
	}

	void _generate_particles(V2f a_center,
	               V2f a_size_range,
	               V2f a_speed_range,
	               ExplosionType a_type,
	               size_t a_total,
	               sf::Color a_color,
	               V2f a_effect_radius_range)	{
		size_t count = 0;
		while (count < a_total){
			V2f position = (a_type == CIRCULAR)? a_center : _rand_pos_within_bounds(a_center.x-50, a_center.y-50, 100, 100);//100x100 rect
			//TODO: do not hardcode the rect size above

			particles.emplace_back(
				position,
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

	//FIXME: find a better way to remove dead particles
	void update(float dt){
		std::vector<Particle> alive_particles;
		for (auto& particle : particles){
			bool alive = particle.update(dt);
			if (alive)
				alive_particles.emplace_back(particle);
		}
		particles = alive_particles;
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

	//FIXME: find a better way to remove dead systems
	void update(float dt){
		std::vector<ParticleSystem> systems;
		for (auto& ps : p_systems){
			ps.update(dt);
			if (ps.has_particles()){
				systems.emplace_back(ps);
			}
		}
		p_systems = systems;
	}

	void add_default_particle_system(V2f at){
		p_systems.emplace_back(rng, at);
	}

	void add_particle_system(ParticleSystem system)	{
		p_systems.emplace_back(system);
	}
	
};// struct ParticleSystemManager




struct PolyDeathAnimation{
	float animation_speed = 50.f;
	ConvexPolygon polygon;
	V2f global_center;
	
	PolyDeathAnimation(ConvexPolygon a_polygon):polygon(a_polygon){
			V2f local_center  = polygon.getGeometricCenter();
			global_center = polygon.getInverseTransform().transformPoint(local_center);
		}

	bool update(float dt){
		//TODO: since we do not apply any transformations to the polygon, maybe it is not necessary to
		// convert to global coordinates, but that may change if I fix the scaling issue for other screen ratio..
		// TODO end
		
		for (size_t i=0; i<polygon.getPointCount(); i++){
			V2f ith = polygon.getInverseTransform().transformPoint(polygon.getPoint(i)); //in global
			V2f dir = (global_center - ith); //points towards center
			float DeathThreshold = 5.f;
			if (dir.length() != 0){
				dir = dir.normalized();
			}else if (dir.length() <= DeathThreshold){
				return false;
			}
			ith += (dir * animation_speed * dt);
			polygon.setPoint(i, ith);
		}
		return true;
	}

	void draw(sf::RenderTarget& target){
		target.draw(polygon);
	}
	
}; //struct PolyDeathAnimation



struct PolyDeathAnimationManager{
	std::vector<PolyDeathAnimation> anims;

	void add_animation(ConvexPolygon polygon){
		anims.emplace_back(polygon);
	}

	void update(float dt){
		//TODO: find a better way to remove dead anims
		// FIXME
		std::vector<PolyDeathAnimation> alive_anims;
		for (auto& anim : anims){
			bool is_alive = anim.update(dt);
			if (is_alive){
				alive_anims.emplace_back(std::move(anim));
			}
		}
		anims = alive_anims;
	}

	void draw(sf::RenderTarget& target) const{
		for (auto& anim: anims){
			anim.draw(target);
		}
	}
}; //PolyDeathAnimationManager
