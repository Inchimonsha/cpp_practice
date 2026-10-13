#pragma once

// Тонкий связующий слой: окно + игровой цикл + рендер (SFML).
// Всю игровую логику держит модель (Player, FallingObject, Score, Aabb),
// которая не знает о SFML.

#include <SFML/Graphics.hpp>

#include <arcade/falling_object.hpp>
#include <arcade/game_state.hpp>
#include <arcade/player.hpp>
#include <arcade/score.hpp>

#include <random>
#include <vector>

namespace arcade {

class Game {
public:
    Game();

    // Запускает игровой цикл; завершается при закрытии окна.
    void run();

private:
    static constexpr unsigned kWidth = 800u;
    static constexpr unsigned kHeight = 600u;
    static constexpr int kGoodPoints = 10;

    void handle_event(const sf::Event& event);
    void spawn_object();
    void update(float dt);
    void update_player(float dt);
    void check_collisions();
    void render();
    void sync_record();   // обновляет рекорд и сохраняет его на диск
    void restart();       // новая игра (счет, предметы, состояние)

    sf::RenderWindow window_;

    GameState state_ = GameState::playing;
    Player player_;
    std::vector<FallingObject> objects_;
    Score score_;
    std::mt19937 rng_;

    float spawn_timer_ = 0.0f;
    float spawn_interval_ = 1.0f;

    // Кэш для заголовка окна: строка не пересоздаётся на каждом кадре.
    int title_score_ = -1;
    int title_lives_ = -1;
};

}  // namespace arcade