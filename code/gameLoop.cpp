#include "gameLoop.h"
#include "entityObject.h"
#include "mapLoader.h"
#include "tileMapVertex.h"
#include "mapLoader.h"
#include "includes.h"
#include "updateAI.h"
#include "entityObject.h"
#include "utilityFunctions.h"
#include <time.h>      

#pragma region forwardDeclarations
bool userInput(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer);
void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer);
#pragma endregion



//main thread (ai thread in updateAI.cpp)
void update(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer)
{
	player.entityMoving = userInput(camera, window, player, lastFrameTimer);
	if (player.entityMoving) { camMoveCheck(camera, window,player, lastFrameTimer); } // only check camera bounds if player is inputting movement
}

const float multiplierForMovement = 25;
// move the camera in the direction the player is moving if they are outside a certain distance of the center of the screen
void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer)
{

	sf::Vector2f centerCam = camera.getCenter();
	sf::Vector2f playerPos = player.sprite.getPosition();

	float dist = distance(centerCam, playerPos);
	if (dist > playerDistFromCenterCamToTriggerMoveCam)
	{
		sf::Vector2f unitVec = roundVectorToWhole(getUnitVector(centerCam, playerPos));

		//scale unit vector with move speed to move camera with bigger steps
		unitVec.x = unitVec.x * player.getSpeed();
		unitVec.y = unitVec.y * player.getSpeed();

		camera.move(unitVec * (lastFrameTimer * multiplierForMovement));
		window.setView(camera);
	}
}


// WASD & Arrow keys to move
// L/Rshift to run
// 0 and 9 to zoom cam
bool userInput(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer)
{
#pragma region 8D movement input

	bool A = false; bool W = false; bool D = false; bool S = false;

	sf::Vector2f pos = player.sprite.getPosition();
	sf::Vector2f change(0, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
	{
		player.running = 3;
	}
	else
	{
		player.running = 1;
	}

	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		change.x = change.x - 1;
		player.moveEntity(pos, change * (float)player.getSpeed() * lastFrameTimer * multiplierForMovement);
		player.sprite.setRotation(270);
		A = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		change.x = change.x + 1;
		player.moveEntity(pos, change * (float)player.getSpeed() * lastFrameTimer * multiplierForMovement);
		player.sprite.setRotation(90);
		D = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		change.y = change.y + 1;
		player.moveEntity(pos, change * (float)player.getSpeed() * lastFrameTimer * multiplierForMovement);
		player.sprite.setRotation(180);
		S = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		change.y = change.y - 1;
		player.moveEntity(pos, change * (float)player.getSpeed() * lastFrameTimer * multiplierForMovement);
		player.sprite.setRotation(0);
		W = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
	{
		debug = !debug;
		sleep(0.1f);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9))
	{
		
		sf::Vector2f siz = camera.getSize();
		siz.x = round(siz.x * 0.5);
		siz.y = round(siz.y * 0.5);
		camera.setSize(siz);
		window.setView(camera);
		sleep(0.1f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
	{
		sf::Vector2f siz = camera.getSize();
		siz.x = round(siz.x * 2);
		siz.y = round(siz.y * 2);
		camera.setSize(siz);
		window.setView(camera);	
		sleep(0.1f);
	}

	if (A && W) { player.sprite.setRotation(315); }
	if (A && S) { player.sprite.setRotation(225); }
	if (S && D) { player.sprite.setRotation(135); }
	if (D && W) { player.sprite.setRotation(35); }

#pragma endregion
	if (A || D || S || W) { return true; }
	else { return false; }
}

bool inView(sf::View& camera, livingEntity * ent)
{
	const sf::Vector2f posCam = camera.getCenter();
	const sf::Vector2f posEnt = ent->getPos();

	const sf::Vector2f sizCam = camera.getSize();
	const sf::Vector2f sizEnt = ent->sprite.getScale();

	if (  ((posCam.x+abs((int)sizCam.x/2)) > (posEnt.x-abs((int)sizEnt.x/2))) && ((posCam.x - abs((int)sizCam.x / 2)) < (posEnt.x + abs((int)sizEnt.x / 2)))  )
	{
		if (((posCam.y + abs((int)sizCam.y / 2)) > (posEnt.y - abs((int)sizEnt.y / 2))) && ((posCam.y - abs((int)sizCam.y / 2)) < (posEnt.y + abs((int)sizEnt.y / 2))))
		{
			return true;
		}
	}
	return false;
}

std::mutex entityMutex;
void drawFloraFauna(sf::RenderWindow& window, const livingEntity& player, Map& map, sf::View& camera)
{
	for (int i = 0; i < map.flora.size(); i++)
	{
		livingEntity * ent = &map.flora[i];
		if (inView(camera, ent))
		{
			window.draw(ent->sprite);
		}	
	}

	std::unique_lock<std::mutex> lock(entityMutex);
	for (int i = 0; i < map.fauna.size(); i++)
	{
		livingEntity* ent = &map.fauna[i];
		if (inView(camera, ent))
		{			
			window.draw(ent->sprite);
		}
	}
}


void drawFloraFaunaDebug(sf::RenderWindow& window, livingEntity& player, Map& map)
{

	for (int i = 0; i < map.flora.size(); i++)
	{
		window.draw(map.flora[i].sprite);
		map.flora[i].label.setPosition(sf::Vector2f(map.flora[i].getPos().x, map.flora[i].getPos().y - 25));
		window.draw(map.flora[i].label);

		sf::Text chunkFlora;
		sf::Vector2i locationChunk = getCurrentTileMapPos(map.flora[i].getPos());
		chunkFlora.setFont(font);
		chunkFlora.setString(std::to_string(locationChunk.x) + "," + std::to_string(locationChunk.y));
		chunkFlora.setPosition(map.flora[i].getPos());
		window.draw(chunkFlora);
	}

	std::unique_lock<std::mutex> lock(entityMutex);
	for (int i = 0; i < map.fauna.size(); i++)
	{
		sf::Color sightCol = { 255,255,255,150 };
		window.draw(map.fauna[i].sprite);

		sf::Lock lock(lineMutexDebug);

		for (int j = 0; j < viewLineList2.size(); j++)
		{
			window.draw(viewLineList2[j]);
		}
		if (i %2 == 0) { viewLineList2.clear(); };

		map.fauna[i].label.setPosition(sf::Vector2f(map.fauna[i].getPos().x, map.fauna[i].getPos().y - 25));
		window.draw(map.fauna[i].label);
		sf::CircleShape circle(map.fauna[i].viewRange);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
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
		//window.draw(circle);

		sf::Text chunkFuna;
		sf::Vector2i locationChunk = getCurrentTileMapPos(map.fauna[i].getPos());
		chunkFuna.setFont(font);
		chunkFuna.setString(std::to_string(locationChunk.x) + "," + std::to_string(locationChunk.y));
		chunkFuna.setPosition(map.fauna[i].getPos());
		window.draw(chunkFuna);
	}

	sf::Text chunk;
	sf::Vector2i playerChunk = getCurrentTileMapPos(player.getPos());
	chunk.setFont(font);
	chunk.setString(std::to_string(playerChunk.x) + "," + std::to_string(playerChunk.y));
	chunk.setPosition(player.getPos());

	window.draw(chunk);
}

sf::Vector2i sideOfMapDist(livingEntity& entity, float tripDist)
{
	sf::Vector2i tmPos = getCurrentTileMapPos(entity.getPos());
	float left = abs(entity.getPos().x) - abs(tmPos.x * mapSize);
	float right = abs(entity.getPos().x) - (abs(tmPos.x * mapSize) + mapSize);

	float up = abs(entity.getPos().y) - abs(tmPos.y * mapSize);
	float down = abs(entity.getPos().y) - (abs(tmPos.y * mapSize)+mapSize);

	int x = 0;
	int y = 0;
	if (abs(left) < tripDist)
	{
		x = -1;
	}
	if (abs(right) < tripDist)
	{
		x = 1;
	}
	if (abs(up) < tripDist)
	{
		y = -1;
	}
	if(abs(down) < tripDist)
	{
		y = 1;
	}
	return sf::Vector2i({ x,y });
}
sf::Vector2i sideOfMap(livingEntity& entity, float boundsLower)
{
	float spawnMapBoundsUpper = 1 - boundsLower;

	float x = entity.sprite.getPosition().x;
	float y = entity.sprite.getPosition().y;

	float tripBoundsx = (x / (mapSize));
	float tripBoundsy = (y / (mapSize));

	int offsetX = 0;
	int offsetY = 0;

	sf::Vector2i vecPos = getCurrentTileMapPos(entity.getPos());

	int indexx = vecPos.x;
	int indexy = vecPos.y;

	if (tripBoundsx - (float)indexx >= spawnMapBoundsUpper)
	{
		offsetX += 1;
	}
	else if (tripBoundsx - (float)indexx <= boundsLower)
	{
		offsetX -= 1;
	}
	if (tripBoundsy - (float)indexy >= spawnMapBoundsUpper)
	{
		offsetY += 1;
	}
	else if (tripBoundsy - (float)indexy <= boundsLower)
	{
		offsetY -= 1;
	}
	return sf::Vector2i{ offsetX,offsetY };
}

void drawnNearestMaps(sf::RenderWindow& window, livingEntity& player, sf::View& camera)
{
	//std::cout << "Player Map Pos-  X:" << indexx << "  Y:" << indexy << std::endl;
	sf::Vector2i off = sideOfMap(player, 0.35f);
	int offsetX = off.x;
	int offsetY = off.y;

	sf::Vector2i vecPos = getCurrentTileMapPos(player.getPos());

	int indexx = vecPos.x;
	int indexy = vecPos.y;

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
		drawFloraFauna(window, player, hashedMaps[{indexx, indexy}], camera);

		if ((offsetX != 0) && (offsetY != 0))
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}], camera);
			drawFloraFauna(window, player, hashedMaps[{indexx, indexy + offsetY}], camera);
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}], camera);
		}
		else if (offsetX != 0)
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}], camera);
			drawFloraFauna(window, player, hashedMaps[{indexx, indexy + offsetY}], camera);
		}
		else if (offsetY != 0)
		{
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy + offsetY}], camera);
			drawFloraFauna(window, player, hashedMaps[{indexx + offsetX, indexy}], camera);
		}
	}
}

//draw all elements you want to show on screen
void draw(sf::RenderWindow & window, livingEntity& player, sf::View& camera)
{
	//TODO check distance of object to see if onscreen or not before drawing

	//tile set ground level draw
	drawnNearestMaps(window, player,camera);

	//player level draw
	window.draw(player.sprite);	
}

sf::Thread mapsThread(&initMaps);
sf::Thread aiThread(&aiUpdate);
void init(sf::View& camera, sf::RenderWindow& window, livingEntity& player)
{
	srand(time(NULL));
	
	std::string fontPath = "resources/font/Pelagiad.ttf";
	if (!font.loadFromFile(fontPath))
	{
		std::cout << "could not load font " << fontPath << " from file" << std::endl;
	}
	

	std::string gameSeed = readFile("resources/environment/map_data/gameSeed.txt");
	srand(std::stoi(gameSeed));

	mapsThread.launch();
	aiThread.launch(); //start ai after maps is loaded since maps loads AIs

	sf::FloatRect fr = player.sprite.getLocalBounds();
	player.sprite.setOrigin(fr.width / 2, fr.height / 2); // set origin of sprite to center of sprite

	player.sprite.setPosition((mapSize * fullWorldDimensions) / 2, (mapSize * fullWorldDimensions) / 2);
	
	//create running animation and assign it to the play object's move animation
	std::vector<std::string> lst{ "resources/player/playerWalk0.png","resources/player/playerWalk1.png", "resources/player/playerWalk2.png", "resources/player/playerWalk3.png", "resources/player/playerWalk4.png" };
	player.animAssign(lst, &player.moveAnim);

	camera.move(sf::Vector2f((mapSize * fullWorldDimensions) / 2, (mapSize * fullWorldDimensions) / 2));
	window.setView(camera);
}


//takes a ref to entity to change animation frame
void animGeneric(baseEntity& entity, std::vector<sf::Texture>& anim)
{
	//TODO reset animFrame when animation state changes, i think this might be an issue
	if (entity.animFrame + 1 >= anim.size())
	{
		entity.animFrame = 0;
	}
	else
	{
		entity.animFrame = entity.animFrame + 1;
	}
	entity.sprite.setTexture((anim)[entity.animFrame]);
}


//logic to switch between defined animation states
void animate(livingEntity& player)
{
	if (player.entityMoving)
	{
		animGeneric(player, player.moveAnim);
	}
	else
	{
		animGeneric(player, player.defaultAnim);
	}
}
