#!/usr/bin/env bash
# Kompilyaciya i zapusk odnogo .cpp-fajla kursa advanced (Linux/macOS).
# Ispol'zovanie: ./run.sh m01_syntax/ex01_types_literals [--std c++23] [--no-run]
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
target="${1:-}"
if [[ -z "$target" ]]; then
    echo "usage: ./run.sh <path|name> [--std c++23] [--compiler g++] [--no-run] [--sanitize]" >&2
    exit 2
fi
shift || true

std="c++23"
compiler="${CXX:-g++}"
run=1
sanitize=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --std) std="$2"; shift 2 ;;
        --compiler) compiler="$2"; shift 2 ;;
        --no-run) run=0; shift ;;
        --sanitize) sanitize=1; shift ;;
        *) echo "unknown option: $1" >&2; exit 2 ;;
    esac
done

src="$target"
[[ -e "$src" ]] || src="$here/$target"
if [[ -d "$src" ]]; then
    src="$(find "$src" -maxdepth 1 -name '*.cpp' | head -n1)"
elif [[ ! -f "$src" && -f "$src.cpp" ]]; then
    src="$src.cpp"
fi
if [[ ! -f "$src" ]]; then
    echo "source not found: $target" >&2
    exit 1
fi

mkdir -p "$here/.build"
name="$(basename "${src%.cpp}")"
exe="$here/.build/$name"

flags=("-std=$std" -O2 -Wall -Wextra -Wpedantic -Wshadow)
libs=()
if [[ "$compiler" == *g++* ]]; then
    # std::print / std::stacktrace trebuyut libstdc++exp (GCC >= 14).
    libs=(-lstdc++exp)
fi
if [[ $sanitize -eq 1 ]]; then
    flags+=(-g -fno-omit-frame-pointer -fsanitize=address,undefined)
fi

echo ">> $compiler ${flags[*]} $src -o $exe ${libs[*]}"
"$compiler" "${flags[@]}" "$src" -o "$exe" "${libs[@]}"

if [[ $run -eq 1 ]]; then
    echo ">> $exe"
    exec "$exe"
fi
