CC    := g++
SRC   := src/*.cpp
OBJ   := main
FLAGS := -Wall

release:
	@mkdir -p build/release
	@$(CC) $(SRC) -o build/release/$(OBJ) $(FLAGS) -O3

debug:
	@mkdir -p build/debug
	@$(CC) $(SRC) -o build/debug/$(OBJ) $(FLAGS) -g -DDEBUG

graph: debug
	@./build/debug/$(OBJ)
	@gnuplot results.p

clean:
	@rm -rf build results.csv
