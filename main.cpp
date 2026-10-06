#include "headers/pch.hpp"
#include "headers/convex_polygon.hpp"
#include "headers/ball.hpp"
#include "headers/game.hpp"
#include "headers/audio_system.hpp"



int main() {
    
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::ContextSettings window_settings;
    window_settings.antiAliasingLevel = 0;
    
    sf::RenderWindow window(
        desktopMode, 
        "Mobile Bouncing Ball", 
        sf::Style::Default,
        sf::State::Fullscreen,
        window_settings
    );
    window.setFramerateLimit(60);

    // 3. Dynamically GET the hardware width and height
    sf::Vector2u windowSize = window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);

    std::cout << "Hardware Resolution Detected: " << windowWidth << "x" << windowHeight << std::endl;


    AudioManager audio_man;
    
    Game game(window, &audio_man);
    game.mainloop(window);

    return 0;
}


