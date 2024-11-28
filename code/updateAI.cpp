#include "updateAI.h"
#include "utilityFunctions.h"
#include "mapLoader.h"
#include "globalSettings.h"
#include <utility>


//called from mapLoader
void initAi()
{

}


void inViewCheck(livingEntity& entity, Map& map, std::vector<livingEntity*>& chunkedEntities, const int& i)
{
	if (entity.viewRange >= distance(chunkedEntities[i]->getPos(), entity.getPos()) - (entity.sprite.getLocalBounds().width/2))
	{
		auto it = std::find(entity.knownEntities.begin(), entity.knownEntities.end(), chunkedEntities[i]);
		
		if (it == entity.knownEntities.end())
		{
			entity.knownEntities.push_back(chunkedEntities[i]);
		}		
	}
}


void loopChunks(livingEntity& entity, Map& map, const int & offX, const int & offY)
{
	int x = entity.getPos().x;
	int y = entity.getPos().y;
	sf::Vector2f offedPos((x ) + (float)(offX * cellSize), (y) + (float)(offY * cellSize));

	std::vector<livingEntity*>& chunkedEntities = map.localMapChunksLiving[calcChunkMap(offedPos)];
	
	for (int i = 0; i < chunkedEntities.size(); i++)
	{
		if (chunkedEntities[i]->name != entity.name)
		{
			
			if (debug)
			{
				sf::Lock lock(lineMutexDebug);

				
				sf::VertexArray line(sf::Lines, 2);

				line[0].position = entity.getPos(); // Start point
				line[0].color = sf::Color::Red;
				line[1].position = chunkedEntities[i]->getPos(); // End point
				line[1].color = sf::Color::Blue;

				//viewLineList.push_back(line);
				
				viewLineList2.push_back(line);
			}

			inViewCheck(entity, map, chunkedEntities, i);

		}
	}
}

void checkChunkForOtherEntities(livingEntity& entity, Map& map)
{
	int x = entity.getPos().x;
	int y = entity.getPos().y;
	sf::Vector2f p(x % mapSize, y % mapSize);
	//if (map.localMapChunksLiving.find(calcChunkMap(p)) != map.localMapChunksLiving.end()) // if simulation entity is in same chunk as other living ent
	//{
	
	
			loopChunks(entity, map, -1, 1);
			loopChunks(entity, map, 1, 1);
			loopChunks(entity, map, 1, -1);
			loopChunks(entity, map, -1, -1);
			loopChunks(entity, map, -1, 0);
			loopChunks(entity, map, 1, 0);
			loopChunks(entity, map, 0, -1);
			loopChunks(entity, map, 0, 1);
			loopChunks(entity, map, 0, 0);
		
		/*
		if (entity.sideOfChunk.left && entity.sideOfChunk.top)
		{
			loopChunks(entity, map, -1, 1);
		}
		else if (entity.sideOfChunk.top && entity.sideOfChunk.right)
		{
			loopChunks(entity, map, 1, 1);
		}
		else if (entity.sideOfChunk.right && entity.sideOfChunk.bottom)
		{
			loopChunks(entity, map, 1, -1);
		}
		else if (entity.sideOfChunk.bottom && entity.sideOfChunk.left)
		{
			loopChunks(entity, map, -1, -1);
		}
		else if (entity.sideOfChunk.left)
		{
			loopChunks(entity, map, -1, 0);
		}
		else if (entity.sideOfChunk.right)
		{
			loopChunks(entity, map, 1, 0);
		}
		else if (entity.sideOfChunk.bottom)
		{
			loopChunks(entity, map, 0, -1);
		}
		else if (entity.sideOfChunk.top)
		{
			loopChunks(entity, map, 0, 1);
		}
		else
		{
			loopChunks(entity, map, 0, 0);		
		}*/
	//}
}

void travel(livingEntity& entity, sf::Vector2f & entPos)
{
	float dist = distance(entPos, sf::Vector2f(entity.targetMove.x, entity.targetMove.y));

	float stepX = (entity.targetMove.x - entPos.x) / dist;
	float stepY = (entity.targetMove.y - entPos.y) / dist;

	entity.setPos(sf::Vector2f((entity.getPos().x) + stepX, (entity.getPos().y + stepY)));
}



void simulate(livingEntity& entity, Map& map)
{

	sf::Vector2f entPos = entity.getPos();

	if ((entity.targetMove.x == 0 && entity.targetMove.y == 0) ||(round(entPos.x) == round(entity.targetMove.x) && round(entPos.y) == round(entity.targetMove.y)))
	{
		entity.targetMove.x = entPos.x + entity.viewRange * cos(rand() % (int)(2 * Pi));
		entity.targetMove.y = entPos.y + entity.viewRange * sin(rand() % (int)(2 * Pi));

		entity.sprite.setRotation(toDeg(atan2(entity.targetMove.y - entPos.y, entity.targetMove.x - entPos.x))); // atan2 takes y as first param (y,x)			
	}

	travel(entity, entPos);
	checkChunkForOtherEntities(entity, map);
}


void aiUpdate()
{
	while (1)
	{
		
		for (auto it = hashedMaps.begin(); it != hashedMaps.end(); it++)
		{
			
			//recalculate map chunks
			it->second.localMapChunksLiving.clear();

			//if player pos map index is same as current looping map index
			sf::Vector2i pPos = getCurrentTileMapPos(player.getPos());
			if (it->first == std::pair<int,int>{pPos.x, pPos.y})
			{
				addObjectToChunkMap(player, it->second);
			}



			
			bool ok = 1;
			do {
				ok = 1;
				for (int a = 0; a < it->second.fauna.size(); a++)
				{
					sf::Vector2i faunaPos = getCurrentTileMapPos(it->second.fauna[a].getPos());
					if (hashedMaps.find({ faunaPos.x, faunaPos.y }) == hashedMaps.end()) { continue; } //dont let entities get added to uncreated maps
					if (faunaPos != it->second.fauna[a].mapLocation) // currentposition != saved position
					{
						std::unique_lock<std::mutex> lock(entityMutex);
						it->second.fauna[a].mapLocation = faunaPos;

						hashedMaps[{faunaPos.x, faunaPos.y}].fauna.push_back(it->second.fauna[a]);	//put saved animal into hashed map


						addObjectToChunkMap(it->second.fauna[a], hashedMaps[{faunaPos.x, faunaPos.y}]);

						it->second.fauna.erase(it->second.fauna.begin() + a); //erase from list of animals

						ok = 0;

						break;
					}
					else
					{
						addObjectToChunkMap(it->second.fauna[a], it->second);
					}
				}		
			} while (!ok);



			for (int a = 0; a < it->second.flora.size(); a++)
			{
				addObjectToChunkMap(it->second.flora[a], it->second);
			}

			std::vector<livingEntity>& ent = it->second.fauna; //only simulate for fauna
			for (int i = 0; i < ent.size(); i++)
			{
				simulate(ent[i], it->second);
			}			
		}
		sf::sleep(sf::seconds(0.01f));
	}
}