#include "includes.h"
#include "gameLoop.h"
#include "entityObject.h"
#include "mapLoader.h"
#include "globalSettings.h"

int main()
{

   
    sf::RenderWindow window(sf::VideoMode(screenx, screeny), "Ancient Cultivator Reborn");

    sf::View camera = window.getDefaultView();

    playerLoad();
    //sf::Thread hotLoad;
    init(camera, window, player);

    srand(time(0));

    // Clocks for independent updates
    sf::Clock updateClock;   
    sf::Clock animateClock; 

    const float updateInterval = UPDATE_RATE;  // 1/x = x updates per second
    const float animateInterval = (UPDATE_RATE) * 10.0f;

    
    // Main game loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float mainUpdateLastFrame = updateClock.getElapsedTime().asSeconds();
        if (mainUpdateLastFrame >= updateInterval)
        {
            update(camera, window, player, mainUpdateLastFrame);
            updateClock.restart();
        }

        // Check if it's time to update animations
        if (animateClock.getElapsedTime().asSeconds() >= animateInterval)
        {
            animate(player);
            animateClock.restart();   
        }

        window.clear();

        draw(window, player, camera);

        window.display();
    }

    return 0;
}
