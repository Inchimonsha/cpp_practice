# Шаблон теоретической темы

Как создать новую тему `theory/thNN_topic_name`:

1. Скопируйте каталог `tools/templates/theory/` в `theory/thNN_topic_name/`.
2. Заполните `README.md`, `theory.md`, `cheatsheet.md`, `pitfalls.md`,
   `sources.md`.
3. Примеры из `examples/` должны:
   - компилироваться с C++20 (укажите команду в README темы);
   - быть либо полными (готовая программа), либо явно помеченными как фрагменты.
4. Проверьте структуру:

   ```bash
   python tools/validators/check_structure.py
   ```

5. Добавьте тему в `ROADMAP.md`, `CHECKLIST.md` и `CHANGELOG.md`.

## Нумерация

`thNN_` — NN двухзначный, уникальный, без пропусков. `topic_name` — короткое
описание латиницей в `snake_case` (пример: `cpp_project_setup`).

## Связанность

Каждая тема обязана ссылаться минимум на одно задание (`exNN_*`) и один проект
(`prjNN_*`) из «Связанных» разделов `theory.md`/`README.md`. Если связи ещё нет —
раздел в план не включается (см. AGENTS.md, правило «ни пустых разделов»).