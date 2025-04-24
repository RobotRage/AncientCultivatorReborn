#include "globalSettings.h";
#include "includes.h"

#include "list"

bool debug = 0;

const int screenx = 800;
const int screeny = 650;
const int playerDistFromCenterCamToTriggerMoveCam = 150;
sf::Font font;

std::list<sf::VertexArray> viewLineList;
std::list<sf::VertexArray> viewLineList2;

livingEntity player;

sf::Texture plyrTexture;
sf::Sprite plyrSprite;
void playerLoad()
{
	plyrTexture.loadFromFile("resources/player/playerWalk0.png");
	plyrSprite.setTexture(plyrTexture);

	player.sprite = &plyrSprite;

	player.setPos(sf::Vector2f(20,20));
	player.name = "player";

	player.sprite->setScale(0.5f, 0.5f);
	player.speed = 20;
}
