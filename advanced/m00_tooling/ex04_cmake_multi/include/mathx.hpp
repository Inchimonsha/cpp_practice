#pragma once

#include <cstdint>

namespace mathx {

// Obshchij delitel' (algoritm Evklida). Vozvrashchaet polozhitel'noe znachenie.
[[nodiscard]] std::int64_t gcd(std::int64_t a, std::int64_t b);

[[nodiscard]] std::int64_t add(std::int64_t a, std::int64_t b);

[[nodiscard]] std::int64_t mul(std::int64_t a, std::int64_t b);

}  // namespace mathx
