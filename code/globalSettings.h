#pragma once
#include "includes.h"
#define Pi 3.14159265359f
#define UPDATE_RATE (1.0f / 144.0f) // 1/x = x updates per second
extern const int screenx;
extern const int screeny;

extern const int playerDistFromCenterCamToTriggerMoveCam;
extern sf::Font font;

extern bool debug;

extern std::list<sf::VertexArray> viewLineList;
extern std::list<sf::VertexArray> viewLineList2;

extern livingEntity player;
void playerLoad();