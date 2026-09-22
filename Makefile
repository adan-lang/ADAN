.SILENT:

.PHONY: run build

FILE ?= ./test/grades-calc.adn

run: build
	echo "—————————————————————— Program's Output ——————————————————————"
	echo
	./build/ADAN $(FILE); \
	EXIT_CODE=$$?; \
	if [ $$EXIT_CODE -ne 0 ]; then \
	    echo; \
	    printf "\033[1;31mADAN exited with code $$EXIT_CODE\033[0m\n"; \
	    exit $$EXIT_CODE; \
	fi

build:
	echo "————————————————————————— Build Logs —————————————————————————"
	
	@printf "\nClearing existing build...                         	\033[1;33m[\033[39m1/3\033[33m]\033[0m\n"
	
	cmake -S . -B ./build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release > /tmp/adan_build.log 2>&1 \
	    || { printf "\n\033[1;31mADAN build failed!\033[0m\n"; \
	         echo ; echo "————————————————————————— Error Logs —————————————————————————"; \
	         cat /tmp/adan_build.log; \
	         echo "———————————————————————————————————————————————————————————————"; \
	         exit 1; }
	
	@printf "Re-compiling ADAN...                               	\033[1;33m[\033[39m2/3\033[33m]\033[0m\n"
	
	cmake --build ./build > /tmp/adan_build.log 2>&1 \
	    || { printf "\n\033[1;31mADAN build failed!\033[0m\n"; \
	         echo ; echo "————————————————————————— Error Logs —————————————————————————" ; echo; \
	         cat /tmp/adan_build.log; \
	         echo ; echo "———————————————————————————————————————————————————————————————"; \
	         exit 1; }
	
	@printf "Finishing ADAN build...                            	\033[1;33m[\033[39m3/3\033[33m]\033[0m\n"
	
	@printf "\n\033[1;32mADAN build success!\033[0m\n\n"