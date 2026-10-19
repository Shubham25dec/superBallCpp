#pragma once
#include "pch.hpp"
#include "convex_polygon.hpp"
#include "ball.hpp"
#include "level.hpp"
#include "utils.hpp"
#include "audio_system.hpp"
#include "button.hpp"
#include "animation.hpp"
#include "settings.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <string>


//all polygon vertices are based on this size, to
// make sure they remain in this exact ratio,
// we a sf::View of this size
inline constexpr sf::Vector2f VIEW_SIZE = {720.f, 1200.f};

typedef enum BallState{
  MOVING,
  MAKING,
  UNBORN
}BallState;


struct Game{
  size_t levelno=0;
  Level current_level;
  Ball ball;
  BallState ball_state = UNBORN;
  sf::Vector2u win_size;
  AudioManager* audio_man;
  PianoNotePlayer piano_np;
  utils::RNG rng;
  ParticleSystemManager particle_system_man;
  PolyDeathAnimationManager pda_man;
  PolySpawnAnimation ps_anim;

  sf::View game_view;
  //sf::View ui_view; //pixel based view for buttons, text etc
  
  Settings settings;

  utils::TouchInfo touch_info;
  //TODO: SSSSSSSSSSSSSSSSS
  //
  // 
  ui::TouchButton temp_nxt_btn;
  ui::TouchButton temp_pre_btn;
  ui::TouchButton temp_setting_btn;
  //
  //
  // TODO:SSSSSSSSSSSSSSSSSSS
  
  Game(sf::RenderWindow& window, AudioManager* a_audio_man):current_level(gameLevels[levelno]),
    audio_man(a_audio_man),
    piano_np(audio_man),
    particle_system_man(&rng),
    ps_anim(&current_level),
    temp_nxt_btn("next", {window.getSize().x-window.getSize().x/6.f, window.getSize().y-window.getSize().y/5.f}),
    temp_pre_btn("prev", {window.getSize().x/36.f, window.getSize().y-window.getSize().y/5.f}),
    temp_setting_btn("settings", {window.getSize().x/2.f-50, window.getSize().y-window.getSize().y/5.f})
  {

    audio_man->load_bg_music("sounds/bg_music.mp3");
    audio_man->load_sound("sounds/die.mp3", "die");
    audio_man->load_sound("sounds/touch.wav", "hit");
      
    _load_current_level();
    _update_views(window);
  }

  //set view so it properly resizes regardless of the screen res
  void _update_views(sf::RenderWindow& window){
    win_size = window.getSize();

    //game view: fixed 720x1200 world, uniformly scaled and centered (letterboxed)
    game_view.setSize(VIEW_SIZE);
    game_view.setCenter(VIEW_SIZE/2.f);
    float scale = std::min(win_size.x/VIEW_SIZE.x, win_size.y/VIEW_SIZE.y);
    V2f wld_size = VIEW_SIZE * scale;
    float vp_width = wld_size.x/win_size.x;
    float vp_height = wld_size.y/win_size.y;
    float left = (1 - vp_width) / 2.f;
    float top = 0.f; //(1-vp_height) /2.f;
    const sf::FloatRect view_rect({left, top}, {vp_width, vp_height});
    game_view.setViewport(view_rect);

    //ui view: 1 unit = 1 real pixel, covers the whole window
    //ui_view.setSize(V2f(win_size));
    //ui_view.setCenter(V2f(win_size)/2.f);
    //ui_view.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));

    window.setView(game_view);
  }

  void mainloop(sf::RenderWindow& window){
    sf::Clock clock;
    utils::FpsVusaliser fps_viz("font.ttf", 20);
    utils::TextSystem text_sys("font.ttf", 20);
  
    if (settings.music){
      audio_man->start_bg_music();
    }
    float dt;
    bool keep_running;
    
    while (window.isOpen()){
      dt = clock.restart().asSeconds();
      keep_running = _handle_events(window, text_sys.font, ps_anim.is_done);
      if (!keep_running){
        window.close();
        return;
      }
      if (!window.hasFocus()){
        sf::sleep(sf::milliseconds(500)); //sleep half a second
        continue;
      }
      
      _update(dt);
      //draw
      window.clear(sf::Color(63, 72, 87));
      window.setView(game_view); //game is drawn in world space
      sf::RectangleShape world_bg(VIEW_SIZE); //default position is (0,0)
      world_bg.setFillColor(sf::Color(40, 46, 58)); //different from the clear color
      window.draw(world_bg);

      _draw(window);

      //window.setView(ui_view); //everything below is drawn in real pixels
      window.setView(window.getDefaultView());
      if (settings.show_fps){
        fps_viz.show_fps(window, {10.0f, 10.0f});
      }
      text_sys.show_text(window, "touch:"+std::to_string(touch_info.pos.x)+std::to_string(touch_info.pos.y), {10.0f, 50.0f});
      sf::CircleShape touch_c(5.f);
      touch_c.setPosition(touch_info.pos);
      window.draw(touch_c);
      text_sys.show_text(window, "level: "+ std::to_string(levelno), {10.0f, 30.0f});
      
      temp_nxt_btn.draw(window);
      temp_pre_btn.draw(window);
      temp_setting_btn.draw(window);
      
      window.display();
    }
  }
  
  void _draw(sf::RenderWindow& window){
    if (settings.particles)
       particle_system_man.draw(window);
    pda_man.draw(window);
    ps_anim.draw(window);
    if (!ps_anim.is_done) return;
    
    ball.draw(window);
    for (auto& poly: current_level.polygons){
      if (settings.shape_shadows) 
         _draw_simulated_poly_shadow(window, poly);
      window.draw(poly);
    }
    for (auto& line: current_level.lines){
      window.draw(utils::makeLine(line[0], line[1], 2.f, sf::Color::Red));
    }
    if (ball_state == MAKING){
      if (settings.aim_line){
        auto end_pos = _get_selection_arrow_end();
        window.draw(utils::makeLine(ball.start_pos, end_pos, 2.5f, sf::Color::White));
        //utils::draw_dotted_line(window, ball.start_pos, end_pos);
        utils::draw_arrow_cap(window, ball.start_pos, end_pos, 20.0f, 2.5f, sf::Color::White);
      }
      window.draw(utils::makeCircle(11.0f, ball.start_pos));
    }
    
  }


  void _update(float dt){
    if (settings.particles){
      particle_system_man.update(dt);
    }
    pda_man.update(dt);
    bool was_added = ps_anim.update(dt);
    if (was_added && settings.sound){
      audio_man->play_sound("hit");
    }
    if (!ps_anim.is_done) return;
    
    auto result = ball.update(dt, current_level.polygons, current_level.lines);
    if (result.ball_collides){
      _add_particle_system(result.collision_pos,
                           result.collision_obj_color);
      if (settings.sound) piano_np.play_next_note();//audio_man->play_sound("hit");
      auto polygon = current_level.polygons[result.collided_poly_index];
      if (polygon.is_mortal){
        pda_man.add_animation(polygon);
        current_level.remove_polygon_at(result.collided_poly_index); // remove dead polygon.
      }
    }
    if (result.ball_dies){
      _add_particle_system(result.collision_pos,
                           result.collision_obj_color);
      
      _reset_level();
      if (settings.sound) audio_man->play_sound("die");
      return;
    }
    if (ball.isdead && ball_state != MAKING) ball_state = UNBORN;
    //ball.end_pos is in world space so compare against VIEW_SIZE, not the pixel win_size
    if (!ball.isdead && (ball.end_pos.x > VIEW_SIZE.x || ball.end_pos.x < 0 ||
       ball.end_pos.y > VIEW_SIZE.y || ball.end_pos.y < 0)){
      _add_particle_system(ball.end_pos, ball.ball_shape.getFillColor());
      if (current_level.get_alive_poly_count() == 0){
        //FIXME since get_live_poly_count iterates over polygons, we can count it somewhere else where we already iterating for eg drawing.
        printf("--- Level %zu complete ---\n", levelno);
        _load_next_level();
        ps_anim.reset(&current_level); //only reset spawn animation when a level completes!
        return;
      }
      _reset_level();
      if (settings.sound) audio_man->play_sound("die");
    }
  }

  //returns false on exit events
  [[nodiscard]]
  bool _handle_events(sf::RenderWindow&window, const sf::Font& temp_font, bool handle_touch_events=true){
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        return false;
      }
      if (const auto* key_press = event->getIf<sf::Event::KeyPressed>()){
        if (key_press->code == sf::Keyboard::Key::Escape){
          return false;
        }
      }
      if (event->is<sf::Event::FocusLost>()){
        (void) window.setActive(false); //FIX MAY FAIL ignoring return value
        audio_man->stop_bg_music();
      }
      if (event->is<sf::Event::FocusGained>()){
        (void) window.setActive(true);
        if (settings.music) audio_man->start_bg_music();
      }
      //if (event->is<sf::Event::MouseButtonPressed>()){
      //  _on_screen_click_down(V2f(sf::Mouse::getPosition()));
      //}else if (event->is<sf::Event::MouseButtonReleased>()){
       // _on_screen_click_up(V2f(sf::Mouse::getPosition()), window, temp_font);
      //}
      
      if (const auto* touch = event->getIf<sf::Event::TouchBegan>()){
        V2f pos = V2f(window.mapPixelToCoords(touch->position, game_view));
        if (touch_info.active) continue; //a finger is already active do not register another
        touch_info = {pos, touch->finger, true};
        _on_screen_click_down(pos);
      }
      if (const auto* touch = event->getIf<sf::Event::TouchEnded>()){
        if (touch->finger == touch_info.fingerid) {//ensure same finger event 
          auto pos = V2f(window.mapPixelToCoords(touch->position, game_view));
          V2f pixel_pos = V2f(touch->position); //raw pixels, needed by ui buttons
          _on_screen_click_up(pos, pixel_pos, window, temp_font);
          touch_info.active = false;
        }
      }
      if (const auto* touch = event->getIf<sf::Event::TouchMoved>()){
        if (touch->finger == touch_info.fingerid){
          touch_info.pos = V2f(window.mapPixelToCoords(touch->position, game_view));
        }
      }
    }
    return true;
  }


  void _on_screen_click_down(sf::Vector2f pos){
    if (ball_state == MOVING){
      //triggers 'if' block below
      // start again if we touch while ball moving
      _reset_level();
    }
    if (ball_state == UNBORN && ps_anim.is_done){
      //touch landed in the letterbox bars (outside the world), ignore
      if (pos.x < 0 || pos.x > VIEW_SIZE.x || pos.y < 0 || pos.y > VIEW_SIZE.y){
        return;
      }
      if (_ball_spawning_inside_polygon(pos)){
        printf("cannot spawn ball inside polygon\n");
        return;//dont spawn inside
      }
      ball_state = MAKING;
      ball.start_pos = pos;
      ball.end_pos = ball.start_pos;
    }
  }
  

  //pos is in world space (game), pixel_pos is in real screen pixels (ui)
  void _on_screen_click_up(sf::Vector2f pos, sf::Vector2f pixel_pos, sf::RenderWindow& window, const sf::Font& temp_font){
    //ensure spawn animation is completed
    if (ball_state == MAKING && ps_anim.is_done){
      ball_state = MOVING;
      ball.velocity = pos - ball.start_pos;
      if (ball.velocity.length() != 0){
        ball.velocity = -1.0f * ball.velocity.normalized();
        ball.isdead = false;
      }else{
        ball_state = UNBORN;
       printf("untouched the same pos, zero vector cannot be normalized!\n");
      }
    }
    //TODO:SSSSSSSSSSSSSSSSSSSSSSSS Fix buttons later
    // 
    if (temp_nxt_btn.is_touched(pixel_pos)){
      _load_next_level();
    }
    if (temp_pre_btn.is_touched(pixel_pos)){
      _load_previous_level();
    }
    if (temp_setting_btn.is_touched(pixel_pos)){
      audio_man->bg_music.pause();
      settings_screen(window, settings, temp_font);
      _apply_updated_settings();
    }
    //TODO: SSSSSSSSSSSSSSSSSSSSSSSS 
  }
  
  
  void _reset_level(){
    ball.isdead = true;
    ball_state = UNBORN;
    _load_current_level();
    piano_np.reset_note();
  }

  void _load_current_level(void){
    current_level = gameLevels[levelno];
  }

  void _load_next_level(void){
    levelno += 1;
    if (levelno >= gameLevels.size()){
      levelno = gameLevels.size() - 1;
    }
    _reset_level();
  }
  
  void _load_previous_level(){
    if (levelno != 0) {
      levelno -= 1;
    }
    _reset_level();
  }


  //simulate shadow by drawing the shape with an offset
  // at the cost of rendering each shape twice
  void _draw_simulated_poly_shadow(sf::RenderWindow& window, ConvexPolygon& poly){
      V2f offset = V2f(5.0f, 5.0f);
      poly.move(offset);
      auto o_color = poly.getFillColor();
      poly.setFillColor(sf::Color::Black);
      window.draw(poly);
      poly.move(offset * -1.0f);
      poly.setFillColor(o_color);
  }

  bool _ball_spawning_inside_polygon(V2f spawn_point){
    for (const ConvexPolygon& polygon: current_level.polygons){
      if (polygon.contains(spawn_point)) return true;
    }
    return false;
  }

  void _apply_updated_settings(){
    //("TODO: apply new settings: eg music toggled or AA\n");
    if (settings.music) audio_man->start_bg_music();
    else audio_man->stop_bg_music();
  }
  
  sf::Vector2f _get_selection_arrow_end(){
    auto m_pos = touch_info.pos;
    auto dir =  V2f(m_pos) - ball.start_pos;
    if (dir.length() > 0) {
      dir = dir.normalized();
      dir *= -3.0f * utils::distance(ball.start_pos, m_pos);
    }
    auto end = dir + m_pos;
    return end;
  }

  void _add_particle_system(V2f pos, sf::Color color){
    particle_system_man.add_particle_system(
        ParticleSystem(
          particle_system_man.rng,
          pos, color
        )
    );
  }
  
  
};//Game

