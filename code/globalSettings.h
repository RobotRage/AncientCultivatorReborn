#pragma once
#include "includes.h"

extern const int screenx;
extern const int screeny;

extern const int playerDistFromCenterCamToTriggerMoveCam;
extern sf::Font font;

extern const bool debug;

extern sf::Mutex lineMutexDebug;
extern std::vector<sf::VertexArray> viewLineList;
extern std::vector<sf::VertexArray> viewLineList2;