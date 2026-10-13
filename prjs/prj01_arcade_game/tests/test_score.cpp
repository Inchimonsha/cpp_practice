// Тесты: счёт, жизни, рекорд (без окна, запускаются в CTest).

#include <arcade/score.hpp>

#include <cpp_course/test.hpp>

#include <filesystem>
#include <fstream>
#include <string>

using arcade::Score;

CPP_COURSE_TEST("Score: стартовые значения") {
    Score s;
    CHECK_EQ(s.points(), 0);
    CHECK_EQ(s.lives(), Score::kStartLives);
    CHECK(s.is_alive());
    CHECK_EQ(s.record(), 0);
}

CPP_COURSE_TEST("Score: добавление очков") {
    Score s;
    s.add_points(10);
    s.add_points(5);
    CHECK_EQ(s.points(), 15);
}

CPP_COURSE_TEST("Score: потеря жизней и game over") {
    Score s;
    for (int i = 0; i < Score::kStartLives; ++i) {
        CHECK(s.is_alive());
        s.lose_life();
    }
    CHECK(!s.is_alive());
}

CPP_COURSE_TEST("Score: lose_life не уходит в минус") {
    Score s;
    for (int i = 0; i < 10; ++i) {
        s.lose_life();
    }
    CHECK_EQ(s.lives(), 0);
}

CPP_COURSE_TEST("Score: reset сохраняет рекорд") {
    Score s;
    s.add_points(100);
    s.reset();
    CHECK_EQ(s.points(), 0);
    CHECK_EQ(s.record(), 100);
    CHECK_EQ(s.lives(), Score::kStartLives);
}

CPP_COURSE_TEST("Score: рекорд выше после нескольких прохождений") {
    Score s;
    s.add_points(100);
    s.reset();              // конец 1-го прохождения: record = 100
    s.add_points(250);
    s.reset();              // конец 2-го прохождения: record = 250
    CHECK_EQ(s.record(), 250);
    CHECK_EQ(s.points(), 0);
    CHECK_EQ(s.lives(), Score::kStartLives);
}

CPP_COURSE_TEST("Score: save/load рекорда") {
    const auto path =
        (std::filesystem::temp_directory_path() / "cpp_course_prj01_record.txt");
    std::filesystem::remove(path);

    Score a;
    a.add_points(42);
    a.reset();  // record = 42
    CHECK(a.save_record(path.string()));

    Score b;
    CHECK(b.load_record(path.string()));
    CHECK_EQ(b.record(), 42);

    std::filesystem::remove(path);
}

CPP_COURSE_TEST("Score: load отсутствующего файла вернёт false") {
    Score s;
    const auto path =
        (std::filesystem::temp_directory_path() / "cpp_course_no_such_record.txt");
    std::filesystem::remove(path);
    CHECK(!s.load_record(path.string()));
}

CPP_COURSE_TEST("Score: load повреждённого файла вернёт false") {
    const auto path =
        (std::filesystem::temp_directory_path() / "cpp_course_bad_record.txt");
    {
        std::ofstream out(path);
        out << "abc";
    }
    Score s;
    CHECK(!s.load_record(path.string()));
    CHECK_EQ(s.record(), 0);
    std::filesystem::remove(path);
}