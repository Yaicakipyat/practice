#include "Shapes.h"
#include <cmath>   

// ... остальной код без изменений
sf::ConvexShape makeShip(float size, sf::Color col) {
    sf::ConvexShape s;
    s.setPointCount(7);
    s.setPoint(0, {0, -size});
    s.setPoint(1, {size * 0.5f, -size * 0.3f});
    s.setPoint(2, {size * 0.4f,  size * 0.6f});
    s.setPoint(3, {size * 0.2f,  size * 0.3f});
    s.setPoint(4, {-size * 0.2f, size * 0.3f});
    s.setPoint(5, {-size * 0.4f, size * 0.6f});
    s.setPoint(6, {-size * 0.5f, -size * 0.3f});
    s.setFillColor(col);
    s.setOrigin(0, 0);
    return s;
}

sf::ConvexShape makeEnemy(float size, sf::Color col) {
    sf::ConvexShape s;
    s.setPointCount(8);
    float r = size;
    for (int i = 0; i < 8; i++) {
        float a = i * PI / 4;
        float rr = (i % 2 == 0) ? r : r * 0.55f;
        s.setPoint(i, {rr * std::sin(a), -rr * std::cos(a)});
    }
    s.setFillColor(col);
    return s;
}
