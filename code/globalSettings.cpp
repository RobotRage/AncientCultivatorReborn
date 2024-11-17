#include "globalSettings.h";
#include "includes.h"
#include <mutex>

const bool debug = 1;

const int screenx = 800;
const int screeny = 650;
const int playerDistFromCenterCamToTriggerMoveCam = 150;
sf::Font font;

sf::Mutex lineMutexDebug;
std::vector<sf::VertexArray> viewLineList;
std::vector<sf::VertexArray> viewLineList2;