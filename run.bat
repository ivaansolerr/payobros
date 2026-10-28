@echo off
if not defined VCPKG_ROOT (
	for /f "delims=" %%I in ('where vcpkg 2^>nul') do (
		set "VCPKG_ROOT=%%~dpI"
		goto :vcpkg_root_found
	)
)
:vcpkg_root_found
if not defined VCPKG_ROOT (
	echo Define VCPKG_ROOT y ejecuta: vcpkg install box2d:x64-mingw-dynamic
	exit /b 1
)
set "VCPKG_INSTALLED=%VCPKG_ROOT%\installed\x64-mingw-dynamic"
if not exist "%VCPKG_INSTALLED%\include\box2d\box2d.h" (
	echo Instala Box2D con: vcpkg install box2d:x64-mingw-dynamic
	exit /b 1
)
g++ -std=c++17 src/main.cpp src/core/*.cpp src/entities/*.cpp src/levels/*.cpp src/objects/*.cpp src/objects/weapons/*.cpp src/states/*.cpp -Isrc -I"%VCPKG_INSTALLED%\include" -L"%VCPKG_INSTALLED%\lib" -lraylib -lbox2d -lopengl32 -lgdi32 -lwinmm -o game.exe
if errorlevel 1 exit /b %errorlevel%
game.exe
