.PHONY : all clean
all=mtx2img
CXXFLAGS=-std=c++20 -O3 -DNDEBUG -march=native -flto -g
CXX=g++

mtx2img:
	mkdir -p build/bin
	g++ -Iinclude -Iexternal $(CXXFLAGS) -o build/bin/mtx2img src/mtx2img.cpp src/main.cpp

clean:
	rm -rf build
