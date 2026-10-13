#include <arcade/game.hpp>

#include <cpp_course/log.hpp>

#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace arcade {

namespace {

// Каталог исполняемого файла — от него строятся пути к данным (в т.ч. рекорду),
// чтобы поведение не зависело от текущей рабочей директории.
std::filesystem::path executable_dir() {
#ifdef _WIN32
    wchar_t buffer[2048];
    const DWORD len = GetModuleFileNameW(nullptr, buffer, sizeof(buffer) / sizeof(buffer[0]));
    return std::filesystem::path(std::wstring_view(buffer, len)).parent_path();
#else
    return std::filesystem::canonical("/proc/self/exe").parent_path();
#endif
}

}  // namespace

Game::Game()
    : window_(sf::VideoMode({kWidth, kHeight}), "Arcade Game"),
      player_(Aabb{370.0f, 540.0f, 60.0f, 20.0f}),
      rng_(std::random_device{}()) {
    window_.setFramerateLimit(60);

    cpp_course::log().set_prefix("prj01");
    score_.load_record((executable_dir() / "record.txt").string());
    if (score_.record() > 0) {
        cpp_course::log().info("рекорд загружен: " + std::to_string(score_.record()));
    }
}

void Game::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        while (const std::optional event = window_.pollEvent()) {
            handle_event(*event);
        }
        update(clock.restart().asSeconds());
        render();
    }
    sync_record();
}

void Game::handle_event(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window_.close();
        return;
    }
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            window_.close();
        } else if (key->code == sf::Keyboard::Key::R &&
                   state_ == GameState::game_over) {
            restart();
        }
    }
}

void Game::spawn_object() {
    std::uniform_real_distribution<float> x_roll(20.0f, kWidth - 50.0f);
    std::uniform_int_distribution<int> kind_roll(0, 3);  // 3:1 полезные/вредные
    std::uniform_real_distribution<float> speed_roll(120.0f, 220.0f);

    const auto kind = kind_roll(rng_) == 0 ? ObjectKind::bad : ObjectKind::good;
    objects_.emplace_back(Aabb{x_roll(rng_), -30.0f, 30.0f, 30.0f}, kind,
                          speed_roll(rng_));
}

void Game::update(float dt) {
    if (state_ != GameState::playing) {
        return;
    }

    // Таймер спавна.
    spawn_timer_ -= dt;
    if (spawn_timer_ <= 0.0f) {
        spawn_object();
        spawn_timer_ += spawn_interval_;
    }

    update_player(dt);

    // Движение предметов и удаление ушедших за нижний край.
    objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
                                  [dt](FallingObject& obj) {
                                      obj.update(dt);
                                      return obj.bounds().y > kHeight + 30.0f;
                                  }),
                   objects_.end());

    check_collisions();

    if (!score_.is_alive()) {
        state_ = GameState::game_over;
        cpp_course::log().info("game over: " + std::to_string(score_.points()));
        sync_record();  // фиксируем рекорд сразу после проигрыша
    }
}

void Game::update_player(float dt) {
    const auto key_down = [](sf::Keyboard::Key key) {
        return sf::Keyboard::isKeyPressed(key);
    };
    if (key_down(sf::Keyboard::Key::Left)) {
        player_.move_left(dt);
    }
    if (key_down(sf::Keyboard::Key::Right)) {
        player_.move_right(dt);
    }
    player_.clamp_to_field(0.0f, static_cast<float>(kWidth));
}

void Game::check_collisions() {
    for (auto it = objects_.begin(); it != objects_.end();) {
        const bool hit = player_.bounds().intersects(it->bounds());
        if (!hit) {
            ++it;
            continue;
        }
        if (it->kind() == ObjectKind::good) {
            score_.add_points(kGoodPoints);
        } else {
            score_.lose_life();
        }
        it = objects_.erase(it);
    }

    if (score_.points() > score_.record()) {
        score_.set_record(score_.points());
    }
}

void Game::render() {
    window_.clear(sf::Color{30, 30, 30});

    sf::RectangleShape player_rect({player_.bounds().w, player_.bounds().h});
    player_rect.setPosition({player_.bounds().x, player_.bounds().y});
    player_rect.setFillColor(sf::Color::Green);
    window_.draw(player_rect);

    for (const auto& obj : objects_) {
        sf::RectangleShape rect({obj.bounds().w, obj.bounds().h});
        rect.setPosition({obj.bounds().x, obj.bounds().y});
        rect.setFillColor(obj.kind() == ObjectKind::good ? sf::Color::Green
                                                         : sf::Color::Red);
        window_.draw(rect);
    }

    // HUD в заголовке окна (текст на экране появится с наличием шрифта, M3).
    if (state_ == GameState::game_over) {
        if (title_lives_ != -2) {  // «-2» — маркер screen game over, показать один раз
            window_.setTitle("Arcade — Game Over! Score: "
                             + std::to_string(score_.points())
                             + "  |  Record: " + std::to_string(score_.record()));
            title_lives_ = -2;
        }
    } else if (score_.points() != title_score_ || score_.lives() != title_lives_) {
        title_score_ = score_.points();
        title_lives_ = score_.lives();
        window_.setTitle("Arcade | Score: " + std::to_string(title_score_)
                         + " | Lives: " + std::to_string(title_lives_));
    }

    window_.display();
}

void Game::sync_record() {
    if (score_.points() > score_.record()) {
        score_.set_record(score_.points());
    }
    if (!score_.save_record((executable_dir() / "record.txt").string())) {
        cpp_course::log().warn("не удалось сохранить рекорд");
    }
}

void Game::restart() {
    cpp_course::log().info("новое прохождение");
    score_.reset();
    objects_.clear();
    player_.set_bounds(Aabb{370.0f, 540.0f, 60.0f, 20.0f});
    spawn_timer_ = 0.0f;
    state_ = GameState::playing;
    title_score_ = -1;
    title_lives_ = -1;
}

}  // namespace arcade