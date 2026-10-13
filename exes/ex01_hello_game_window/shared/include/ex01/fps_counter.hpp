#pragma once

// FpsCounter — сглаженный счётчик FPS (кадров в секунду).
//
// Пример использования в игровом цикле:
//
//     FpsCounter fps;
//     while (window.isOpen()) {
//         const float dt = clock.restart().asSeconds();
//         const float value = fps.update(dt);   // сглаженный FPS
//         window.setTitle("FPS: " + std::to_string(static_cast<int>(value)));
//     }
//
// Логика вынесена из main.cpp намеренно: её можно проверить unit-тестами без
// создания окна (см. tests/test_fps_counter.cpp).

namespace ex01 {

class FpsCounter {
public:
    // Обновляет счётчик новым кадром и возвращает сглаженное значение FPS.
    // dt_seconds — время, прошедшее с предыдущего кадра, в секундах.
    float update(float dt_seconds);

    // Текущее сглаженное значение FPS (без обновления).
    float fps() const { return smoothed_fps_; }

private:
    float smoothed_fps_ = 0.0f;
};

}  // namespace ex01