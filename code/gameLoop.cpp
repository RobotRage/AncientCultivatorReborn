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
bool userInput(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
#pragma endregion



//main thread (ai thread in updateAI.cpp)
void update(sf::View& camera, sf::RenderWindow& window, livingEntity& player)
{
	player.entityMoving = userInput(camera, window, player);
	if (player.entityMoving) { camMoveCheck(camera, window,player); } // only check camera bounds if player is inputting movement
	//print("update");
}

// move the camera in the direction the player is moving if they are outside a certain distance of the center of the screen
void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player)
{
	sf::Vector2f centerCam = camera.getCenter();
	sf::Vector2f playerPos = player.sprite.getPosition();

	float dist = distanceCheck(centerCam, playerPos);
	if (dist > playerDistFromCenterCamToTriggerMoveCam)
	{

		//unitVector gives direction but moves only 1 pixel at a time
		sf::Vector2f unitVec = roundVectorToWhole(getUnitVector(centerCam, playerPos));

		//scale unit vector with move speed to move camera with bigger steps
		unitVec.x = unitVec.x * player.getSpeed();
		unitVec.y = unitVec.y * player.getSpeed();

		//move the camera in the direction of the vector from the center of the screen pointing to the player and set the view
		camera.move(unitVec);
		window.setView(camera);
	}
}


// WASD & Arrow keys to move
// L/Rshift to run
// 0 and 9 to zoom cam
bool userInput(sf::View& camera, sf::RenderWindow& window, livingEntity& player)
{
#pragma region 8D movement input

	bool A = false; bool W = false; bool D = false; bool S = false;

	sf::Vector2f pos = player.sprite.getPosition();
	sf::Vector2f change(0, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
	{
		player.running = 2;
	}
	else
	{
		player.running = 1;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		change.x = change.x - 1;
		player.moveEntity(pos, change * (float)player.getSpeed());
		player.sprite.setRotation(270);
		A = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		change.x = change.x + 1;
		player.moveEntity(pos, change * (float)player.getSpeed());
		player.sprite.setRotation(90);
		D = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		change.y = change.y + 1;
		player.moveEntity(pos, change * (float)player.getSpeed());
		player.sprite.setRotation(180);
		S = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		change.y = change.y - 1;
		player.moveEntity(pos, change * (float)player.getSpeed());
		player.sprite.setRotation(0);
		W = true;
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

	if (abs(player.travelCounter.x) > (mapSize / 2) || abs(player.travelCounter.y) > (mapSize / 2))
	{
		calcNearestMaps(player);
		player.travelCounter.x = 0;
		player.travelCounter.y = 0;
	}

#pragma endregion
	if (A || D || S || W) { return true; }
	else { return false; }
}


//draw all elements you want to show on screen
void draw(sf::RenderWindow & window, livingEntity& player)
{
	//TODO check distance of object to see if onscreen or not before drawing

	//tile set ground level draw
	drawnNearestMaps(window, player);

	//player level draw
	window.draw(player.sprite);	
}


sf::Thread aiThread(&aiUpdate);
void init(sf::View& camera, sf::RenderWindow& window, livingEntity& player)
{
	srand(time(NULL));

	aiThread.launch();
	initAi();
	std::string gameSeed = readFile("resources/environment/map_data/gameSeed.txt");
	srand(std::stoi(gameSeed));

	
	
	//hotLoad.launch();
	initMaps(player);
	

	//multithreaded AI logic
	

	sf::FloatRect fr = player.sprite.getLocalBounds();
	player.sprite.setOrigin(fr.width / 2, fr.height / 2); // set origin of sprite to center of sprite

	player.sprite.setPosition((mapSize * fullWorldDimensions) / 2, (mapSize * fullWorldDimensions) / 2);
	
	//create running animation and assign it to the play object's move animation
	std::vector<std::string> lst{ "resources/player/playerWalk0.png","resources/player/playerWalk1.png", "resources/player/playerWalk2.png", "resources/player/playerWalk3.png", "resources/player/playerWalk4.png" };
	player.animAssign(lst, &player.moveAnim);

	camera.move(sf::Vector2f((mapSize * fullWorldDimensions) / 2, (mapSize * fullWorldDimensions) / 2));
	window.setView(camera);
	calcNearestMaps(player);
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
