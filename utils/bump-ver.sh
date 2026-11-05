#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(git -C "$SCRIPT_DIR/.." rev-parse --show-toplevel)"
README="$REPO_ROOT/README.md"
MAIN="$REPO_ROOT/adanc/src/main.cc"
README_REL="README.md"
MAIN_REL="adanc/src/main.cc"

branch=$(git -C "$REPO_ROOT" branch --show-current)
upstream=$(git -C "$REPO_ROOT" rev-parse --abbrev-ref --symbolic-full-name '@{upstream}')
remote=$(git -C "$REPO_ROOT" config --get "branch.$branch.remote")
upstream_branch="${upstream#"$remote"/}"

[[ "$remote" != "." && "$upstream_branch" != "$upstream" ]] || {
    echo "Current branch must track a remote branch. Aborting." >&2
    exit 1
}

git -C "$REPO_ROOT" fetch --quiet "$remote"
[[ -z "$(git -C "$REPO_ROOT" status --porcelain -- "$README_REL" "$MAIN_REL")" &&
   "$(git -C "$REPO_ROOT" rev-list --count "$upstream...HEAD")" == "0" ]] || {
    echo "Keep the version files clean and synchronize the branch with $upstream before bumping." >&2
    exit 1
}

readme_current=$(sed -nE 's/.*badge\/version-([0-9]+\.[0-9]+\.[0-9]+)-blue.*/\1/p' "$README" | head -n 1)
compiler_current=$(sed -nE 's/.*COMPILER_VERSION = "version-([0-9]+\.[0-9]+\.[0-9]+)";.*/\1/p' "$MAIN" | head -n 1)

[[ -n "$readme_current" && "$readme_current" == "$compiler_current" ]] || {
    echo "Could not find matching versions in README.md and adanc/src/main.cc. Aborting." >&2
    exit 1
}

echo "Current README version: $readme_current"
echo "Current compiler version: $compiler_current"

read -rp "Bump to: " new_version

[[ "$new_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || {
    echo "Invalid version. Use major.minor.patch. Aborting." >&2
    exit 1
}

[[ "$new_version" != "$readme_current" ]] || {
    echo "Version is already $new_version. Nothing to do."
    exit 0
}

sed -i.bak -E "s|(badge/version-)[0-9]+\.[0-9]+\.[0-9]+(-blue)|\1${new_version}\2|" "$README"
sed -i.bak -E "s|(COMPILER_VERSION = \"version-)[0-9]+\.[0-9]+\.[0-9]+(\";)|\1${new_version}\2|" "$MAIN"
rm -f "$README.bak" "$MAIN.bak"

commit_message="Bump! (version: $readme_current -> $new_version)"
git -C "$REPO_ROOT" commit --only -m "$commit_message" -- "$README_REL" "$MAIN_REL"
git -C "$REPO_ROOT" push "$remote" "HEAD:refs/heads/$upstream_branch"

echo "Version bumped to $new_version, committed, and pushed."