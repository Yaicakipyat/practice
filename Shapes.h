#ifndef SHAPES_H
#define SHAPES_H

#include <SFML/Graphics.hpp>
#include "constants.h"

sf::ConvexShape makeShip(float size, sf::Color col);
sf::ConvexShape makeEnemy(float size, sf::Color col);

#endif
