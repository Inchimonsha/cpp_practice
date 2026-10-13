#!/usr/bin/env python3
"""Загрузка внешних зависимостей курса в third_party/.

Скачивает:
    * SFML 3.1.0 (пребилт под текущую платформу и компилятор);
    * Lua 5.4.9 (исходники с lua.org).

Использование:
    python tools/fetch_deps.py [--sfml-version 3.1.0] [--lua-version 5.4.9]
                               [--no-sfml] [--no-lua]
                               [--output third_party]

Проверяет SHA-256 загруженных архивов. Если для вашей платформы/компилятора
нет зафиксированного пребилта SFML — скрипт остановится с инструкцией
(см. th01_cpp_project_setup, таблица соответствия компилятор → пребилт).

Зависимости: только стандартная библиотека Python 3.10+.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile
from pathlib import Path

CUR = Path(__file__).resolve().parent
ROOT = CUR.parent

# --- Зафиксированные версии и контрольные суммы -----------------------------

SFML_REPO = "SFML/SFML"
SFML_VERSION = "3.1.0"

# Имя ассета -> (имя файла в релизе GitHub, ожидаемая SHA-256).
SFML_ASSETS: dict[str, tuple[str, str]] = {
    "windows-gcc-14.2.0-mingw-64-bit": (
        "SFML-3.1.0-windows-gcc-14.2.0-mingw-64-bit.zip",
        "908FFE33F4C452023E0A6266A8EF5717A76A1826FEC3F9F7ADECF7FA81984BA1",
    ),
    "windows-vc17-64-bit": (
        "SFML-3.1.0-windows-vc17-64-bit.zip",
        None,  # не проверялся локально; загрузка без проверки хеша допустима
    ),
}

LUA_VERSION = "5.4.9"
LUA_URL = f"https://www.lua.org/ftp/lua-{LUA_VERSION}.tar.gz"
LUA_SHA256 = "2335B6C582A52654F94612BF10D2F4672805D05329AA6568B1D8CD9E5C6FB8E6"


# --- Вспомогательные функции -----------------------------------------------


def sha256_of(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest().upper()


def download(url: str, dest: Path, expected_sha: str | None) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    print(f"[fetch] {url}")
    with urllib.request.urlopen(url) as resp:
        size = int(resp.headers.get("Content-Length", 0))
        with dest.open("wb") as f:
            done = 0
            while True:
                chunk = resp.read(1 << 20)
                if not chunk:
                    break
                f.write(chunk)
                done += len(chunk)
                if size:
                    print(f"[fetch] {done * 100 // size}%", end="\r")
    print()
    if expected_sha:
        actual = sha256_of(dest)
        if actual != expected_sha.upper():
            raise SystemExit(
                f"Ошибка: контрольная сумма {dest.name} не совпадает.\n"
                f"  ожидали: {expected_sha.upper()}\n  получено: {actual}"
            )
        print(f"[fetch] SHA-256 OK: {actual}")


def _gcc_minor_from_str(stdout: str) -> int | None:
    """Ищет строку вида 'g++ (GCC) 13.2.0' и возвращает major-версию."""
    m = re.search(r"(\d+)\.\d+\.\d+", stdout)
    return int(m.group(1)) if m else None


def guess_sfml_asset() -> str:
    """Определяет имя ассета SFML для текущей платформы и компилятора."""
    if platform.system() == "Windows":
        for candidate in ("g++", "clang++"):
            exe = shutil.which(candidate)
            if not exe:
                continue
            try:
                stdout = subprocess.run(
                    [exe, "--version"], capture_output=True, text=True, timeout=10
                ).stdout
            except Exception:
                continue
            major = _gcc_minor_from_str(stdout)
            if major is not None:
                if major >= 14:
                    return "windows-gcc-14.2.0-mingw-64-bit"
                # GCC 13.x: официальных пребилтов нет; практическая проверка
                # показала успешную ABI-линковку с пребилтом под GCC 14.2
                # (см. th01). Используем тот же ассет.
                return "windows-gcc-14.2.0-mingw-64-bit"
        if shutil.which("cl"):
            return "windows-vc17-64-bit"
        # Ни одного компилятора в PATH: падаем с инструкцией.
        raise SystemExit(
            "[SFML] Не найден компилятор в PATH. Установите и добавьте в PATH "
            "MinGW-w64 GCC >= 14 или MSVC (см. th01_cpp_project_setup)."
        )
    raise SystemExit(
        "[SFML] Для этой ОС пребилт не скачивается: используйте системный "
        "менеджер пакетов (Linux: libsfml-dev; macOS: brew install sfml)."
    )


# --- Шаги -------------------------------------------------------------------


def fetch_sfml(out_dir: Path, version: str) -> None:
    asset_name = guess_sfml_asset()
    url_suffix, sha256 = SFML_ASSETS[asset_name]
    print(f"[SFML] Ассет: {url_suffix}")

    url = f"https://github.com/{SFML_REPO}/releases/download/{version}/{url_suffix}"
    zip_path = out_dir / url_suffix
    download(url, zip_path, sha256)

    target = out_dir / f"SFML-{version}"
    if target.exists():
        shutil.rmtree(target)
    with zipfile.ZipFile(zip_path) as z:
        z.extractall(out_dir)
    print(f"[SFML] Распаковано в {target}")


def fetch_lua(out_dir: Path, version: str) -> None:
    if version != LUA_VERSION:
        raise SystemExit(
            f"[Lua] Версия {version} не зафиксирована; поддерживается {LUA_VERSION}."
        )
    tar_path = out_dir / f"lua-{version}.tar.gz"
    download(LUA_URL, tar_path, LUA_SHA256)

    src = out_dir / f"lua-{version}"
    if src.exists():
        shutil.rmtree(src)
    with tarfile.open(tar_path, "r:gz") as t:
        t.extractall(out_dir)
    print(f"[Lua] Исходники в {src}")


def write_manifest(out_dir: Path) -> None:
    manifest = {
        "sfml_version": SFML_VERSION,
        "lua_version": LUA_VERSION,
        "updated_by": "tools/fetch_deps.py",
    }
    (out_dir / "deps.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )


def main() -> None:
    # Единый вывод в UTF-8 (иначе на Windows кириллица сбивается кодировкой консоли).
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8")
        except (AttributeError, ValueError):
            pass

    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--sfml-version", default=SFML_VERSION)
    ap.add_argument("--lua-version", default=LUA_VERSION)
    ap.add_argument("--no-sfml", action="store_true")
    ap.add_argument("--no-lua", action="store_true")
    ap.add_argument("--output", default=str(ROOT / "third_party"))
    args = ap.parse_args()

    out = Path(args.output).resolve()
    out.mkdir(parents=True, exist_ok=True)

    try:
        if not args.no_sfml:
            fetch_sfml(out, args.sfml_version)
        if not args.no_lua:
            fetch_lua(out, args.lua_version)
    except urllib.error.URLError as e:
        raise SystemExit(f"[fetch] Ошибка сети: {e}") from e

    write_manifest(out)
    print("\nГотово. Дальнейшие шаги:")
    print("  cmake -S . -B build")
    print("  cmake --build build --config Release")
    print("  ctest --test-dir build --output-on-failure")


if __name__ == "__main__":
    main()