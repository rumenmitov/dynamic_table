CC    := g++
SRC   := src/*.cpp
OBJ   := main
FLAGS := -Wall

all: release clean

release:
	@mkdir -p build/release
	@$(CC) $(SRC) -o build/release/$(OBJ) $(FLAGS) -O3
	@./build/release/main

debug:
	@mkdir -p build/debug
	@$(CC) $(SRC) -o build/debug/$(OBJ) $(FLAGS) -g -DDEBUG
	@./build/debug/main

clean:
	@rm -rf build
