#include "mapLoader.h"  
#include "tileMapVertex.h" 
#include "utilityFunctions.h"
#include "entityObject.h"
#include "includes.h"
#include <mutex>
#include <map>
#include <unordered_map>
#include "updateAI.h"



const int widthHeightInTiles = 25;
const int tilePixelSize = 32;
const int mapSize = (tilePixelSize * widthHeightInTiles);
const int fullWorldDimensions = 1;
int additionalDimensions = 0;


void drawFloraFauna(sf::RenderWindow& window, const livingEntity& player,const Map& map)
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

int iter = 0;
void drawFloraFaunaDebug(sf::RenderWindow& window, const livingEntity& player, Map& map)
{
	for (int i = 0; i < map.flora.size(); i++)
	{
		window.draw(map.flora[i].sprite);
		map.flora[i].label.setPosition(map.flora[i].getPos());
		window.draw(map.flora[i].label);

	}
	for (int i = 0; i < map.fauna.size(); i++)
	{
		sf::Color sightCol = { 255,255,255,150 };
		window.draw(map.fauna[i].sprite);

		sf::Lock lock(lineMutexDebug);
		
		for (int j = 0; j < viewLineList2.size(); j++)
		{
			window.draw(viewLineList2[j]);
		}
		iter++;
		if (iter > 200)
		{
			iter = 0;
			viewLineList2.clear();
		}
		map.fauna[i].label.setPosition(map.fauna[i].getPos());
		window.draw(map.fauna[i].label);
		sf::CircleShape circle(map.fauna[i].viewRange);
		circle.setOrigin(circle.getRadius() , circle.getRadius() );
		circle.setPosition(map.fauna[i].getPos());
		if (map.fauna[i].knownEntities.size() > 0)
		{
			sightCol = { 255,10,10,150 };
		}
		else
		{
			sightCol = { 255,255,255,150 };
		}
		circle.setFillColor(sightCol);
		window.draw(circle);
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

		std::pair<int, int> p = { x,y };
		hashedMaps.emplace(p,map);
	}
}
const int cellSize = mapSize;
int calcChunkMap(sf::Vector2f & obj)
{
	int posx = (obj.x);
	int posy = (obj.y);
	int moddedx = posx % mapSize;
	int moddedy = posy % mapSize;

	int cellX = (int)floor((moddedx / cellSize));
	int cellY = (int)floor((moddedy / cellSize));
	return  cellX * 73856093 ^ cellY * 19349663; // Hash function for 2D coordinates
}

void addObjectToChunkMap(livingEntity& obj, Map& map) {	
	int posx = (obj.sprite.getPosition().x );
	int posy = (obj.sprite.getPosition().y );
	int moddedx = posx % mapSize;
	int moddedy = posy % mapSize;

	int cellX = (int)floor((moddedx / cellSize));
	int cellY = (int)floor((moddedy / cellSize));
	int hashKey = cellX * 73856093 ^ cellY * 19349663; // Hash function for 2D coordinates

	map.localMapChunksLiving[hashKey].push_back(&obj);

	// Calculate relative position within the chunk
	float chunkLeft = cellX * cellSize;
	float chunkRight = chunkLeft + cellSize;
	float chunkTop = cellY * cellSize;
	float chunkBottom = chunkTop + cellSize;



	// Determine which sides the object is near
	ChunkSideInfo sideInfo;
	const float threshold = cellSize/3.0f; 

	if (moddedx - chunkLeft <= threshold) sideInfo.left = true;
	if (chunkRight - moddedx <= threshold) sideInfo.right = true;
	if (moddedy - chunkTop <= threshold) sideInfo.top = true;
	if (chunkBottom - moddedy <= threshold) sideInfo.bottom = true;

	obj.sideOfChunk = sideInfo;
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

	//std::cout << "Player Map Pos-  X:" << indexx << "  Y:" << indexy << std::endl;

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

	addToMapIfNotFound(indexx + offsetX, indexy + offsetY);
	addToMapIfNotFound(indexx, indexy + offsetY);
	addToMapIfNotFound(indexx + offsetX, indexy);
	addToMapIfNotFound(indexx, indexy);

	window.draw(hashedMaps[{indexx, indexy}].tiles);

	//only draw if we need to
	if ((offsetX != 0) && (offsetY != 0))
	{
		window.draw(hashedMaps[{indexx + offsetX, indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx, indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx + offsetX, indexy}].tiles);
	}
	else if (offsetX != 0)
	{
		window.draw(hashedMaps[{indexx + offsetX, indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx, indexy + offsetY}].tiles);
	}
	else if (offsetY != 0)
	{
		window.draw(hashedMaps[{indexx + offsetX, indexy + offsetY}].tiles);
		window.draw(hashedMaps[{indexx + offsetX, indexy}].tiles);
	}

	if (debug)
	{
		drawFloraFaunaDebug(window, player, hashedMaps[{indexx, indexy}]);

		if ((offsetX != 0) && (offsetY != 0))
		{
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx, indexy + offsetY}]);
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx + offsetX, indexy}]);
		}
		else if (offsetX != 0)
		{
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx, indexy + offsetY}]);
		}
		else if (offsetY != 0)
		{
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFaunaDebug(window, player, hashedMaps[{indexx + offsetX, indexy}]);
		}
	}
	else
	{
		drawFloraFauna(window, player, hashedMaps[{indexx, indexy}]);

		if ((offsetX != 0) && (offsetY != 0))
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFauna(window, player, hashedMaps[{indexx, indexy + offsetY}]);
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}]);
		}
		else if (offsetX != 0)
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFauna(window, player, hashedMaps[{indexx, indexy + offsetY}]);
		}
		else if (offsetY != 0)
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}]);
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}]);
		}
	}
}

void randFauna(Map& map, sf::Vector2f& pos, livingEntity& animal)
{
	sf::Vector2f animalpPos(rand() % mapSize + 1, rand() % mapSize + 1);
	//sf::Vector2f animalpPos(25, 25);
	animal.setPos(animalpPos + pos);
	map.fauna.push_back(animal);
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
	ent.label.setString(ent.name);
}

livingEntity tgrass;
livingEntity tsheep;

int entityCounter = 0;
void spawnFloraAndFauna(Map& map, sf::Vector2f& pos)
{
	int grassCount = 10;
	int sheepCount = 2;
	map.flora.reserve(grassCount);
	for (int i= 0; i < grassCount; i++)
	{		
		
		tgrass.name = "grass_" + std::to_string(entityCounter);
		tgrass.sprite.setOrigin(tgrass.sprite.getLocalBounds().width / 2, tgrass.sprite.getLocalBounds().height / 2);
		randFlora(map, pos, tgrass);
		setLabelFont(tgrass);
		entityCounter++;
	}
	for (int i = 0; i < sheepCount; i++)
	{			
		tsheep.name = "sheep_" + std::to_string(entityCounter);
		tsheep.sprite.setOrigin(tsheep.sprite.getLocalBounds().width / 2, tsheep.sprite.getLocalBounds().height / 2);
		randFauna(map, pos, tsheep);
		setLabelFont(tsheep);
		entityCounter++;
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


