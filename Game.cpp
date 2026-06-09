#include "Game.h"
#include "Shapes.h"
#include "constants.h"
#include <cmath>     
#include <algorithm>  
#include <cstdlib>    
#include <ctime>      

void Game::init() {
    px = W / 2.f; py = H - 60.f;
    lives = 3;
    shootCooldown = 0; invincible = 0;
    playerBullets.clear(); enemyBullets.clear();
    enemies.clear(); particles.clear();
    wave = 0; waveTimer = 0; formTimer = 0; formDone = false;
    enemyShootTimer = 0;
    gameOver = false; gameOverTimer = 0; flashTimer = 0;
    spawnWave();

    // звёзды
    stars.clear();
    for (int i = 0; i < 120; i++) {
        sf::CircleShape s(std::rand() % 2 == 0 ? 1.f : 0.5f);
        s.setPosition(std::rand() % W, std::rand() % H);
        int b = 100 + std::rand() % 156;
        s.setFillColor({(sf::Uint8)b, (sf::Uint8)b, (sf::Uint8)b});
        stars.push_back(s);
    }
}

void Game::spawnWave() {
    enemies.clear();
    formDone = false;
    formTimer = 0;
    wave++;

    int rows = std::min(3 + wave / 2, 5);
    int cols = std::min(6 + wave, 10);

    float gapX = W / (float)(cols + 1);
    float gapY = 45.f;
    float startY = 60.f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            Enemy e;
            e.homeX = gapX * (c + 1);
            e.homeY = startY + r * gapY;
            // появляются сверху за пределами экрана
            e.x = e.homeX + (std::rand() % 200 - 100);
            e.y = -50 - r * 30 - c * 5;
            e.phase = (float)(c + r) * 0.3f;
            e.hp = (r == 0) ? 2 : 1;
            sf::Color cols[] = {
                {255,100,100}, {255,200,50}, {100,220,255},
                {180,100,255}, {100,255,150}
            };
            e.col = cols[r % 5];
            e.shape = makeEnemy(14.f, e.col);
            e.alive = true;
            enemies.push_back(e);
        }
    }
}

void Game::spawnParticles(float x, float y, sf::Color col, int n) {
    for (int i = 0; i < n; i++) {
        Particle p;
        float angle = (float)(std::rand() % 360) * PI / 180.f;
        float speed = 50.f + std::rand() % 150;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        p.maxLife = p.life = 0.4f + (std::rand() % 40) / 100.f;
        float r = 2.f + std::rand() % 4;
        p.shape.setRadius(r);
        p.shape.setOrigin(r, r);
        p.shape.setPosition(x, y);
        p.shape.setFillColor(col);
        particles.push_back(p);
    }
}

void Game::update(float dt) {
    if (gameOver) {
        gameOverTimer += dt;
        if (gameOverTimer > 2.5f) init();
        return;
    }

    flashTimer = std::max(0.f, flashTimer - dt);

    // звёзды
    for (auto& s : stars) {
        sf::Vector2f pos = s.getPosition();
        pos.y += 30.f * dt;
        if (pos.y > H) pos.y = 0;
        s.setPosition(pos);
    }

    if (invincible > 0) invincible -= dt;

    // движение игрока
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        px -= pSpeed * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        px += pSpeed * dt;
    px = std::max(20.f, std::min((float)W - 20.f, px));

    // стрельба игрока
    shootCooldown -= dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shootCooldown <= 0) {
        Bullet b;
        b.shape.setSize({4, 14});
        b.shape.setFillColor({100, 255, 100});
        b.shape.setOrigin(2, 7);
        b.shape.setPosition(px, py - 20);
        b.vy = -520.f;
        playerBullets.push_back(b);
        shootCooldown = 0.18f;
    }

    // движение пуль игрока
    for (auto& b : playerBullets) {
        b.shape.move(0, b.vy * dt);
        if (b.shape.getPosition().y < -10) b.alive = false;
    }

    // движение врагов / вход в строй
    formTimer += dt;
    float enterTime = 2.5f;
    if (formTimer < enterTime) {
        float t = formTimer / enterTime;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            e.x += (e.homeX - e.x) * dt * 4.f;
            e.y += (e.homeY - e.y) * dt * 4.f;
        }
    } else {
        if (!formDone) formDone = true;
        float wobble = 18.f + wave * 2.f;
        float waveSpeed = 0.8f + wave * 0.1f;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (!e.diving) {
                e.phase += dt * waveSpeed;
                e.x = e.homeX + std::sin(e.phase) * wobble;
                e.y = e.homeY + std::cos(e.phase * 0.5f) * 8.f;
                // случайное пикирование
                if (formDone && std::rand() % 10000 < (int)(5 + wave * 1.5f)) {
                    e.diving = true;
                    float dx = px - e.x;
                    float dy = py - e.y;
                    float len = std::sqrt(dx*dx + dy*dy);
                    float spd = 200.f + wave * 15.f;
                    e.diveVX = dx / len * spd;
                    e.diveVY = dy / len * spd;
                }
            } else {
                e.x += e.diveVX * dt;
                e.y += e.diveVY * dt;
                if (e.y > H + 40) {
                    e.y = -40;
                    e.x = e.homeX;
                    e.diving = false;
                }
            }
            e.shape.setPosition(e.x, e.y);
        }
    }

    // стрельба врагов
    enemyShootTimer -= dt;
    if (enemyShootTimer <= 0 && formDone) {
        enemyShootTimer = std::max(0.35f, 1.2f - wave * 0.08f);
        std::vector<int> aliveIdx;
        for (int i = 0; i < (int)enemies.size(); i++)
            if (enemies[i].alive) aliveIdx.push_back(i);
        if (!aliveIdx.empty()) {
            int idx = aliveIdx[std::rand() % aliveIdx.size()];
            Bullet b;
            b.shape.setSize({3, 10});
            b.shape.setFillColor({255, 80, 80});
            b.shape.setOrigin(1.5f, 5);
            b.shape.setPosition(enemies[idx].x, enemies[idx].y);
            b.vy = 220.f + wave * 12.f;
            enemyBullets.push_back(b);
        }
    }

    // движение пуль врагов
    for (auto& b : enemyBullets) {
        b.shape.move(0, b.vy * dt);
        if (b.shape.getPosition().y > H + 10) b.alive = false;
    }

    // столкновения: пули игрока с врагами
    for (auto& b : playerBullets) {
        if (!b.alive) continue;
        sf::Vector2f bp = b.shape.getPosition();
        for (auto& e : enemies) {
            if (!e.alive) continue;
            float dx = bp.x - e.x;
            float dy = bp.y - e.y;
            if (dx*dx + dy*dy < 18.f*18.f) {
                b.alive = false;
                e.hp--;
                if (e.hp <= 0) {
                    e.alive = false;
                    spawnParticles(e.x, e.y, e.col, 14);
                } else {
                    spawnParticles(e.x, e.y, {255,255,255}, 5);
                }
                break;
            }
        }
    }

    // столкновения: пули врагов с игроком
    for (auto& b : enemyBullets) {
        if (!b.alive) continue;
        sf::Vector2f bp = b.shape.getPosition();
        float dx = bp.x - px;
        float dy = bp.y - py;
        if (dx*dx + dy*dy < 16.f*16.f && invincible <= 0) {
            b.alive = false;
            lives--;
            invincible = 2.f;
            flashTimer = 0.3f;
            spawnParticles(px, py, {100, 200, 255}, 16);
            if (lives <= 0) {
                spawnParticles(px, py, {255,150,50}, 30);
                gameOver = true;
            }
        }
    }

    // столкновения: враги с игроком
    for (auto& e : enemies) {
        if (!e.alive || invincible > 0) continue;
        float dx = e.x - px;
        float dy = e.y - py;
        if (dx*dx + dy*dy < 22.f*22.f) {
            e.alive = false;
            lives--;
            invincible = 2.f;
            flashTimer = 0.3f;
            spawnParticles(e.x, e.y, e.col, 12);
            spawnParticles(px, py, {100, 200, 255}, 12);
            if (lives <= 0) gameOver = true;
        }
    }

    // частицы
    for (auto& p : particles) {
        p.life -= dt;
        if (p.life <= 0) { p.alive = false; continue; }
        p.shape.move(p.vx * dt, p.vy * dt);
        float a = p.life / p.maxLife;
        sf::Color c = p.shape.getFillColor();
        c.a = (sf::Uint8)(a * 255);
        p.shape.setFillColor(c);
    }

    // удаление мёртвых объектов
    auto rm = [](auto& v){
        v.erase(std::remove_if(v.begin(), v.end(),
            [](const auto& x){ return !x.alive; }), v.end());
    };
    rm(playerBullets);
    rm(enemyBullets);
    rm(enemies);
    rm(particles);

    // следующая волна
    if (formDone && enemies.empty()) {
        waveTimer += dt;
        if (waveTimer > 1.5f) {
            waveTimer = 0;
            spawnWave();
        }
    }
}

void Game::draw(sf::RenderWindow& win) {
    sf::Color bg = (flashTimer > 0) ? sf::Color(60, 10, 10) : sf::Color(5, 5, 18);
    win.clear(bg);

    for (auto& s : stars) win.draw(s);

    for (auto& e : enemies) {
        if (!e.alive) continue;
        e.shape.setPosition(e.x, e.y);
        win.draw(e.shape);
        if (e.hp == 2) {
            sf::RectangleShape bar({16,3});
            bar.setFillColor({255,255,0});
            bar.setPosition(e.x - 8, e.y - 20);
            win.draw(bar);
        }
    }

    for (auto& b : playerBullets) win.draw(b.shape);
    for (auto& b : enemyBullets) win.draw(b.shape);
    for (auto& p : particles) win.draw(p.shape);

    bool showPlayer = !gameOver && (invincible <= 0 || (int)(invincible * 8) % 2 == 0);
    if (showPlayer) {
        auto ship = makeShip(18.f, {100, 200, 255});
        ship.setPosition(px, py);
        win.draw(ship);
        sf::CircleShape glow(5);
        glow.setOrigin(5,5);
        glow.setPosition(px, py + 18);
        glow.setFillColor({80, 140, 255, 120});
        win.draw(glow);
    }

    for (int i = 0; i < lives; i++) {
        auto icon = makeShip(9.f, {80, 160, 220});
        icon.setPosition(18 + i * 24.f, H - 18.f);
        win.draw(icon);
    }

    for (int i = 0; i < wave; i++) {
        sf::CircleShape dot(3);
        dot.setOrigin(3,3);
        dot.setPosition(W - 15 - i * 10.f, H - 15);
        dot.setFillColor({180, 180, 80});
        win.draw(dot);
    }
}
