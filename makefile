.SILENT:

build:
	@rm -rf ./build
	@mkdir -p ./build
	@g++ main.cc -o ./build/adan

install: build
	@cp ./build/adan /usr/local/bin/adan
	@chmod +x /usr/local/bin/adan