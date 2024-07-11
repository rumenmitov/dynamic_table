CC    := g++
SRC   := src/*.cpp
OBJ   := main
FLAGS := -Wall

release:
	mkdir -p build/release
	$(CC) $(SRC) -o build/release/$(OBJ) $(FLAGS) -O3

debug:
	mkdir -p build/debug
	$(CC) $(SRC) -o build/debug/$(OBJ) $(FLAGS) -g -DDEBUG

clean:
	rm -rf build