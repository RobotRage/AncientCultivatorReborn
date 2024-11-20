#pragma once

#include "includes.h"
#include <mutex>
#include "utilityFunctions.h"

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
    sf::Sprite sprite;
    std::vector<sf::Texture> moveAnim;     // Animation textures for movement
    std::vector<sf::Texture> defaultAnim;  // Animation textures for default state

    std::vector<int> lst;
    
    sf::Text label;

    // Variadic template constructor to load textures
    template<typename... Paths>
    void load(Paths... paths)
    {
        loadTextures(paths...); // Load textures using variadic templates
        if (!defaultAnim.empty()) {
            sprite.setTexture(defaultAnim[0]); // Set the initial texture if available
        }
    }

    void setPos(sf::Vector2f pos)
    {
        sprite.setPosition(pos);
    }
    sf::Vector2f getPos()
    {
        return sprite.getPosition();
    }

    void animAssign(const std::vector<std::string>& texturePaths, std::vector<sf::Texture>* anim)
    {
        anim->clear();
        for (const auto& path : texturePaths) {
            sf::Texture texture;
            if (texture.loadFromFile(path)) {
                anim->push_back(texture);
            }
            else {
                std::cout << "-#ERROR# failed to load texture from " << path << std::endl;
            }
        }
    }

protected:
    void loadTexture(const std::string& path)
    {
        sf::Texture texture;
        if (texture.loadFromFile(path)) {
            defaultAnim.push_back(texture);
        }
        else {
            std::cout << "-#ERROR# failed to load texture from " << path << std::endl;
        }
    }

    template<typename... Paths>
    void loadTextures(const std::string& path, Paths... rest)
    {
        loadTexture(path);       
        loadTextures(rest...);   // Recursively call for the rest
    }

    // Base case for recursion
    void loadTextures() { /* Do nothing */ }
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

    std::vector<baseEntity*> knownEntities;

    sf::Vector2i targetMove{ 0,0 };

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
        sprite.setPosition(currentPosition + change);
    }
};
