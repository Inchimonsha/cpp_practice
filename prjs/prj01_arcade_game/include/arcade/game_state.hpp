#pragma once

namespace arcade {

// Состояния игры (переходы — см. design/screens.md).
enum class GameState {
    playing,
    game_over,
};

}  // namespace arcade