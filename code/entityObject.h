#pragma once

#include "includes.h"
#include "globalSettings.h"
#include "utilityFunctions.h"
#include "list"
struct ChunkSideInfo {
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
};

class baseEntity
{
public:
    sf::Vector2i mapLocation;

    std::string name;
    int animFrame = 0;
    sf::Sprite * sprite;

    std::list<int> lst;
    
    sf::Text label;

    // Variadic template constructor to load textures


    void setPos(sf::Vector2f pos)
    {
        sprite->setPosition(pos);
    }

  
};

class nonLivingEntity : public baseEntity
{

};


class livingEntity : public baseEntity
{
public:

    //stats
    int level = 1;
    double health = 100;
    double speed = 4;
    int perceptionStat = 1;
    float viewRange = 100.0f * perceptionStat;
    float interactRange = 10.0f;
    sf::Vector2f travelCounter{ 0,0 };

    std::list<baseEntity*> knownEntities;

    sf::Vector2i targetMove{ 0 ,0};

    //states
    bool alive = true;
    bool entityMoving = false;
    int running = 1; // Multiplier for running speed

    // Variadic template constructor to initialize baseEntity
    template<typename... Paths>
    livingEntity(Paths... paths)
        : baseEntity(paths...) // Forward paths to the base constructor
    {
    }

    // Method to get the movement speed, modified by the "running" multiplier
    double getSpeed() const
    {
        return speed * running;
    }

    // Moves the entity by a given vector change
    void moveEntity(const sf::Vector2f& currentPosition, const sf::Vector2f& change)
    {
        travelCounter += change;
        sprite->setPosition(currentPosition + change);
    }
};
