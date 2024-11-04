#include "mapLoader.h"  
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include "includes.h"
#include <mutex>
#include <map>

#pragma region forwards
void putMapInVec(sf::Texture& m_tileset, std::vector<std::vector<Map>>& maps);
void putMapInVec(sf::Texture& m_tileset, std::vector<std::vector<Map>>& newMaps, std::vector<std::vector<Map>>& oldMaps, const sf::Vector2i offset);
#pragma endregion


baseEntity grass;
const int widthHeightInTiles = 25;
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

//Map mapsAry[fullWorldDimensions][fullWorldDimensions];

std::vector<std::vector<Map>> mapsAry;
std::map<Map, int> mapX;
sf::Texture m_tileset;

int positiveDirectionOffsetSubtractor = 0;
void calcLoadNewMaps(int additionalMapDimensions, livingEntity& player, sf::View& camera, int direction, sf::RenderWindow& window)
{
	additionalDimensions += additionalMapDimensions;
	int newDimensions = fullWorldDimensions + additionalDimensions;

	float offsetRowf = (newDimensions - fullWorldDimensions) / 2.0f;
	float offsetColf = (newDimensions - fullWorldDimensions) / 2.0f;

	int offsetRow = round(offsetRowf);
	int offsetCol = round(offsetColf);

	std::vector<std::vector<Map>> newMapsAry;

	putMapInVec(m_tileset, newMapsAry, mapsAry, sf::Vector2i(offsetRow, offsetCol));
	mapsAry.clear();
	mapsAry = newMapsAry;
	if (direction < 0)
	{
		player.setPos(player.getPos() + sf::Vector2f(offsetRow * mapSize, offsetCol * mapSize)); // need to check proper direction before offsetting
	}
	else if (direction > 0)
	{
		player.setPos(player.getPos() - sf::Vector2f(offsetRow * mapSize, offsetCol * mapSize)); // need to check proper direction before offsetting
	}
	camera.setCenter(player.getPos());
	window.setView(camera);
}

//spawn map to the left or up if player is in the left or up 35% of the map
float spawnMapBoundsLower = 0.35f;

//spawn map to the right or down if player is in the right or down 65% of the map
float spawnMapBoundsUpper = 1 - spawnMapBoundsLower; 

sf::Vector2i getMapToLoad(const livingEntity& player)
{
	float x = player.sprite.getPosition().x;
	float y = player.sprite.getPosition().y;

	float tripBoundsy = (y / (mapSize));
	float tripBoundsx = (x / (mapSize));

	int indexy = (y / (mapSize));
	int indexx = (x / (mapSize));

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
	return sf::Vector2i(offsetX, offsetY);
}

void drawnNearestMaps(sf::RenderWindow& window, livingEntity& player, sf::View& camera)
{
	float x = player.sprite.getPosition().x;
	float y = player.sprite.getPosition().y;

	sf::Vector2i off = getMapToLoad(player);

	int offsetX = off.x;
	int offsetY = off.y;

	int indexx = (x / (mapSize));
	int indexy = (y / (mapSize));

	//std::cout << "current x map " << indexx << " ,  current y map " << indexy << std::endl;
	int indexOffsetX = (indexx + offsetX);
	int indexOffsetY = (indexy + offsetY);

	int mapsSize = mapsAry.size() - 1;
	if ( (indexOffsetX >= mapsSize) || (indexOffsetY >= mapsSize))
	{
		calcLoadNewMaps(4, player, camera, 1, window);
		return;
	}
	else if ( (indexOffsetY < 0) || (indexOffsetX < 0))
	{
		calcLoadNewMaps(4, player, camera, -1, window);
		return;
	}
	
	window.draw(mapsAry[indexx][indexy].tiles);
	
	//only draw if we need to
	if ((offsetX != 0) && (offsetY != 0))
	{
		window.draw(mapsAry[indexx + offsetX][indexy + offsetY].tiles);
		window.draw(mapsAry[indexx][indexy + offsetY].tiles);
		window.draw(mapsAry[indexx + offsetX][indexy].tiles);
	}
	else if (offsetX != 0)
	{
		window.draw(mapsAry[indexx + offsetX][indexy + offsetY].tiles);
		window.draw(mapsAry[indexx][indexy + offsetY].tiles);
	}
	else if (offsetY != 0)
	{
		window.draw(mapsAry[indexx + offsetX][indexy + offsetY].tiles);
		window.draw(mapsAry[indexx + offsetX][indexy].tiles);
	}
	

	drawFloraFauna(window, player, mapsAry[indexx][indexy]);

	if ((offsetX != 0) && (offsetY != 0))
	{
		drawFloraFauna(window, player, mapsAry[indexx + offsetX][indexy + offsetY]);
		drawFloraFauna(window, player, mapsAry[indexx][indexy + offsetY]);
		drawFloraFauna(window, player, mapsAry[indexx + offsetX][indexy]);
	}
	else if (offsetX != 0)
	{
		drawFloraFauna(window, player, mapsAry[indexx + offsetX][indexy + offsetY]);
		drawFloraFauna(window, player, mapsAry[indexx][indexy + offsetY]);
	}
	else if (offsetY != 0)
	{
		drawFloraFauna(window, player, mapsAry[indexx + offsetX][indexy + offsetY]);
		drawFloraFauna(window, player, mapsAry[indexx + offsetX][indexy]);
	}
	
}



std::mutex caclMapsMutex;


int grassCount = 20;
void randGrass(Map& map, sf::Vector2f& pos)
{
	sf::Vector2f grassPos(rand() % mapSize + 1, rand() % mapSize + 1);
	grass.setPos(grassPos + pos);
	map.flora.emplace_back(grass);
}

void spawnFloraAndFauna(Map& map, sf::Vector2f& pos)
{
	map.flora.reserve(grassCount);
	for (int i= 0; i < grassCount; i++)
	{
		randGrass(map, pos);
	}
}



void putMapInVec(sf::Texture& m_tileset, std::vector<std::vector<Map>>& maps)
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

			column.push_back(std::move(map));
		}
		maps.push_back(std::move(column));
	}
}
void putMapInVec(sf::Texture& m_tileset, std::vector<std::vector<Map>>& newMaps, std::vector<std::vector<Map>>& oldMaps, const sf::Vector2i offsetAry)
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

			if ((i > offsetAry.x) && (j > offsetAry.y) && (i <= oldMaps.size()) && (j <= oldMaps.size()))
			{
				oldMaps[i - 1][j - 1].tiles.setPosition(oldMaps[i - 1][j - 1].tiles.getPosition()+sf::Vector2f(offset.x*mapSize, offset.y * mapSize));
				column.push_back(oldMaps[i-1][j-1]);
			}
			else
			{
				Map map(m_tileset, offset);
				spawnFloraAndFauna(map, offset);
				column.push_back(std::move(map));
			}
		}
		newMaps.push_back(std::move(column));
	}
}
/*
//TODO
//currently this re creates the 2d grid map but only in the postive x & y direction, 
//will need to figure out how to expand all around the initially loaded map to not modify the terrain
void putMapInVec(const int& ix, std::vector<std::vector<Map>>& mapsT, sf::Vector2f extraOffset = {0,0}) //cant run this while ingame otherwise scenery will change
{
	int mapIx = mapSize * ix;
	std::vector<Map> column;
	column.reserve(fullWorldDimensions + extraMaps);
	sf::Vector2f offset;
	for (int j = 0; j < fullWorldDimensions + extraMaps; j++)
	{
		offset.x = (mapIx);
		offset.y = (mapSize * j);
		Map map(offset);

		spawnFloraAndFauna(map, offset);
		column.push_back(std::move(map));
	}
	mapsT.push_back(std::move(column));
}*/

/*
std::mutex mapMutex;
std::condition_variable cv;
bool loadExtraMaps = false;
void hotLoadMaps(livingEntity& player, sf::RenderWindow& window, sf::View& view)
{
	std::unique_lock<std::mutex> lock(mapMutex);
	while (1)
	{
		cv.wait(lock, [] {return loadExtraMaps; });
		loadExtraMaps = false;

		std::vector<std::vector<Map>> tmpMaps;
		int extra = (extraMaps + fullWorldDimensions);
		int extraOffset = mapSize * extra;
		for (int i = 0; i < extra; i++)
		{
			putMapInVec(i,tmpMaps, sf::Vector2f(extraOffset/2, extraOffset/2));
		}
		maps = tmpMaps;
		player.setPos(sf::Vector2f(extraOffset / 2, extraOffset / 2));
		calcNearestMaps(player);
		view.setCenter(sf::Vector2f(extraOffset / 2, extraOffset / 2));
		window.setView(view);
	}
}
*/

void initMaps(livingEntity & player)
{
	
	// Load the tileset texture
	if (!m_tileset.loadFromFile("resources/environment/Grass And Road Tiles/grassAndRoad.png"))
		std::cout << "failed to load" << std::endl;

	m_tileset.setSmooth(false); //try fix the black lines between sprites

	
	grass.load("resources/environment/Grass And Road Tiles/grassTuft.png");

	putMapInVec(m_tileset, mapsAry);
	
	//player.setPos(player.getPos() - sf::Vector2f(mapSize / 2, mapSize / 2));

	//extraMaps = 10;
	//caclMapsMutex.lock();

	//std::lock_guard<std::mutex> lock(mapMutex);
	//loadExtraMaps = true;
	//cv.notify_one();
	//caclMapsMutex.unlock();
}


