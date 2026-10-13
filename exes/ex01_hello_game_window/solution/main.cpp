// ex01 — Hello, Game Window: эталонное решение.
// SFML 3.1.0, C++20. Это одно из возможных решений.
//
// Структура цикла: события → обновление (с dt) → отрисовка → display.

#include <ex01/fps_counter.hpp>

#include <SFML/Graphics.hpp>

#include <string>

namespace {

constexpr unsigned kWindowWidth = 800u;
constexpr unsigned kWindowHeight = 600u;
constexpr float kMoveSpeed = 200.0f;  // пикселей в секунду

const sf::Color kBackgroundColor{30, 30, 30};
const sf::Color kPlayerColor = sf::Color::Green;

}  // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowHeight}),
                            "Hello, Game Window!");
    window.setFramerateLimit(60);

    sf::RectangleShape player({50.0f, 50.0f});
    player.setFillColor(kPlayerColor);
    player.setPosition({kWindowWidth / 2.0f, kWindowHeight / 2.0f});

    sf::Clock clock;
    ex01::FpsCounter fps;

    while (window.isOpen()) {
        // --- События -------------------------------------------------------
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // --- Обновление ----------------------------------------------------
        const float dt = clock.restart().asSeconds();
        const float fps_value = fps.update(dt);

        sf::Vector2f velocity{0.0f, 0.0f};
        const auto key_down = [](sf::Keyboard::Key key) {
            return sf::Keyboard::isKeyPressed(key);
        };
        if (key_down(sf::Keyboard::Key::Left))  velocity.x -= 1.0f;
        if (key_down(sf::Keyboard::Key::Right)) velocity.x += 1.0f;
        if (key_down(sf::Keyboard::Key::Up))    velocity.y -= 1.0f;
        if (key_down(sf::Keyboard::Key::Down))  velocity.y += 1.0f;

        player.move(velocity * kMoveSpeed * dt);

        // --- Заголовок с FPS ------------------------------------------------
        window.setTitle("Hello, Game Window!  FPS: "
                        + std::to_string(static_cast<int>(fps_value)));

        // --- Отрисовка -----------------------------------------------------
        window.clear(kBackgroundColor);
        window.draw(player);
        window.display();
    }
    return 0;
}