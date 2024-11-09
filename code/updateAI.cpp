#include "updateAI.h"
#include "utilityFunctions.h"
#include "mapLoader.h"

//called from mapLoader
void initAi()
{

}


void simulate(livingEntity& entity, Map & map)
{
	entity.setPos(sf::Vector2f(entity.getPos().x + 1, entity.getPos().y + 1));

	if (map.localMapChunks.find(hashMapChunkIntoChunks(entity.getPos().x, entity.getPos().y)) != map.localMapChunks.end()) // if simulation entity is in same chunk as other living ent
	{
		std::vector<livingEntity*>& chunkedEntities = map.localMapChunks[hashMapChunkIntoChunks(entity.getPos().x, entity.getPos().y)];
		for (int i = 0; i < chunkedEntities.size(); i++)
		{
			if (entity.sprite.getGlobalBounds().intersects(chunkedEntities[i]->sprite.getGlobalBounds()))
			{
				std::cout << entity.name << " collided with " << chunkedEntities[i]->name << std::endl;
			}
		}
	}
}

void aiUpdate()
{
	while (1)
	{
		for (auto it = hashedMaps.begin(); it != hashedMaps.end(); it++)
		{
			it->second.localMapChunks.clear();
			for (int a = 0; a < it->second.fauna.size(); a++)
			{
				addObjectToChunkMap(it->second.fauna[a], it->second);
			}

			std::vector<livingEntity>& ent = it->second.fauna;
			for (int i = 0; i < ent.size(); i++)
			{
				simulate(ent[i], it->second);
			}			
		}
		//print("ai");
		//sf::sleep(sf::seconds(0.5f));
	}
}