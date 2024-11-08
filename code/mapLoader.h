#pragma once
#include <vector>
#include "includes.h"
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"

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

    Map() 
    {
        flora.resize(50);
        fauna.resize(50);
    };
    Map(sf::Texture tileSet, sf::Vector2f& offset)
    {
        flora.resize(50);
        fauna.resize(50);
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

void spawnFloraAndFauna(Map& map, sf::Vector2f& pos);
void drawnNearestMaps(sf::RenderWindow& window, livingEntity& player, sf::View& camera);
void drawFloraFauna(sf::RenderWindow& window, const livingEntity& player, const Map& map);
void initMaps(livingEntity& player);

//void hotLoadMaps(livingEntity& player, sf::RenderWindow& window, sf::View& view);