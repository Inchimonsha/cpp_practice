// ex01 — опциональный smoke-тест окна.
//
// Создаёт окно, выполняет несколько кадров цикла, затем закрывает окно и
// выходит с кодом 0. Требует интерактивной сессии (окно на экране),
// поэтому регистрируется в CTest только при
//     -DCPP_COURSE_RUN_WINDOW_TESTS=ON
// (по умолчанию OFF — см. exes/ex01/CMakeLists.txt).
//
// В CI без дисплея окно не создаётся — тест пропускается. Это намеренно:
// GUI-тесты в CI не должны «висеть» (обсуждение — в th49_testing).

#include <SFML/Graphics.hpp>

#include <optional>

int main() {
    constexpr unsigned kWidth = 320u;
    constexpr unsigned kHeight = 240u;
    constexpr int kFramesToRun = 10;

    sf::RenderWindow window(sf::VideoMode({kWidth, kHeight}), "ex01 smoke");

    sf::CircleShape dot(10.0f);
    dot.setFillColor(sf::Color::Cyan);

    int frames = 0;
    while (window.isOpen() && frames < kFramesToRun) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        window.clear(sf::Color::Black);
        window.draw(dot);
        window.display();
        ++frames;
    }

    // Цикл завершился штатно (не «завис») — окно закрываем явно.
    window.close();
    return 0;
}