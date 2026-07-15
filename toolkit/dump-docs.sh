#!/bin/sh

{
    find . \
        \( -type d \( \
            -name .git -o \
            -name .venv -o \
            -name __pycache__ -o \
            -name build -o \
            -name assets -o \
            -name .lustre -o \
            -name .github -o \
            -name toolkit -o \
            -name blueprints \
        \) -prune \) -o \
        -type f -name "*.md" -print |
    while IFS= read -r file
    do
        printf "## %s\n\n" "$file"
        cat "$file"
        printf "\n\n"
    done
} | wl-copy --trim-newline --type text/plain