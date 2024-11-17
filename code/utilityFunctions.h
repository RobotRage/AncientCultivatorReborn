#pragma once
#include "includes.h"
#include "entityObject.h"
#include <mutex>

float distance(const sf::Vector2f& p1, const sf::Vector2f& p2);
sf::Vector2f roundVectorToWhole(const sf::Vector2f& vec);
sf::Vector2f getUnitVector(const sf::Vector2f& from, const sf::Vector2f& to);
std::string readFile(const std::string filePath);
std::vector<int> strVecToIntVec(std::vector<std::string> strLst);
std::vector<std::string> splitString(const std::string str, char delimiter);
void print(std::string s);
void sleep(float seconds);

