#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if ! command -v black &>/dev/null && ! python3 -m black --version &>/dev/null 2>&1; then
    echo "Installing black..."
    python3 -m pip install --quiet black
fi

fmt_py() {
    if command -v black &>/dev/null; then
        black --quiet "$@"
    else
        python3 -m black --quiet "$@"
    fi
}

echo "Formatting Python files..."
mapfile -t py_files < <(find "$REPO_ROOT" -name "*.py" -not -path "*/__pycache__/*")
[[ ${#py_files[@]} -gt 0 ]] && fmt_py "${py_files[@]}"

if ! command -v clang-format &>/dev/null; then
    echo "Installing clang-format..."
    if   command -v apt-get &>/dev/null; then sudo apt-get update -qq && sudo apt-get install -y -qq clang-format || sudo apt-get install -y -qq "$(apt-cache search clang-format | awk 'NR==1{print $1}')"
    elif command -v dnf     &>/dev/null; then sudo dnf install -y clang-tools-extra
    elif command -v yum     &>/dev/null; then sudo yum install -y clang
    elif command -v pacman  &>/dev/null; then sudo pacman -S --noconfirm clang
    elif command -v zypper  &>/dev/null; then sudo zypper install -y clang
    elif command -v apk     &>/dev/null; then sudo apk add --no-cache clang
    else
        echo "Error: no supported package manager found to install clang-format" >&2
        exit 1
    fi
fi

echo "Formatting C++ files..."
mapfile -t cpp_files < <(find "$REPO_ROOT" \( -name "*.cpp" -o -name "*.hpp" \) -not -path "*/bin/*")
[[ ${#cpp_files[@]} -gt 0 ]] && clang-format -i "${cpp_files[@]}"

echo "Done."
