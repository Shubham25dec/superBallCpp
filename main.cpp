#include "headers/game.hpp"
#include "headers/audio_system.hpp"
#include "headers/asset_manager.hpp"


int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    
    sf::RenderWindow window(desktopMode, "superball");
    
    window.setFramerateLimit(60);

    AudioManager audio_man;
    PianoNotePlayer piano_np(&audio_man, "resources/sounds/");
    AssetManager asset_man("resources/font.ttf");

    audio_man.load_bg_music("resources/sounds/bg_music.ogg");
    audio_man.load_sound("resources/sounds/die.wav", "die");
    audio_man.load_sound("resources/sounds/spawn.wav", "spawn");

    asset_man.load_texture("resources/textures/start-button.png", "start_button");
    
    
    Game game(window, &audio_man, &asset_man, &piano_np);
    game.mainloop(window);

    return 0;
}


