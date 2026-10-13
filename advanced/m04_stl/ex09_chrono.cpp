// m04_stl/ex09_chrono — std::chrono.
// Standart: C++23. Zapusk: ./run.ps1 m04_stl/ex09_chrono
#include <chrono>
#include <iostream>
#include <print>

int main() {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    const milliseconds total = 1500ms + 500ms;
    const seconds sec = duration_cast<seconds>(total);
    std::println("total = {} ms = {} s", total.count(), sec.count());

    const auto start = steady_clock::now();
    volatile int sink = 0;
    for (int i = 0; i < 1000; ++i) { sink += i; }
    const auto elapsed = steady_clock::now() - start;
    (void)sink;
    std::println("elapsed >= 0? {} ({} us)",
                 elapsed >= nanoseconds::zero(),
                 duration_cast<microseconds>(elapsed).count());

    const year_month_day date{year{2026}, March, day{12}};
    std::println("date = {:04d}-{:02d}-{:02d}",
                 static_cast<int>(date.year()),
                 static_cast<unsigned>(date.month()),
                 static_cast<unsigned>(date.day()));

    const weekday wd{sys_days{date}};
    std::println("weekday c_encoding = {} (0=Sun..6=Sat)", static_cast<unsigned>(wd.c_encoding()));

    using frames_60fps = duration<int, std::ratio<1, 60>>;   // 1/60 s
    const frames_60fps frame{5};
    const milliseconds frame_ms = duration_cast<milliseconds>(frame);
    std::println("5 frames @60fps = {} ms", frame_ms.count());

    std::cout << "chrono done\n";
    return (sec.count() == 2 && date.year() == year{2026} && frame_ms.count() == 83) ? 0 : 1;
}
