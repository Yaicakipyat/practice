// Galaga on SFML — no text output, pure gameplay
// Compile: g++ main.cpp -o galaga -lsfml-graphics -lsfml-window -lsfml-system
//
// Controls: A/D or Left/Right — move, Space — shoot
// Enemies spawn in waves, dive toward player.
// No score text, no lives text, no "START", no "GAME OVER" text on screen.
// Visual feedback only: lives shown as small ship icons, game restarts on death.

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// ── constants ───────────────────────────────────────────────────────────────
const int   W  = 600;
const int   H  = 700;
const float PI = 3.14159265f;

// ── helpers ─────────────────────────────────────────────────────────────────
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

// ── structs ──────────────────────────────────────────────────────────────────
struct Bullet {
    sf::RectangleShape shape;
    float vy;
    bool alive = true;
};

struct Enemy {
    sf::ConvexShape shape;
    float x, y;
    float homeX, homeY;   // grid position
    float phase = 0;      // wobble
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

// ── game state ───────────────────────────────────────────────────────────────
struct Game {
    // player
    float px = W / 2.f, py = H - 60.f;
    float pSpeed = 250.f;
    int   lives = 3;
    float shootCooldown = 0;
    float invincible = 0;  // brief invincibility after hit

    // bullets
    std::vector<Bullet>   playerBullets;
    std::vector<Bullet>   enemyBullets;

    // enemies
    std::vector<Enemy>    enemies;
    int   wave = 0;
    float waveTimer = 0;
    float formTimer = 0;   // enemies enter formation
    bool  formDone = false;

    // particles
    std::vector<Particle> particles;

    // stars
    std::vector<sf::CircleShape> stars;

    // misc
    float enemyShootTimer = 0;
    bool  gameOver = false;
    float gameOverTimer = 0;   // wait before restart
    float flashTimer = 0;

    void init() {
        px = W / 2.f; py = H - 60.f;
        lives = 3;
        shootCooldown = 0; invincible = 0;
        playerBullets.clear(); enemyBullets.clear();
        enemies.clear(); particles.clear();
        wave = 0; waveTimer = 0; formTimer = 0; formDone = false;
        enemyShootTimer = 0;
        gameOver = false; gameOverTimer = 0; flashTimer = 0;
        spawnWave();

        // stars
        stars.clear();
        for (int i = 0; i < 120; i++) {
            sf::CircleShape s(std::rand() % 2 == 0 ? 1.f : 0.5f);
            s.setPosition(std::rand() % W, std::rand() % H);
            int b = 100 + std::rand() % 156;
            s.setFillColor({(sf::Uint8)b, (sf::Uint8)b, (sf::Uint8)b});
            stars.push_back(s);
        }
    }

    void spawnWave() {
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
                // start from off-screen top
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

    void spawnParticles(float x, float y, sf::Color col, int n = 12) {
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

    void update(float dt) {
        if (gameOver) {
            gameOverTimer += dt;
            if (gameOverTimer > 2.5f) init();
            return;
        }

        flashTimer = std::max(0.f, flashTimer - dt);

        // ── stars scroll ────────────────────────────────────────────────────
        for (auto& s : stars) {
            sf::Vector2f pos = s.getPosition();
            pos.y += 30.f * dt;
            if (pos.y > H) pos.y = 0;
            s.setPosition(pos);
        }

        // ── player movement ──────────────────────────────────────────────────
        if (invincible > 0) invincible -= dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            px -= pSpeed * dt;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            px += pSpeed * dt;
        px = std::max(20.f, std::min((float)W - 20.f, px));

        // ── player shoot ─────────────────────────────────────────────────────
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

        // ── player bullets ────────────────────────────────────────────────────
        for (auto& b : playerBullets) {
            b.shape.move(0, b.vy * dt);
            if (b.shape.getPosition().y < -10) b.alive = false;
        }

        // ── enemy movement / formation ────────────────────────────────────────
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
                    // randomly start dive
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
                    // curve back up when past player
                    if (e.y > H + 40) {
                        e.y = -40;
                        e.x = e.homeX;
                        e.diving = false;
                    }
                }
                e.shape.setPosition(e.x, e.y);
            }
        }

        // ── enemy shoot ───────────────────────────────────────────────────────
        enemyShootTimer -= dt;
        if (enemyShootTimer <= 0 && formDone) {
            enemyShootTimer = std::max(0.35f, 1.2f - wave * 0.08f);
            // pick a random alive enemy
            std::vector<int> alive;
            for (int i = 0; i < (int)enemies.size(); i++)
                if (enemies[i].alive) alive.push_back(i);
            if (!alive.empty()) {
                int idx = alive[std::rand() % alive.size()];
                Bullet b;
                b.shape.setSize({3, 10});
                b.shape.setFillColor({255, 80, 80});
                b.shape.setOrigin(1.5f, 5);
                b.shape.setPosition(enemies[idx].x, enemies[idx].y);
                b.vy = 220.f + wave * 12.f;
                enemyBullets.push_back(b);
            }
        }

        // ── enemy bullets ─────────────────────────────────────────────────────
        for (auto& b : enemyBullets) {
            b.shape.move(0, b.vy * dt);
            if (b.shape.getPosition().y > H + 10) b.alive = false;
        }

        // ── collisions: player bullets vs enemies ────────────────────────────
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

        // ── collisions: enemy bullets vs player ──────────────────────────────
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

        // ── collisions: enemies vs player ─────────────────────────────────────
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

        // ── particles ─────────────────────────────────────────────────────────
        for (auto& p : particles) {
            p.life -= dt;
            if (p.life <= 0) { p.alive = false; continue; }
            p.shape.move(p.vx * dt, p.vy * dt);
            float a = p.life / p.maxLife;
            sf::Color c = p.shape.getFillColor();
            c.a = (sf::Uint8)(a * 255);
            p.shape.setFillColor(c);
        }

        // ── cleanup dead objects ──────────────────────────────────────────────
        auto rm = [](auto& v){ v.erase(std::remove_if(v.begin(), v.end(),
            [](const auto& x){ return !x.alive; }), v.end()); };
        rm(playerBullets);
        rm(enemyBullets);
        rm(enemies);
        rm(particles);

        // ── next wave? ────────────────────────────────────────────────────────
        if (formDone && enemies.empty()) {
            waveTimer += dt;
            if (waveTimer > 1.5f) { waveTimer = 0; spawnWave(); }
        }
    }

    void draw(sf::RenderWindow& win) {
        // background
        sf::Color bg = (flashTimer > 0)
            ? sf::Color(60, 10, 10)
            : sf::Color(5, 5, 18);
        win.clear(bg);

        for (auto& s : stars) win.draw(s);

        // enemies
        for (auto& e : enemies) {
            if (!e.alive) continue;
            e.shape.setPosition(e.x, e.y);
            win.draw(e.shape);
            // hp bar for 2-hp enemies
            if (e.hp == 2) {
                sf::RectangleShape bar({16,3});
                bar.setFillColor({255,255,0});
                bar.setPosition(e.x - 8, e.y - 20);
                win.draw(bar);
            }
        }

        // player bullets
        for (auto& b : playerBullets)
            win.draw(b.shape);

        // enemy bullets
        for (auto& b : enemyBullets)
            win.draw(b.shape);

        // particles
        for (auto& p : particles)
            win.draw(p.shape);

        // player ship (blink when invincible)
        bool showPlayer = !gameOver &&
            (invincible <= 0 || (int)(invincible * 8) % 2 == 0);
        if (showPlayer) {
            auto ship = makeShip(18.f, {100, 200, 255});
            ship.setPosition(px, py);
            win.draw(ship);
            // engine glow
            sf::CircleShape glow(5);
            glow.setOrigin(5,5);
            glow.setPosition(px, py + 18);
            glow.setFillColor({80, 140, 255, 120});
            win.draw(glow);
        }

        // lives as small ship icons (bottom-left)
        for (int i = 0; i < lives; i++) {
            auto icon = makeShip(9.f, {80, 160, 220});
            icon.setPosition(18 + i * 24.f, H - 18.f);
            win.draw(icon);
        }

        // wave indicator — dots bottom-right (no text)
        for (int i = 0; i < wave; i++) {
            sf::CircleShape dot(3);
            dot.setOrigin(3,3);
            dot.setPosition(W - 15 - i * 10.f, H - 15);
            dot.setFillColor({180, 180, 80});
            win.draw(dot);
        }
    }
};

// ── main ─────────────────────────────────────────────────────────────────────
int main() {
    std::srand((unsigned)std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode(W, H), "Galaga",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    Game game;
    game.init();

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Escape) window.close();
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;  // cap delta

        game.update(dt);
        game.draw(window);
        window.display();
    }
    return 0;
}
