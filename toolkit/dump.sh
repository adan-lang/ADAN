#!/bin/sh

{
    tree -a -I ".git|build|assets|.lustre|.github|toolkit|manifest.toml|LICENSE"

    printf "\n\n"

    find . \
        \( -type d -name "__pycache__" -o -name "build" -o -name "assets" -o -name ".lustre" -o -name ".github" -o -name "toolkit" \) -prune -o \
        -type f \
        ! -name "manifest.toml" \
        ! -name "LICENSE" \
        ! -path "./.venv/*" \
        ! -path "./blueprints/*" \
        ! -path "./.git/*" \
        -print | while IFS= read -r file
    do
        printf "## %s\n\n\n" "$file"

        cat "$file"

        printf "\n\n"
    done
} | wl-copy --trim-newline --type text/plain