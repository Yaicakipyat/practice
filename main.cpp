#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "Game.h"
#include "constants.h"

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
        if (dt > 0.05f) dt = 0.05f;

        game.update(dt);
        game.draw(window);
        window.display();
    }
    return 0;
}
