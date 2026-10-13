#!/usr/bin/env python3
"""Проверка обязательной структуры учебных разделов курса.

Проверяются:
    * обязательные файлы в theory/thNN_*, exes/exNN_*, prjs/prjNN_*;
    * обязательные README у секций theory/, exes/, prjs/, assets/, tools/;
    * уникальность и последовательность нумерации;
    * отсутствие пустых обязательных каталогов;
    * соответствие плану в ROADMAP.md.

Использование:
    python tools/validators/check_structure.py [--root <путь-к-курсу>]
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

# --- Обязательные файлы -----------------------------------------------------

THEORY_FILES = {
    "README.md": True,
    "theory.md": True,
    "examples/": "dir",
    "cheatsheet.md": True,
    "pitfalls.md": True,
    "sources.md": True,
}

EXERCISE_FILES = {
    "README.md": True,
    "task.md": True,
    "starter/": "dir",
    "solution/": "dir",
    "tests/": "dir",
    "hints.md": True,
    "review.md": True,
}

PROJECT_FILES = {
    "README.md": True,
    "brief.md": True,
    "requirements.md": True,
    "milestones.md": True,
    "architecture.md": True,
    "design/": "dir",
    "src/": "dir",
    "include/": "dir",
    "assets/": "dir",
    "tests/": "dir",
    "docs/": "dir",
    "CMakeLists.txt": True,
    "CHANGELOG.md": True,
}

SECTION_READMES = ["theory", "exes", "prjs", "assets", "tools"]

NORME = re.compile(r"^(th|ex|prj)(\d+)_")

errors: list[str] = []
warnings: list[str] = []


def report_dir(step: str, entry: Path, required: dict[str, object]) -> None:
    for name, kind in required.items():
        path = entry / name
        if kind == "dir":
            if not path.is_dir():
                errors.append(f"{step}: отсутствует каталог {name} в {entry.name}")
            elif not any(path.iterdir()):
                errors.append(f"{step}: каталог пуст: {path}")
        else:
            if not path.is_file():
                errors.append(f"{step}: отсутствует файл {name} в {entry.name}")
            elif path.stat().st_size == 0:
                errors.append(f"{step}: файл пуст: {path}")


def check_sequence(codes: list[tuple[str, int, Path, Path]]) -> None:
    seen: dict[int, Path] = {}
    for prefix, num, entry, _ in codes:
        if num in seen:
            errors.append(f"Дубликат номера: {prefix}{num:02d} в {seen[num]} и {entry}")
        seen[num] = entry
    nums = sorted(seen)
    if nums and nums[0] != 1:
        warnings.append(f"Нумерация начинается не с 01: {prefix}{nums[0]:02d}")
    for a, b in zip(nums, nums[1:]):
        if b - a > 1:
            warnings.append(f"Пропуск номера между {prefix}{a:02d} и {prefix}{b:02d}")


def find_by_prefix(root: Path, prefix: str) -> list[tuple[str, int, Path, Path]]:
    """Возвращает (prefix, number, dir, section_dir) для каталогов prefixNN_*."""
    result: list[tuple[str, int, Path, Path]] = []
    for section_dir in root.iterdir():
        if not section_dir.is_dir():
            continue
        for entry in section_dir.iterdir():
            if not entry.is_dir():
                continue
            m = NORME.match(entry.name)
            if m and m.group(1) == prefix:
                result.append((prefix, int(m.group(2)), entry, section_dir))
    return sorted(result, key=lambda t: t[1])


def check_against_roadmap(root: Path) -> None:
    """Сверка существующих разделов с планом в ROADMAP.md (информационно)."""
    roadmap = root / "ROADMAP.md"
    if not roadmap.is_file():
        warnings.append("ROADMAP.md отсутствует — сверка с планом пропущена")
        return
    text = roadmap.read_text(encoding="utf-8")

    def refs(prefix: str) -> set[str]:
        distinct: set[str] = set()
        # Код может встречаться с суффиксом ("th01_тема") или без ("th01").
        for m in re.finditer(rf"\b{prefix}\d{{2}}(?:_|\b)", text):
            distinct.add(m.group(0)[:5].rstrip("_"))
        return distinct

    for prefix in ("th", "ex", "prj"):
        for _, num, entry, _ in find_by_prefix(root, prefix):
            code = f"{prefix}{num:02d}"
            if code not in refs(prefix):
                warnings.append(
                    f"{entry} есть на диске, но '{code}' не упомянут в ROADMAP.md"
                )


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", default=None, help="Корень курса (по умолчанию — родитель tools/)")
    args = ap.parse_args()

    # Единый вывод в UTF-8 (иначе на Windows кириллица сбивается кодировкой консоли).
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8")
        except (AttributeError, ValueError):
            pass

    # Файл: <корень>/tools/validators/check_structure.py → корень через parents[2].
    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parents[2]

    # README обязательных секций
    for section in SECTION_READMES:
        readme = root / section / "README.md"
        if not readme.is_file():
            errors.append(f"Секция {section}/: отсутствует README.md")

    # Разделы
    for prefix, required, label in [
        ("th", THEORY_FILES, "тема"),
        ("ex", EXERCISE_FILES, "задание"),
        ("prj", PROJECT_FILES, "проект"),
    ]:
        items = find_by_prefix(root, prefix)
        if not items:
            warnings.append(f"Разделов {prefix}NN_* не найдено")
        for _, _, entry, _ in items:
            report_dir(f"Раздел '{entry.name}'", entry, required)
        check_sequence(items)

    check_against_roadmap(root)

    print(f"[check_structure] найдено ошибок: {len(errors)}")
    for e in errors:
        print(f"  ERROR: {e}")
    print(f"[check_structure] предупреждений: {len(warnings)}")
    for w in warnings:
        print(f"  WARN : {w}")

    sys.exit(1 if errors else 0)


if __name__ == "__main__":
    main()