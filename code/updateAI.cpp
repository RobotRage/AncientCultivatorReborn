#include "updateAI.h"
#include "utilityFunctions.h"
#include "mapLoader.h"

//called from mapLoader
void initAi()
{

}

void inViewCheck(livingEntity& entity, Map& map, std::vector<livingEntity*>& chunkedEntities, const int& i)
{
	if (entity.viewRange >= distanceCheck(chunkedEntities[i]->getPos(), entity.getPos()))
	{
		auto it = std::find(entity.knownEntities.begin(), entity.knownEntities.end(), chunkedEntities[i]);

		//add size offset for sprite size
		if (it == entity.knownEntities.end())
		{
			entity.knownEntities.push_back(chunkedEntities[i]);
		}		
	}
}

void checkChunkForOtherEntities(livingEntity& entity, Map& map)
{
	if (map.localMapChunksLiving.find(calcChunkMap(entity)) != map.localMapChunksLiving.end()) // if simulation entity is in same chunk as other living ent
	{
		if (entity.sideOfChunk.left && entity.sideOfChunk.top)
		{

		}
		else if (entity.sideOfChunk.top && entity.sideOfChunk.right)
		{

		}
		else if (entity.sideOfChunk.right && entity.sideOfChunk.bottom)
		{

		}
		else if (entity.sideOfChunk.bottom && entity.sideOfChunk.left)
		{

		}
		else if (entity.sideOfChunk.left)
		{

		}
		else if (entity.sideOfChunk.right)
		{

		}
		else if (entity.sideOfChunk.bottom)
		{

		}
		else if (entity.sideOfChunk.top)
		{

		}
		else
		{
			std::vector<livingEntity*>& chunkedEntities = map.localMapChunksLiving[calcChunkMap(entity)];
			for (int i = 0; i < chunkedEntities.size(); i++)
			{
				if (chunkedEntities[i]->getPos() == entity.getPos()) { continue; }
				inViewCheck(entity, map, chunkedEntities, i);

			}
		}
	}
}

void simulate(livingEntity& entity, Map& map)
{
	entity.setPos(sf::Vector2f(entity.getPos().x + rand() % 40 - 20, entity.getPos().y + rand() % 40 - 20));
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