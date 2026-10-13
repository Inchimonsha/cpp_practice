// Тесты логики FpsCounter — без графики, выполняются в CI.

#include <ex01/fps_counter.hpp>

#include <cpp_course/test.hpp>

#include <cmath>

using ex01::FpsCounter;

CPP_COURSE_TEST("FpsCounter: первый кадр возвращает мгновенное значение") {
    FpsCounter fps;
    // dt = 1/60 с ≈ 16.67 мс  →  ~60 FPS
    const float f = fps.update(1.0f / 60.0f);
    CHECK(std::abs(f - 60.0f) < 1.0f);
}

CPP_COURSE_TEST("FpsCounter: стационарный dt сходится к значению") {
    FpsCounter fps;
    constexpr int kFrames = 300;
    for (int i = 0; i < kFrames; ++i) {
        fps.update(1.0f / 60.0f);
    }
    CHECK(std::abs(fps.fps() - 60.0f) < 1.0f);
}

CPP_COURSE_TEST("FpsCounter: реакция на смену dt") {
    FpsCounter fps;
    // Начали с 60 FPS...
    for (int i = 0; i < 200; ++i) {
        fps.update(1.0f / 60.0f);
    }
    const float before = fps.fps();
    // ...резко сменили на 30 FPS: значение должно сместиться вниз.
    for (int i = 0; i < 60; ++i) {
        fps.update(1.0f / 30.0f);
    }
    const float after = fps.fps();
    CHECK(after < before);
    CHECK(after > 0.0f);
}

CPP_COURSE_TEST("FpsCounter: нулевой dt не приводит к бесконечности") {
    FpsCounter fps;
    const float f = fps.update(0.0f);
    CHECK(std::isfinite(f));
    CHECK(f >= 0.0f);
}

CPP_COURSE_TEST("FpsCounter: значения конечны и неотрицательны") {
    FpsCounter fps;
    for (int i = 0; i < 100; ++i) {
        const float f = fps.update(1.0f / 60.0f);
        CHECK(std::isfinite(f));
        CHECK(f >= 0.0f);
    }
}