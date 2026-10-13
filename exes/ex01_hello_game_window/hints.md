# Подсказки: ex01 — Hello, Game Window

Подсказки упорядочены «по силе»: открывайте следующую только после того, как
застряли на соответствующем шаге.

## Уровень 1 — не знаете, с чего начать

- Прочитайте раздел 6 (Синтаксис) и код `hello.cpp` в `theory/th01/examples/`,
  чтобы убедиться, что окружение настроено.
- Изучите сигнатуры `sf::RenderWindow`, `sf::Clock`, `sf::Keyboard` в
  документации SFML 3.1.0 (раздел Window/System).

## Уровень 2 — события и цикл

- В SFML 3 события приходят как `std::variant`:
  `while (const std::optional event = window.pollEvent()) { ... }`.
  Тип события проверяется через `event->is<sf::Event::Closed>()`;
  данные — через `event->getIf<sf::Event::KeyPressed>()`.
- `dt` получается так: `const float dt = clock.restart().asSeconds();`
  вызов в начале итерации каждого кадра.

## Уровень 3 — движение и заголовок

- Движение зависит от времени:
  `player.move(velocity * speed * dt)` — здесь `velocity` — единичный вектор
  (`{-1,0}`, `{1,0}`, `{0,-1}`, `{0,1}` и комбинации).
- Заголовок: `window.setTitle("FPS: " + std::to_string(int(fps_value)))`,
  где `fps_value` — возврат `FpsCounter::update(dt)`.
- Если FPS «прыгает» — сгладьте (в каркасе уже есть EMA).