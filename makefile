.SILENT:
.PHONY: build install clean

build:
	rm -rf ./build
	cmake -S . -B ./build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build

install: build
	cp ./build/adan /usr/local/bin/adan
	chmod +x /usr/local/bin/adan

clean:
	rm -rf ./build