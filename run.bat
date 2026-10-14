g++ -std=c++17 src/main.cpp src/core/*.cpp src/entities/*.cpp src/levels/*.cpp src/objects/*.cpp src/states/*.cpp -Isrc -lraylib -lopengl32 -lgdi32 -lwinmm -o game.exe && game.exe
