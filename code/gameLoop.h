#pragma once

#include "includes.h"
#include "entityObject.h"

void camMoveCheck(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
void update(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
void draw(sf::RenderWindow& window, livingEntity& player);
void init(sf::View& camera, sf::RenderWindow& window, livingEntity& player);
void animate(livingEntity& player);
void animGeneric(baseEntity& entity, std::vector<sf::Texture>& anim);

