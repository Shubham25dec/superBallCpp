#include "headers/game.hpp"
#include "headers/audio_system.hpp"

int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

    sf::ContextSettings win_settings;
    win_settings.antiAliasingLevel = 8; //Trying high aa level //FIXME: temporary
    
    
    sf::RenderWindow window(desktopMode,
                        "superball",
                        sf::Style::Default,
                        sf::State::Fullscreen,
                        win_settings);
    
    window.setFramerateLimit(120);
    //FIXME: temporary 120fps, set to 60 later!

    AudioManager audio_man;
    
    Game game(window, &audio_man);
    game.mainloop(window);

    return 0;
}


