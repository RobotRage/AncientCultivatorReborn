#pragma once

#include "includes.h"
#include "entityObject.h"

void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer);
void update(sf::View& camera, sf::RenderWindow& window, livingEntity& player, float lastFrameTimer);
void draw(sf::RenderWindow& window, livingEntity& player, sf::View& camera);
void init(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
void animate(livingEntity& player);
void animGeneric(baseEntity& entity, std::vector<sf::Texture>& anim);
sf::Vector2i sideOfMap(livingEntity& entity, float bounds);
sf::Vector2i sideOfMapDist(livingEntity& entity, float tripDist);