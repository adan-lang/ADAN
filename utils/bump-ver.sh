#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "$0")" && pwd)"
README="$SCRIPT_DIR/../README.md"
MAIN="$SCRIPT_DIR/../src/main.cc"

readme_current=$(grep -oP '(?<=badge/version-)[0-9]+\.[0-9]+\.[0-9]+' "$README" | head -1)
compiler_current=$(grep -oP '(?<=COMPILER_VERSION = "version-)[0-9]+\.[0-9]+\.[0-9]+' "$MAIN" | head -1)

echo "Current README version: $readme_current"
echo "Current compiler version: $compiler_current"

read -rp "Bump to: " new_version

if [[ ! "$new_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Invalid version. Use major.minor.patch. Aborting."
    exit 1
fi

sed -i -E "s|(badge/version-)[0-9]+\.[0-9]+\.[0-9]+(-blue)|\1${new_version}\2|" "$README"
sed -i -E "s|(COMPILER_VERSION = \"version-)[0-9]+\.[0-9]+\.[0-9]+(\";)|\1${new_version}\2|" "$MAIN"

echo "Version bumped to $new_version"