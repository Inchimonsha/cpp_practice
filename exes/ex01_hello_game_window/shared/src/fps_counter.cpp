#include <ex01/fps_counter.hpp>

namespace ex01 {

// Экспоненциальное сглаживание (EMA): новый FPS смешивается с предыдущим
// значением. Коэффициент alpha определяет, насколько быстро счётчик
// реагирует на изменение FPS. Чем меньше alpha, тем плавнее показатель.
namespace {
constexpr float kAlpha = 0.1f;
}

float FpsCounter::update(float dt_seconds) {
    // Защита от деления на ноль (dt == 0 у первого кадра).
    const float instant_fps = dt_seconds > 0.0f ? 1.0f / dt_seconds : 0.0f;
    if (smoothed_fps_ == 0.0f) {
        // Первый кадр — принимаем мгновенное значение целиком.
        smoothed_fps_ = instant_fps;
    } else {
        smoothed_fps_ = kAlpha * instant_fps + (1.0f - kAlpha) * smoothed_fps_;
    }
    return smoothed_fps_;
}

}  // namespace ex01