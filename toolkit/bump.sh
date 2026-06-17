#!/bin/bash

README="$(dirname "$0")/../README.md"

current=$(grep -oP '(?<=version-)[^-"]+' "$README" | head -1)
echo "Current version: $current"

read -rp "Bump to: " new_version

if [[ -z "$new_version" ]]; then
    echo "No version provided. Aborting."
    exit 1
fi

sed -i "s/version-${current}-/version-${new_version}-/" "$README"
echo "Version bumped: $current -> $new_version"
