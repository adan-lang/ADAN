.PHONY: run build

SHELL := /bin/bash

BUILD_DIR ?= ./build/ninja
BUILD_LOG ?= /tmp/adan_build.log

run: build
	@output_file=$$(mktemp) || { printf "Failed to create temporary output file\n" >&2; exit 1; }; \
	$(BUILD_DIR)/ADAN > "$$output_file"; \
	exit_code=$$?; \
	if [ -s "$$output_file" ]; then \
	    printf "—————————————————————— Program's Output ——————————————————————\n\n"; \
	    cat "$$output_file"; \
	fi; \
	rm -f "$$output_file"; \
	if [ $$exit_code -ne 0 ]; then \
	    printf "\n\033[1;31mADAN exited with code %s\033[0m\n" "$$exit_code"; \
	    exit $$exit_code; \
	fi

build:
	@set -eu -o pipefail; \
	copy_log_to_clipboard() { \
	    if command -v pbcopy >/dev/null 2>&1; then \
	        pbcopy < "$(BUILD_LOG)" && return 0; \
	    fi; \
	    if command -v wl-copy >/dev/null 2>&1; then \
	        wl-copy < "$(BUILD_LOG)" && return 0; \
	    fi; \
	    if command -v xclip >/dev/null 2>&1; then \
	        xclip -selection clipboard < "$(BUILD_LOG)" && return 0; \
	    fi; \
	    if command -v xsel >/dev/null 2>&1; then \
	        xsel --clipboard --input < "$(BUILD_LOG)" && return 0; \
	    fi; \
	    return 1; \
	}; \
	report_build_failure() { \
	    printf "\n\033[1;31mADAN build failed!\033[0m\n\n"; \
	    printf "————————————————————————— Error Logs —————————————————————————\n\n"; \
	    cat "$(BUILD_LOG)"; \
	    printf "\n———————————————————————————————————————————————————————————————\n"; \
	    if copy_log_to_clipboard; then \
	        printf "\nBuild errors copied to the clipboard.\n"; \
	    else \
	        printf "Clipboard copy unavailable; install pbcopy, wl-copy, xclip, or xsel.\n"; \
	    fi; \
	}; \
	printf "————————————————————————— Build Logs —————————————————————————\n"; \
	if ! cmake -S . -B "$(BUILD_DIR)" -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release > "$(BUILD_LOG)" 2>&1; then \
	    report_build_failure; \
	    exit 1; \
	fi; \
	printf "\n"; \
	if ! cmake --build "$(BUILD_DIR)" --parallel 6 2>&1 | tee "$(BUILD_LOG)" | awk ' \
	    /^ninja: no work to do\./ { print "Build is up to date."; next } \
	    /warning:/ { print "  " $$0; next } \
	    /^\[[0-9]+\/[0-9]+\] / { \
	        progress = $$1; \
	        sub(/^\[/, "", progress); \
	        sub(/\]$$/, "", progress); \
	        split(progress, counters, "/"); \
	        command_total = counters[2] - 1; \
	        detail = $$0; \
	        sub(/^\[[0-9]+\/[0-9]+\] /, "", detail); \
	        if (detail ~ /^Building CXX object /) { \
	            source_position = index(detail, "adanc/src/"); \
	            if (source_position > 0) \
	                detail = substr(detail, source_position + length("adanc/src/")); \
	            else \
	                sub(/^.*\//, "", detail); \
	            sub(/\.o$$/, "", detail); \
	            printf "\033[1;33m[%02d/%02d]\033[0m Compiling: %s\n", counters[1], command_total, detail; \
	        } else if (detail ~ /^Linking CXX executable /) { \
	            sub(/^Linking CXX executable /, "", detail); \
	            printf "\033[1;33m[%02d/%02d]\033[0m Linking: %s\n", counters[1], command_total, detail; \
	        } else if (detail ~ /^Linking CXX static library /) { \
	            sub(/^Linking CXX static library /, "", detail); \
	            printf "\033[1;33m[%02d/%02d]\033[0m Linking: %s\n", counters[1], command_total, detail; \
	        } \
	    } \
	'; then \
	    report_build_failure; \
	    exit 1; \
	fi; \
	printf "\n\033[1;32mADAN build success!\033[0m\n\n"