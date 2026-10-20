#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
README="$SCRIPT_DIR/../README.md"
MAIN="$SCRIPT_DIR/../adanc/src/main.cc"

readme_current=$(sed -nE 's/.*badge\/version-([0-9]+\.[0-9]+\.[0-9]+)-blue.*/\1/p' "$README" | head -n 1)
compiler_current=$(sed -nE 's/.*COMPILER_VERSION = "version-([0-9]+\.[0-9]+\.[0-9]+)";.*/\1/p' "$MAIN" | head -n 1)

if [[ -z "$readme_current" || -z "$compiler_current" ]]; then
    echo "Could not find the version in README.md or adanc/src/main.cc. Aborting." >&2
    exit 1
fi

echo "Current README version: $readme_current"
echo "Current compiler version: $compiler_current"

read -rp "Bump to: " new_version

if [[ ! "$new_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Invalid version. Use major.minor.patch. Aborting."
    exit 1
fi

sed -i.bak -E "s|(badge/version-)[0-9]+\.[0-9]+\.[0-9]+(-blue)|\1${new_version}\2|" "$README"
sed -i.bak -E "s|(COMPILER_VERSION = \"version-)[0-9]+\.[0-9]+\.[0-9]+(\";)|\1${new_version}\2|" "$MAIN"
rm -f "$README.bak" "$MAIN.bak"

echo "Version bumped to $new_version"