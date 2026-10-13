#pragma once

// Счёт игры: очки, жизни, рекорд. Чистая логика — не зависит от SFML,
// тестируется без окна (tests/test_score.cpp).

#include <string_view>

namespace arcade {

class Score {
public:
    static constexpr int kStartLives = 3;

    Score() = default;

    // Очки.
    void add_points(int points) { points_ += points; }
    int points() const { return points_; }

    // Жизни и состояние игры.
    int lives() const { return lives_; }
    bool is_alive() const { return lives_ > 0; }
    void lose_life();       // уменьшает жизни (но не ниже нуля)
    void reset();           // сбрасывает счёт игры; рекорд сохраняется

    // Рекорд.
    int record() const { return record_; }
    void set_record(int value) { record_ = value; }
    bool load_record(std::string_view path);  // false при ошибке/повреждении
    bool save_record(std::string_view path) const;

private:
    int points_ = 0;
    int lives_ = kStartLives;
    int record_ = 0;
};

}  // namespace arcade