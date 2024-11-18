#include "updateAI.h"
#include "utilityFunctions.h"
#include "mapLoader.h"
#include "globalSettings.h"
//called from mapLoader
void initAi()
{

}


void inViewCheck(livingEntity& entity, Map& map, std::vector<livingEntity*>& chunkedEntities, const int& i)
{
	if (entity.viewRange >= distance(chunkedEntities[i]->getPos(), entity.getPos()))
	{
		auto it = std::find(entity.knownEntities.begin(), entity.knownEntities.end(), chunkedEntities[i]);

		if (debug)
		{
			std::cout << entity.name << " pos: " << entity.getPos().x << "," << entity.getPos().y << "  " << chunkedEntities[i]->name << " targetPos: " << chunkedEntities[i]->getPos().x << "," << chunkedEntities[i]->getPos().y << std::endl;
		}
		
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

				//viewLineList.clear();
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

void travel()
{

}

void simulate(livingEntity& entity, Map& map)
{
	//entity.setPos(sf::Vector2f(entity.getPos().x + rand() % 40 - 20, entity.getPos().y + rand() % 40 - 20));
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
			addObjectToChunkMap(player, it->second);
			for (int a = 0; a < it->second.fauna.size(); a++)
			{
				addObjectToChunkMap(it->second.fauna[a], it->second);
			}

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
		//print("ai");
		sf::sleep(sf::seconds(0.5f));
	}
}