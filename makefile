.SILENT:
.PHONY: build install clean

build:
	sudo rm -rf ./build
	cmake -S . -B ./build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build

install: build
	sudo cp ./build/adan /usr/local/bin/adan
	sudo chmod +x /usr/local/bin/adan

clean:
	rm -rf ./build