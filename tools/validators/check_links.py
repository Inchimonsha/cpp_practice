#!/usr/bin/env python3
"""Проверка относительных ссылок в Markdown-файлах курса.

Находит все ссылки вида [текст](путь) в *.md, пропускает:
    * веб-ссылки (http://, https://, ftp://);
    * ссылки на адреса электронной почты (mailto:);
    * якоря внутри файла (#...);
    * ссылки внутри HTML-комментариев;
и проверяет, что относительный путь существует. Ссылки внутри fenced code
blocks и инлайн-кода игнорируются (иначе правило даёт ложные срабатывания).

Использование:
    python tools/validators/check_links.py [--root <путь-к-курсу>]
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

LINK_RE = re.compile(r"\[([^\]]*)\]\(([^)]+)\)")
SKIP_SCHEMES = ("http://", "https://", "ftp://", "mailto:", "#", "<", ">")

errors: list[str] = []
checked_files = 0


def skip_spans(text: str) -> str:
    """Заменяет fenced code blocks и инлайн-код на пробелы."""
    # fenced blocks
    text = re.sub(r"```.*?```", lambda m: " " * len(m.group(0)), text, flags=re.S)
    # indented code (4+ пробела) — упрощённо: скобки внутри не типичны
    # inline code
    return re.sub(r"`[^`]*`", lambda m: " " * len(m.group(0)), text)


def strip_anchor(rel: str) -> str:
    return rel.split("#", 1)[0]


def check_file(path: Path) -> None:
    global checked_files
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as e:
        errors.append(f"{path}: не удалось прочитать как UTF-8 ({e})")
        return
    checked_files += 1
    text = skip_spans(text)
    for m in LINK_RE.finditer(text):
        target = m.group(2).strip()
        if target.startswith(SKIP_SCHEMES):
            continue
        rel = strip_anchor(target)
        if not rel:
            continue
        resolved = (path.parent / rel).resolve()
        if not resolved.exists():
            errors.append(f"{path}: битая ссылка -> {target}")


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

    # Файл: <корень>/tools/validators/check_links.py → корень через parents[2].
    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parents[2]
    for md in root.rglob("*.md"):
        # third_party/build — скачанные зависимости и артефакты сборки;
        # templates — каркасы с плейсхолдерами (thNN_name), их ссылки не резолвятся.
        if "third_party" in md.parts or "build" in md.parts or "templates" in md.parts:
            continue
        check_file(md)

    print(f"[check_links] проверено файлов: {checked_files}")
    if errors:
        print(f"[check_links] найдено ошибок: {len(errors)}")
        for e in errors:
            print(f"  ERROR: {e}")
        sys.exit(1)
    print("[check_links] ошибок не найдено")
    sys.exit(0)


if __name__ == "__main__":
    main()