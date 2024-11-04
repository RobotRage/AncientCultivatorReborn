#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

//tilemap code sourced from https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
class TileMap : public sf::Drawable, public sf::Transformable
{
public:

    bool load(sf::Texture& m_tileset, const sf::Vector2u& tileSize, const int& width, const int& height,const sf::Vector2f& offset, int tile)
    {
        M_tileset = m_tileset;
        sf::Vector2f off;
        off.x = round(offset.x);
        off.y = round(offset.y);


        // Resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);

        // Populate the vertex array, with one quad per tile
        for ( int i = 0; i < width; ++i)
        {
            for ( int j = 0; j < height; ++j)
            {
                // Get the current tile number
                int tileNumber = tile;

                // Find its position in the tileset texture
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // Get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices[(i + j * width) * 4];               

                // Define its 4 corners
                quad[0].position = sf::Vector2f(round(i * tileSize.x),round( j * tileSize.y))             + off;
                quad[1].position = sf::Vector2f(round( (i + 1) * tileSize.x),round( j * tileSize.y))      + off;
                quad[2].position = sf::Vector2f(round((i + 1) * tileSize.x),round((j + 1) * tileSize.y))  + off;
                quad[3].position = sf::Vector2f(round(i * tileSize.x),round((j + 1) * tileSize.y))        + off;

                // Define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(round(tu * tileSize.x),round(tv * tileSize.y));
                quad[1].texCoords = sf::Vector2f(round((tu + 1) * tileSize.x),round(tv * tileSize.y));
                quad[2].texCoords = sf::Vector2f(round((tu + 1) * tileSize.x),round((tv + 1) * tileSize.y));
                quad[3].texCoords = sf::Vector2f(round(tu * tileSize.x),round((tv + 1) * tileSize.y));
            }
        }
        return true;
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        states.texture = &M_tileset;

        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices; // Vertex array to hold the tiles
    sf::Texture M_tileset;
};
