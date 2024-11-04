#include "mapLoader.h"  
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include "includes.h"
#include <mutex>
#include <map>




baseEntity grass;
const int widthHeightInTiles = 60;
const int tilePixelSize = 32;
const int mapSize = (tilePixelSize * widthHeightInTiles);
int fullWorldDimensions = 50;

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

Map mapsAry[200][200];

std::map<Map, int> mapX;

std::vector<std::vector<Map>> maps;
std::vector<Map> mapToDraw;

void drawnNearestMaps(sf::RenderWindow& window, const livingEntity& player)
{
	int x = player.sprite.getPosition().x;
	int y = player.sprite.getPosition().y;

	int indexx = (y / (mapSize));
	int indexy = (x / (mapSize));

	window.draw(mapsAry[indexy][indexx].tiles);
	window.draw(mapsAry[indexy - 1][indexx].tiles);
	window.draw(mapsAry[indexy][indexx - 1].tiles);
	window.draw(mapsAry[indexy + 1][indexx].tiles);
	window.draw(mapsAry[indexy][indexx + 1].tiles);

	window.draw(mapsAry[indexy - 1][indexx + 1].tiles);
	window.draw(mapsAry[indexy + 1][indexx + 1].tiles);
	window.draw(mapsAry[indexy + 1][indexx - 1].tiles);
	window.draw(mapsAry[indexy - 1][indexx - 1].tiles);

	drawFloraFauna(window, player, mapsAry[indexy][indexx]);
	drawFloraFauna(window, player, mapsAry[indexy - 1][indexx]);
	drawFloraFauna(window, player, mapsAry[indexy][indexx-1]);
	drawFloraFauna(window, player, mapsAry[indexy+1][indexx]);
	drawFloraFauna(window, player, mapsAry[indexy][indexx+1]);

	drawFloraFauna(window, player, mapsAry[indexy-1][indexx+1]);
	drawFloraFauna(window, player, mapsAry[indexy + 1][indexx+1]);
	drawFloraFauna(window, player, mapsAry[indexy+1][indexx - 1]);
	drawFloraFauna(window, player, mapsAry[indexy + 1][indexx]);
	drawFloraFauna(window, player, mapsAry[indexy-1][indexx - 1]);
}

std::mutex caclMapsMutex;
int extraMaps = 0;
void calcNearestMaps(const livingEntity& player)
{
	/*
	caclMapsMutex.lock();
	int x = player.sprite.getPosition().x;
	int y = player.sprite.getPosition().y;
	if (x == 0) { x = 1; } if (y == 0) { y = 1; }

	int indexx = (y / (mapSize + extraMaps));
	int indexy = (x / (mapSize + extraMaps));

	mapToDraw.clear();

	mapToDraw.push_back(maps[indexy][indexx]); 
	mapToDraw.push_back(maps[indexy-1][indexx]);
	mapToDraw.push_back(maps[indexy][indexx-1]);
	mapToDraw.push_back(maps[indexy+1][indexx]);
	mapToDraw.push_back(maps[indexy][indexx+1]);

	mapToDraw.push_back(maps[indexy-1][indexx+1]);
	mapToDraw.push_back(maps[indexy+1][indexx+1]);
	mapToDraw.push_back(maps[indexy+1][indexx-1]);
	mapToDraw.push_back(maps[indexy-1][indexx-1]);
	caclMapsMutex.unlock();
	*/
}

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



void putMapInVec(const int& ix, sf::Texture& m_tileset)
{
	int mapIx = mapSize * ix;
	//std::vector<Map> column;
	//column.reserve(fullWorldDimensions + extraMaps);
	sf::Vector2f offset;
	int fullExtra = fullWorldDimensions + extraMaps;
	for (int j = 0; j < fullExtra; j++)
	{		
		offset.x = (mapIx);
		offset.y = (mapSize * j);
		Map map(m_tileset,offset);
		spawnFloraAndFauna(map, offset);

		mapsAry[ix][j] = std::move(map);
		//column.push_back(std::move(map));
	}
	//maps.push_back(std::move(column));
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
	sf::Texture m_tileset;
	// Load the tileset texture
	if (!m_tileset.loadFromFile("resources/environment/Grass And Road Tiles/grassAndRoad.png"))
		std::cout << "failed to load" << std::endl;

	m_tileset.setSmooth(false); //try fix the black lines between sprites

	
	grass.load("resources/environment/Grass And Road Tiles/grassTuft.png");
	for (int i = 0; i < fullWorldDimensions; i++)
	{
		putMapInVec(i, m_tileset);
	}
	//player.setPos(player.getPos() - sf::Vector2f(mapSize / 2, mapSize / 2));

	//extraMaps = 10;
	//caclMapsMutex.lock();

	//std::lock_guard<std::mutex> lock(mapMutex);
	//loadExtraMaps = true;
	//cv.notify_one();
	//caclMapsMutex.unlock();
}


