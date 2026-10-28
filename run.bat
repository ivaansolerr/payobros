@echo off
setlocal
cd /d "%~dp0"

echo Compilando PayoBros...
cmake --build build
if errorlevel 1 (
    echo Error en la compilacion con CMake, intentando con g++...
    g++ -std=c++17 src/main.cpp src/core/*.cpp src/entities/*.cpp src/levels/*.cpp src/objects/*.cpp src/objects/weapons/*.cpp src/states/*.cpp -Isrc -Ivendor/include -Ibuild/_deps/raylib-src/src -Lvendor/lib -Lbuild/_deps/raylib-build/raylib -lraylib -lbox2d -lopengl32 -lgdi32 -lwinmm -o payobros.exe
    if errorlevel 1 (
        echo Error en la compilacion.
        exit /b %errorlevel%
    )
    echo Ejecutando PayoBros...
    payobros.exe
    exit /b 0
)

echo Ejecutando PayoBros...
build\payobros.exe
