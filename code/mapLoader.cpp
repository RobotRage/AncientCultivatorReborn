#include "mapLoader.h"  
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include "includes.h"
#include <mutex>
#include <map>
#include <unordered_map>
#include "updateAI.h"


const int widthHeightInTiles = 100;
const int tilePixelSize = 32;
const int mapSize = (tilePixelSize * widthHeightInTiles);
const int fullWorldDimensions = 5;
int additionalDimensions = 0;

const int cellSize = tilePixelSize * 5; // chunks



sf::Texture m_tileset;

//map squares of tiles
std::unordered_map<std::pair<int, int>, Map, KeyHash, KeyEqual> hashedMaps(100000);
void addToMapIfNotFound(int x, int y)
{
	std::lock_guard<std::mutex> lock(mapLoaderMutex);
	if (hashedMaps.find({ x, y }) == hashedMaps.end()) {
		sf::Vector2f vecOff{ (float)x * mapSize, (float)y * mapSize };
		Map map(m_tileset, vecOff);
		spawnFloraAndFauna(map, vecOff);

		std::pair<int, int> p = { x,y };
		
		hashedMaps.emplace(p,map);
	}
}


int calcChunkMap(sf::Vector2f obj)
{
	int posx = (obj.x);
	int posy = (obj.y);
	//int moddedx = posx % mapSize;
	//int moddedy = posy % mapSize;

	int cellX = (int)floor((posx / cellSize));
	int cellY = (int)floor((posy / cellSize));
	return  cellX * 73856093 ^ cellY * 19349663; // Hash function for 2D coordinates
}

void addObjectToChunkMap(livingEntity& obj, Map& map) {	
	int posx = (obj.sprite.getPosition().x );
	int posy = (obj.sprite.getPosition().y );
	//int moddedx = posx % mapSize;
	//int moddedy = posy % mapSize;

	int cellX = (int)floor((posx / cellSize));
	int cellY = (int)floor((posy / cellSize));
	int hashKey = cellX * 73856093 ^ cellY * 19349663; // Hash function for 2D coordinates

	map.localMapChunksLiving[hashKey].push_back(&obj);

}

sf::Vector2i getCurrentTileMapPos(sf::Vector2f pos)
{
	float x = pos.x;
	float y = pos.y;

	return sf::Vector2i(static_cast<int>(std::floor(x / mapSize)), static_cast<int>(std::floor(y / mapSize)));
}


void randFauna(Map& map, sf::Vector2f& pos, livingEntity& animal)
{
	sf::Vector2f animalpPos(rand() % mapSize + 1, rand() % mapSize + 1);
	//sf::Vector2f animalpPos(25, 25);
	animal.setPos(animalpPos + pos);
	map.fauna.emplace_back(animal);
	addObjectToChunkMap(animal, map);
}
void randFlora(Map& map, sf::Vector2f& pos, livingEntity& plant)
{
	sf::Vector2f plantPos(rand() % mapSize + 1, rand() % mapSize + 1);
	//sf::Vector2f plantPos(1, 1);
	plant.setPos(plantPos + pos);
	map.flora.emplace_back(plant);
	addObjectToChunkMap(plant, map);
}

void setLabelFont(livingEntity& ent)
{
	ent.label.setFont(font);
	ent.label.setPosition(ent.getPos());
	std::string str = ent.name;
	ent.label.setString(str);
}

livingEntity tgrass;
livingEntity tsheep;

void recalcEnt(Map& map, sf::Vector2f& mapPos, livingEntity & ent, sf::Vector2f newPos)
{
	ent.mapLocation = getCurrentTileMapPos(sf::Vector2f(mapPos.x, mapPos.y));
	ent.sprite.setOrigin(ent.sprite.getLocalBounds().width / 2, ent.sprite.getLocalBounds().height / 2);
	ent.setPos(newPos);
	setLabelFont(ent);
}

int entityCounter = 0;
void spawnFloraAndFauna(Map& map, sf::Vector2f& pos)
{
	int grassCount = 500;
	int sheepCount = 100;
	map.flora.reserve(grassCount);
	for (int i= 0; i < grassCount; i++)
	{		
		tgrass.mapLocation = getCurrentTileMapPos(sf::Vector2f(pos.x, pos.y));
		tgrass.name = "grass_" + std::to_string(entityCounter);
		tgrass.sprite.setOrigin(tgrass.sprite.getLocalBounds().width / 2, tgrass.sprite.getLocalBounds().height / 2);
		randFlora(map, pos, tgrass);
		setLabelFont(tgrass);
		entityCounter++;
		sleep(0.01);
	}
	for (int i = 0; i < sheepCount; i++)
	{			
		tsheep.mapLocation = getCurrentTileMapPos(sf::Vector2f(pos.x, pos.y));
		tsheep.name = "sheep_" + std::to_string(entityCounter);
		tsheep.sprite.setOrigin(tsheep.sprite.getLocalBounds().width / 2, tsheep.sprite.getLocalBounds().height / 2);
		randFauna(map, pos, tsheep);
		setLabelFont(tsheep);
		entityCounter++;
		sleep(0.01);
	}
}


void putMapInVec(sf::Texture& m_tileset)
{
	int worldDimensionsTotal = fullWorldDimensions + additionalDimensions;

	for (int i = 0; i < worldDimensionsTotal; i++)
	{
		int mapIx = mapSize * i;
		std::vector<Map> column;
		column.reserve(worldDimensionsTotal);
		sf::Vector2f offset;
		for (int j = 0; j < worldDimensionsTotal; j++)
		{		
			offset.x = (mapIx);
			offset.y = (mapSize * j);
			Map map(m_tileset,offset);
			spawnFloraAndFauna(map, offset);
			std::lock_guard<std::mutex> lock(mapLoaderMutex);
			sleep(0.1);
			hashedMaps[{i, j}] = map;
			sleep(0.1);
		}
	}
}
std::mutex mapLoaderMutex;
void initMaps()
{
	if (!m_tileset.loadFromFile("resources/environment/Grass And Road Tiles/grassAndRoad.png"))
	{
		std::cout << "failed to load" << std::endl;
	}
		

	m_tileset.setSmooth(false); //try fix the black lines between sprites

	tgrass.load("resources/environment/Grass And Road Tiles/grassTuft.png");	
	tsheep.load("resources/animals/tile000.png");
	tsheep.sprite.setScale(2, 2);
	

	putMapInVec(m_tileset);

	initAi();
}


