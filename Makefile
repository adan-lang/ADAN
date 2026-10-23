.PHONY: run build test-semantic

FILE ?= ./test/test2.adn
BUILD_DIR ?= ./build
BUILD_LOG ?= /tmp/adan_build.log

run: build
	@output_file=$$(mktemp) || { printf "Failed to create temporary output file\n" >&2; exit 1; }; \
	$(BUILD_DIR)/ADAN "$(FILE)" > "$$output_file"; \
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
	@set -eu; \
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
	printf "\n\033[1;33m[1/3]\033[0m Configuring build...\n"; \
	if ! cmake -S . -B "$(BUILD_DIR)" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release > "$(BUILD_LOG)" 2>&1; then \
	    report_build_failure; \
	    exit 1; \
	fi; \
	printf "\033[1;33m[2/3]\033[0m Compiling ADAN...\n"; \
	if ! cmake --build "$(BUILD_DIR)" > "$(BUILD_LOG)" 2>&1; then \
	    report_build_failure; \
	    exit 1; \
	fi; \
	printf "\033[1;33m[3/3]\033[0m Finishing build...\n"; \
	printf "\n\033[1;32mADAN build success!\033[0m\n\n"

test-semantic: build
	@printf "—————————————————————— Program's Output ——————————————————————\n\n"
	@$(BUILD_DIR)/ADAN -st ./test/grades-calc.adn
	@printf "\n"
