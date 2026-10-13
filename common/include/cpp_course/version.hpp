#pragma once

// Версия курса. Используется в демо и диагностических сообщениях.

#define CPP_COURSE_VERSION_MAJOR 0
#define CPP_COURSE_VERSION_MINOR 1
#define CPP_COURSE_VERSION_PATCH 0

namespace cpp_course {

inline constexpr int version_major = CPP_COURSE_VERSION_MAJOR;
inline constexpr int version_minor = CPP_COURSE_VERSION_MINOR;
inline constexpr int version_patch = CPP_COURSE_VERSION_PATCH;

// Человекочитаемая строка версии: "0.1.0".
inline constexpr char version_string[] = "0.1.0";

}  // namespace cpp_course