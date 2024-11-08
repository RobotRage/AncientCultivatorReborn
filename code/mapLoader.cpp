#include "mapLoader.h"  
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include "includes.h"
#include <mutex>
#include <map>
#include <unordered_map>




const int widthHeightInTiles = 85;
const int tilePixelSize = 32;
const int mapSize = (tilePixelSize * widthHeightInTiles);
const int fullWorldDimensions = 4;
int additionalDimensions = 0;

void drawFloraFauna(sf::RenderWindow& window, const livingEntity& player, const Map& map)
{
	for (int i = 0; i < map.flora.size(); i++)
	{
		window.draw(map.flora[i].sprite);
	}
	for (int i = 0; i < map.fauna.size(); i++)
	{
		window.draw(map.fauna[i].sprite);
	}
}

sf::Texture m_tileset;

std::unordered_map<std::pair<int, int>, Map, KeyHash, KeyEqual> hashedMaps(100000);
void addToMapIfNotFound(int x, int y)
{
	if (hashedMaps.find({ x, y }) == hashedMaps.end()) {
		sf::Vector2f vecOff{ (float)x * mapSize, (float)y * mapSize };
		Map map(m_tileset, vecOff);
		spawnFloraAndFauna(map, vecOff);
		hashedMaps[{x, y}] = map;
	}
}

void drawnNearestMaps(sf::RenderWindow& window, livingEntity& player, sf::View& camera)
{
	float spawnMapBoundsLower = 0.35f;

	//spawn map to the right or down if player is in the right or down 65% of the map
	float spawnMapBoundsUpper = 1 - spawnMapBoundsLower;

	float x = player.sprite.getPosition().x;
	float y = player.sprite.getPosition().y;

	float tripBoundsy = (y / (mapSize));
	float tripBoundsx = (x / (mapSize));

	int indexy = static_cast<int>(std::floor(y / mapSize));
	int indexx = static_cast<int>(std::floor(x / mapSize));

	std::cout << "Player Map Pos-  X:" << indexx << "  Y:" << indexy << std::endl;

	int offsetX = 0;
	int offsetY = 0;

	if (tripBoundsx - (float)indexx >= spawnMapBoundsUpper)
	{
		offsetX += 1;
	}
	else if (tripBoundsx - (float)indexx <= spawnMapBoundsLower)
	{
		offsetX -= 1;
	}
	if (tripBoundsy - (float)indexy >= spawnMapBoundsUpper)
	{
		offsetY += 1;
	}
	else if (tripBoundsy - (float)indexy <= spawnMapBoundsLower)
	{
		offsetY -= 1;
	}

	addToMapIfNotFound( indexx + offsetX, indexy + offsetY);
	addToMapIfNotFound( indexx, indexy + offsetY);
	addToMapIfNotFound( indexx + offsetX, indexy);
	addToMapIfNotFound(indexx, indexy);

	window.draw(hashedMaps[{indexx, indexy}].tiles);

	//only draw if we need to
	if ((offsetX != 0) && (offsetY != 0))
	{
		window.draw(hashedMaps[{indexx + offsetX, indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx,indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx + offsetX, indexy}].tiles);
	}
	else if (offsetX != 0)
	{
		window.draw(hashedMaps[{indexx + offsetX,indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx, indexy + offsetY}].tiles);
	}
	else if (offsetY != 0)
	{
		window.draw(hashedMaps[{indexx + offsetX,indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx + offsetX, indexy}].tiles);
	}


	drawFloraFauna(window, player, hashedMaps[{indexx,indexy}]);

	if ((offsetX != 0) && (offsetY != 0))
	{
		drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
		drawFloraFauna(window, player, hashedMaps[{indexx, indexy + offsetY}]);
		drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}]);
	}
	else if (offsetX != 0)
	{
		drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
		drawFloraFauna(window, player, hashedMaps[{indexx,indexy + offsetY}]);
	}
	else if (offsetY != 0)
	{
		drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
		drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}]);
	}
}



void randFauna(Map& map, sf::Vector2f& pos, livingEntity animal)
{
	sf::Vector2f animalpPos(rand() % mapSize + 1, rand() % mapSize + 1);
	animal.setPos(animalpPos + pos);
	map.fauna.push_back(animal);
}
void randFlora(Map& map, sf::Vector2f& pos, livingEntity plant)
{
	sf::Vector2f plantPos(rand() % mapSize + 1, rand() % mapSize + 1);
	plant.setPos(plantPos + pos);
	map.flora.emplace_back(plant);
}

baseEntity grass;
livingEntity sheep;
void spawnFloraAndFauna(Map& map, sf::Vector2f& pos)
{
	int grassCount = 20;
	int sheepCount = 5;
	map.flora.reserve(grassCount);
	for (int i= 0; i < grassCount; i++)
	{
		randFlora(map, pos, grass);
	}
	for (int i = 0; i < sheepCount; i++)
	{
		randFauna(map, pos, sheep);
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
			hashedMaps[{i, j}] = map;
		}
	}
}

void initMaps(livingEntity & player)
{
	if (!m_tileset.loadFromFile("resources/environment/Grass And Road Tiles/grassAndRoad.png"))
		std::cout << "failed to load" << std::endl;

	m_tileset.setSmooth(false); //try fix the black lines between sprites

	
	grass.load("resources/environment/Grass And Road Tiles/grassTuft.png");

	sheep.load("resources/animals/tile000.png");
	sheep.sprite.setScale(1.5f, 1.5f);
	putMapInVec(m_tileset);
}


