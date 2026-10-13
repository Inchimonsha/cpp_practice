// Тесты логики журнала ex04. Проверяют контракт shared/event_log.hpp.

#include <ex04/event_log.hpp>

#include <cpp_course/test.hpp>

using namespace ex04;

#include <cmath>

CPP_COURSE_TEST("starts_with: базовые случаи") {
    CHECK(starts_with("Kill Monster", "Kill"));
    CHECK(starts_with("Kill Monster", ""));
    CHECK(starts_with("", ""));
    CHECK(!starts_with("Monster", "Kill"));
    CHECK(!starts_with("Ki", "Kill"));
}

CPP_COURSE_TEST("starts_with: точное совпадение — это префикс") {
    CHECK(starts_with("Pickup", "Pickup"));
}

CPP_COURSE_TEST("push_event: добавляет в конец") {
    std::vector<std::string> log;
    push_event(log, "level up");
    push_event(log, "pickup coin");
    CHECK_EQ(log.size(), static_cast<std::size_t>(2));
    CHECK_EQ(log[0], "level up");
    CHECK_EQ(log[1], "pickup coin");
}

CPP_COURSE_TEST("count_matching: ноль для пустого журнала") {
    std::vector<std::string> empty;
    CHECK_EQ(count_matching(empty, "x"), static_cast<std::size_t>(0));
}

CPP_COURSE_TEST("count_matching: считает события с фрагментом") {
    std::vector<std::string> log{"Kill Goblin", "Pickup Coin", "Kill Boss"};
    CHECK_EQ(count_matching(log, "Kill"), static_cast<std::size_t>(2));
    CHECK_EQ(count_matching(log, "Coin"), static_cast<std::size_t>(1));
    CHECK_EQ(count_matching(log, "absent"), static_cast<std::size_t>(0));
}

CPP_COURSE_TEST("count_matching: регистр учитывается") {
    std::vector<std::string> log{"kill", "Kill"};
    CHECK_EQ(count_matching(log, "kill"), static_cast<std::size_t>(1));
}

CPP_COURSE_TEST("filter_prefix: сохраняет порядок и фильтрует") {
    std::vector<std::string> log{"Kill Goblin", "Pickup Coin", "Kill Boss"};
    const auto filtered = filter_prefix(log, "Kill");
    CHECK_EQ(filtered.size(), static_cast<std::size_t>(2));
    CHECK_EQ(filtered[0], "Kill Goblin");
    CHECK_EQ(filtered[1], "Kill Boss");
}

CPP_COURSE_TEST("filter_prefix: пустой префикс — весь журнал") {
    std::vector<std::string> log{"a", "bb"};
    const auto filtered = filter_prefix(log, "");
    CHECK_EQ(filtered.size(), static_cast<std::size_t>(2));
}

CPP_COURSE_TEST("filter_prefix: несовпадения дают пустой результат") {
    std::vector<std::string> log{"Kill", "Pickup"};
    const auto filtered = filter_prefix(log, "Drop");
    CHECK(filtered.empty());
}

CPP_COURSE_TEST("average_length: пустой журнал → 0") {
    std::vector<std::string> empty;
    CHECK_EQ(average_length(empty), 0.0);
}

CPP_COURSE_TEST("average_length: среднее по размерам") {
    std::vector<std::string> log{"ab", "abcd"};  // 2 и 4
    const double avg = average_length(log);
    CHECK(std::abs(avg - 3.0) < 1e-9);
}

CPP_COURSE_TEST("integration: add + count + filter + stats") {
    std::vector<std::string> log;
    push_event(log, "Kill Monster");
    push_event(log, "Pickup Coin");
    push_event(log, "Kill Boss");

    CHECK_EQ(log.size(), static_cast<std::size_t>(3));
    CHECK_EQ(count_matching(log, "Monster"), static_cast<std::size_t>(1));
    CHECK_EQ(filter_prefix(log, "Kill").size(), static_cast<std::size_t>(2));
    // 12 + 11 + 9 = 32; 32 / 3 = 10.666...
    CHECK(std::abs(average_length(log) - (32.0 / 3.0)) < 1e-9);
}