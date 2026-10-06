#include "headers/game.hpp"
#include "headers/audio_system.hpp"


int main() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    
    sf::RenderWindow window(desktopMode, "superball");
    window.setFramerateLimit(60);

    AudioManager audio_man;
    
    Game game(window, &audio_man);
    game.mainloop(window);

    return 0;
}


