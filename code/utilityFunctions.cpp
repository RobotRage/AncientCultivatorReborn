#include "includes.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include "entityObject.h"
#include "mapLoader.h"

int dot(int u1, int u2, int v1, int v2)
{
    return ((u1 * v1) + (u2 * v2));
}

int magnitude(int x, int y)
{
    x *= x;
    y *= y;

    return sqrt(x + y);
}

// angle against horizontal axis
int getVectorAngle(int u1, int u2, int v1, int v2)
{
    int numerator = dot(u1, u2, v1, v2);
    int denominator = magnitude(u1, u2) * magnitude(v1, v2);

    float result = ((float)numerator / (float)denominator);
    int deg = acos(result) * (180 / Pi);
    return deg;
}

double toDeg(double rad)
{
    return rad * (180 / Pi);
}

void sleep(float seconds)
{
    sf::sleep(sf::seconds(seconds));;
}

void print(std::string s)
{
    std::cout << s << std::endl;
}

bool moved = false;



float distance(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
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
