#pragma once
#include "includes.h"
#define Pi 3.14159265359f
extern const int screenx;
extern const int screeny;

extern const int playerDistFromCenterCamToTriggerMoveCam;
extern sf::Font font;

extern bool debug;

extern sf::Mutex lineMutexDebug;
extern std::vector<sf::VertexArray> viewLineList;
extern std::vector<sf::VertexArray> viewLineList2;

extern livingEntity player;
void playerLoad();