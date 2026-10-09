.SILENT:

.PHONY: run build test-parser test-lexer

FILE ?= ./test/grades-calc.adn
BUILD_DIR ?= ./build

run: build
	@output_file=$$(mktemp) || { printf "Failed to create temporary output file\n" >&2; exit 1; }; \
	$(BUILD_DIR)/ADAN $(FILE) > "$$output_file"; \
	EXIT_CODE=$$?; \
	if [ -s "$$output_file" ]; then \
	    echo "—————————————————————— Program's Output ——————————————————————"; \
	    echo; \
	    cat "$$output_file"; \
	fi; \
	rm -f "$$output_file"; \
	if [ $$EXIT_CODE -ne 0 ]; then \
	    echo; \
	    printf "\033[1;31mADAN exited with code $$EXIT_CODE\033[0m\n"; \
	    exit $$EXIT_CODE; \
	fi

build:
	echo "————————————————————————— Build Logs —————————————————————————"
	
	@printf "\nClearing existing build...                         	\033[1;33m[\033[39m1/3\033[33m]\033[0m\n"
	
	cmake -S . -B $(BUILD_DIR) -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release > /tmp/adan_build.log 2>&1 \
	    || { printf "\n\033[1;31mADAN build failed!\033[0m\n"; \
	         echo ; echo "————————————————————————— Error Logs —————————————————————————"; \
	         cat /tmp/adan_build.log; \
	         echo "———————————————————————————————————————————————————————————————"; \
	         exit 1; }
	
	@printf "Re-compiling ADAN...                               	\033[1;33m[\033[39m2/3\033[33m]\033[0m\n"
	
	cmake --build $(BUILD_DIR) > /tmp/adan_build.log 2>&1 \
	    || { printf "\n\033[1;31mADAN build failed!\033[0m\n"; \
	         echo ; echo "————————————————————————— Error Logs —————————————————————————" ; echo; \
	         cat /tmp/adan_build.log; \
	         echo ; echo "———————————————————————————————————————————————————————————————"; \
	         exit 1; }
	
	@printf "Finishing ADAN build...                            	\033[1;33m[\033[39m3/3\033[33m]\033[0m\n"
	
	@printf "\n\033[1;32mADAN build success!\033[0m\n\n"

test-lexer: build
	echo "—————————————————————— Program's Output ——————————————————————"
	echo
	$(BUILD_DIR)/ADAN -lt ./test/grades-calc.adn

test-parser: build
	echo "—————————————————————— Program's Output ——————————————————————"
	echo
	$(BUILD_DIR)/ADAN -pt ./test/grades-calc.adn