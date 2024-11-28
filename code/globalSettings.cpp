#include "globalSettings.h";
#include "includes.h"
#include <mutex>



bool debug = 0;

const int screenx = 800;
const int screeny = 650;
const int playerDistFromCenterCamToTriggerMoveCam = 150;
sf::Font font;

sf::Mutex lineMutexDebug;
std::vector<sf::VertexArray> viewLineList;
std::vector<sf::VertexArray> viewLineList2;

livingEntity player;
void playerLoad()
{
	player.load("resources/player/playerWalk0.png");
	player.sprite.setScale(0.5f, 0.5f);
}
