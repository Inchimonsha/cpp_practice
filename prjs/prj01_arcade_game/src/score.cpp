#include <arcade/score.hpp>

#include <cpp_course/log.hpp>

#include <charconv>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace arcade {

void Score::lose_life() {
    if (lives_ > 0) {
        --lives_;
    }
}

void Score::reset() {
    // Рекорд — максимум за всё время: перед сбросом учитываем текущий счёт.
    if (points_ > record_) {
        record_ = points_;
    }
    points_ = 0;
    lives_ = kStartLives;
}

bool Score::load_record(std::string_view path) {
    const std::string file(path);  // std::string_view не конструирует потоки напрямую
    std::ifstream in(file);
    if (!in) {
        return false;
    }
    int value = 0;
    in >> value;
    if (in.fail() || value < 0) {
        cpp_course::log().warn("record: некорректный файл");
        return false;
    }
    record_ = value;
    return true;
}

bool Score::save_record(std::string_view path) const {
    const std::string file(path);
    std::ofstream out(file);
    if (!out) {
        return false;
    }
    out << record_;
    return static_cast<bool>(out);
}

}  // namespace arcade