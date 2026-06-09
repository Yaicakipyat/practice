#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>

struct Bullet {
    sf::RectangleShape shape;
    float vy;
    bool alive = true;
};

struct Enemy {
    sf::ConvexShape shape;
    float x, y;
    float homeX, homeY;   // позиция в строю
    float phase = 0;      // покачивание
    bool  diving = false;
    float diveVX = 0, diveVY = 0;
    float diveAngle = 0;
    bool  alive = true;
    sf::Color col;
    int   hp = 1;
};

struct Particle {
    sf::CircleShape shape;
    float vx, vy;
    float life, maxLife;
    bool alive = true;
};

#endif
