#include "includes.h"
#include "gameLoop.h"
#include "entityObject.h"
#include "mapLoader.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(screenx, screeny), "Ancient Cultivator Reborn");

    sf::View camera = window.getDefaultView();

    livingEntity player;
    player.load("resources/player/playerWalk0.png");
    //sf::Thread hotLoad;
    init(camera, window, player);


    

    // Clocks for independent updates
    sf::Clock updateClock;   
    sf::Clock animateClock; 

    const float updateInterval = 1.0f / 200.0f;  // 1/x = x updates per second
    const float animateInterval = 1.0f / 10.0f; 

    
    // Main game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (updateClock.getElapsedTime().asSeconds() >= updateInterval)
        {
            update(camera, window, player);
            updateClock.restart();
        }

        // Check if it's time to update animations
        if (animateClock.getElapsedTime().asSeconds() >= animateInterval)
        {
            animate(player);
            animateClock.restart();   
        }

        window.clear();

        draw(window, player);

        window.display();
    }

    return 0;
}
