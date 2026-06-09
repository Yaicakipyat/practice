#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameObjects.h"
#include "constants.h"
class Game {
public:
    void init();
    void update(float dt);
    void draw(sf::RenderWindow& win);

private:
    // Игрок
    float px = W / 2.f, py = H - 60.f;
    float pSpeed = 250.f;
    int   lives = 3;
    float shootCooldown = 0;
    float invincible = 0;

    // Пули
    std::vector<Bullet>   playerBullets;
    std::vector<Bullet>   enemyBullets;

    // Враги
    std::vector<Enemy>    enemies;
    int   wave = 0;
    float waveTimer = 0;
    float formTimer = 0;     // таймер входа в строй
    bool  formDone = false;

    // Частицы
    std::vector<Particle> particles;

    // Звёзды
    std::vector<sf::CircleShape> stars;

    // Разное
    float enemyShootTimer = 0;
    bool  gameOver = false;
    float gameOverTimer = 0;
    float flashTimer = 0;

    // Вспомогательные методы
    void spawnWave();
    void spawnParticles(float x, float y, sf::Color col, int n = 12);
};

#endif
