#!/usr/bin/env bash
set -euo pipefail

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	echo "Not inside a git repository." >&2
	exit 1
fi

branch=$(git symbolic-ref --quiet --short HEAD 2>/dev/null || true)
if [[ -z "$branch" ]]; then
	echo "Detached HEAD is not supported." >&2
	exit 1
fi

upstream=$(git rev-parse --abbrev-ref --symbolic-full-name "@{upstream}" 2>/dev/null || true)
if [[ -z "$upstream" ]]; then
	echo "No upstream configured for $branch." >&2
	exit 1
fi

if ! committer_ident=$(git var GIT_COMMITTER_IDENT 2>/dev/null); then
	echo "Git committer identity is not configured." >&2
	exit 1
fi

month=$(date +"%B")
day=$(date +"%-d")
year=$(date +"%Y")
time=$(date +"%-I:%M %p")
timezone_source=${LAZY_GIT_TIMEZONE:-${TZ:-$(git config --get lazygit.timezone 2>/dev/null || true)}}
timezone_name=$(TZ="$timezone_source" date +"%Z" 2>/dev/null || date +"%Z")

committer_name=${committer_ident%% <*}
committer_timezone_offset=${committer_ident##* }

case "$day" in
	1|21|31) suffix="st" ;;
	2|22) suffix="nd" ;;
	3|23) suffix="rd" ;;
	*) suffix="th" ;;
esac

commit_message="upd: lazily commit on $month $day$suffix, $year $time $timezone_name $committer_timezone_offset by $committer_name"
stash_name="lazy-git-autostash-$(date +%s)"
temp_branch="lazy-git-temp-$(date +%s)"
stashed=0
temp_branch_created=0
current_branch="$branch"

resolve_remaining_conflicts() {
	local conflict_paths
	conflict_paths=$(git diff --name-only --diff-filter=U)
	if [[ -z "$conflict_paths" ]]; then
		return
	fi
	while IFS= read -r path; do
		[[ -z "$path" ]] && continue
		if git checkout --theirs -- "$path" >/dev/null 2>&1; then
			git add -- "$path"
		else
			git rm -f -- "$path" >/dev/null 2>&1 || true
		fi
	done <<< "$conflict_paths"
}

cleanup_on_error() {
	status=$?
	if [[ "$status" -ne 0 ]]; then
		if git rev-parse --verify REBASE_HEAD >/dev/null 2>&1; then
			git rebase --abort >/dev/null 2>&1 || true
		fi
		if git rev-parse --verify MERGE_HEAD >/dev/null 2>&1; then
			git merge --abort >/dev/null 2>&1 || true
		fi
		if git rev-parse --verify CHERRY_PICK_HEAD >/dev/null 2>&1; then
			git cherry-pick --abort >/dev/null 2>&1 || true
		fi
		active_branch=$(git symbolic-ref --quiet --short HEAD 2>/dev/null || true)
		if [[ -n "$active_branch" && "$active_branch" != "$current_branch" ]]; then
			git switch "$current_branch" >/dev/null 2>&1 || true
		fi
		if [[ "$temp_branch_created" -eq 1 ]]; then
			git branch -D "$temp_branch" >/dev/null 2>&1 || true
		fi
		if [[ "$stashed" -eq 1 ]]; then
			echo "Automatic merge failed. Your changes remain in stash: $stash_name" >&2
		fi
	fi
	exit "$status"
}

trap cleanup_on_error EXIT

if [[ -n "$(git status --porcelain)" ]]; then
	git stash push --include-untracked --message "$stash_name" >/dev/null
	stashed=1
	git switch -c "$temp_branch" >/dev/null
	temp_branch_created=1
	git stash pop --index >/dev/null
	stashed=0
	git add -A
	git commit -m "$commit_message" >/dev/null
	git switch "$current_branch" >/dev/null
fi

git pull --rebase --stat

if [[ "$temp_branch_created" -eq 1 ]]; then
	if ! git cherry-pick -X theirs "$temp_branch"; then
		resolve_remaining_conflicts
		git cherry-pick --continue >/dev/null
	fi
	git branch -D "$temp_branch" >/dev/null
	temp_branch_created=0
fi

if [[ -n "$(git ls-files --unmerged)" ]]; then
	resolve_remaining_conflicts
fi

if [[ -n "$(git ls-files --unmerged)" ]]; then
	echo "Automatic conflict resolution could not complete cleanly." >&2
	exit 1
fi

# if [[ -z "$(git status --porcelain)" ]]; then
# 	echo "No changes to commit." >&2
# 	exit 0
# fi

git push

trap - EXIT