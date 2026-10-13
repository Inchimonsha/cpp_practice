// ex01 — Hello, Game Window: каркас (starter).
// Заполните TODO, следуя task.md. Подсказки — в hints.md.
//
// SFML 3 API: события приходят как std::variant; проверяйте их через
// event->is<T>() и event->getIf<T>().

#include <ex01/fps_counter.hpp>

#include <SFML/Graphics.hpp>

#include <string>

namespace {

constexpr unsigned kWindowWidth = 800u;
constexpr unsigned kWindowHeight = 600u;

// Скорость движения прямоугольника в пикселях в секунду.
constexpr float kMoveSpeed = 200.0f;

const sf::Color kBackgroundColor{30, 30, 30};
const sf::Color kPlayerColor = sf::Color::Green;

}  // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowHeight}),
                            "Hello, Game Window!");
    window.setFramerateLimit(60);

    // Игровой прямоугольник.
    sf::RectangleShape player({50.0f, 50.0f});
    player.setFillColor(kPlayerColor);
    player.setPosition({kWindowWidth / 2.0f, kWindowHeight / 2.0f});

    sf::Clock clock;
    ex01::FpsCounter fps;

    while (window.isOpen()) {
        // TODO 1: обработать события (закрытие окна; Esc — закрыть окно).
        // Подсказка: while (const std::optional event = window.pollEvent()).
        //        if (event->is<sf::Event::Closed>()) window.close();

        // TODO 2: вычислить dt в секундах (sf::Clock::restart) и обновить FpsCounter.

        // TODO 3: движение прямоугольника стрелками с учётом dt.
        // sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) и т.д.

        // TODO 4: заголовок окна с текущим FPS.
        // window.setTitle("FPS: " + std::to_string(...));

        window.clear(kBackgroundColor);
        window.draw(player);
        window.display();
    }
    return 0;
}