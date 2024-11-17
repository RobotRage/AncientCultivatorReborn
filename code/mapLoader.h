#pragma once
#include <vector>
#include "includes.h"
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include <unordered_map>
#include <utility>
extern const int widthHeightInTiles;
extern const int tilePixelSize;
extern const int mapSize;
extern const int fullWorldDimensions;

//seperate maps sections into chunks to be loaded in and out

class Map
{
public:

    TileMap tiles;
    std::vector<livingEntity> flora;
    std::vector<livingEntity> fauna;
    std::unordered_map<int, std::vector<livingEntity*>> localMapChunksLiving;
    std::unordered_map<int, std::vector<baseEntity*>> localMapChunksItems;

    Map() 
    {
    };
    Map(sf::Texture tileSet, sf::Vector2f& offset)
    {
        //std::vector<std::string> loadedMap;
        //loadedMap.reserve(19200);
       // std::string strList2 = readFile(file);
        //for (int i = 0; i < 19200; i++)
        //{
            //loadedMap.push_back("26");
       // }

        int tile = 26;
        if (!tiles.load(tileSet, sf::Vector2u(tilePixelSize, tilePixelSize), widthHeightInTiles,
            widthHeightInTiles, sf::Vector2f((float)+offset.x, (float)+offset.y), tile))
        {
            print("failed to load map resource");
        }
    }
};



// Custom hash function for a pair of integers
struct KeyHash {
    std::size_t operator()(const std::pair<int, int>& key) const {
        const std::size_t prime = 31;  // A prime number
        return std::hash<int>()(key.first) + prime * std::hash<int>()(key.second);
    }
};

// Custom equality function for comparing two pairs of integers
struct KeyEqual {
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

// localMapChunks only operates on hashedMaps


extern std::unordered_map<std::pair<int, int>, Map, KeyHash, KeyEqual> hashedMaps;
int calcChunkMap(sf::Vector2f & obj);
void addObjectToChunkMap(livingEntity& obj, Map& map);
void spawnFloraAndFauna(Map& map, sf::Vector2f& pos);
void drawnNearestMaps(sf::RenderWindow& window, livingEntity& player, sf::View& camera);
void drawFloraFauna(sf::RenderWindow& window, const livingEntity& player,const Map& map);
void drawFloraFaunaDebug(sf::RenderWindow& window, const livingEntity& player, Map& map);
void initMaps(livingEntity& player);

//void hotLoadMaps(livingEntity& player, sf::RenderWindow& window, sf::View& view);