#include "includes.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include "entityObject.h"
#include "mapLoader.h"

void sleep(float seconds)
{
    sf::sleep(sf::seconds(seconds));;
}

void print(std::string s)
{
    std::cout << s << std::endl;
}

bool moved = false;


// Calculates the distance between two points represented as SFML vectors
float distanceCheck(const sf::Vector2f& v1, const sf::Vector2f& v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Rounds the components of a vector to the nearest whole number
sf::Vector2f roundVectorToWhole(const sf::Vector2f& vec)
{
    return sf::Vector2f(std::round(vec.x), std::round(vec.y));
}


// Calculates the unit vector pointing from v1 to v2
sf::Vector2f getUnitVector(const sf::Vector2f& from, const sf::Vector2f& to)
{
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float length = std::sqrt(dx * dx + dy * dy);

    // Avoid division by zero
    if (length != 0)
    {
        return sf::Vector2f(dx / length, dy / length);
    }
    else
    {
        return sf::Vector2f(0, 0);
    }
}

std::string readFile(const std::string filePath)
{
    std::ifstream file(filePath); 
    std::string lines;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
    }

    std::string line;
    while (std::getline(file, line)) { 
        lines += line;
    }

    file.close(); 
    return lines; 
}

std::vector<int> strVecToIntVec(std::vector<std::string> strLst)
{
    std::vector<int> intLst; 
    for (int i = 0; i < strLst.size(); i++)
    {
        intLst.push_back(std::stoi(strLst[i]));
    }
    return intLst;
}

std::vector<std::string> splitString(const std::string str, char delimiter) {
    std::vector<std::string> tokens; 
    std::string token; 
    std::stringstream ss(str);

    // Use getline to read each token separated by the delimiter
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token); // Add the token to the vector
    }

    return tokens; // Return the vector of tokens
}
