#!/bin/bash
set -e

if [[ -z "${VCPKG_ROOT:-}" ]]; then
	echo "Define VCPKG_ROOT and install Box2D with: vcpkg install box2d:x64-linux" >&2
	exit 1
fi

VCPKG_INSTALLED="$VCPKG_ROOT/installed/x64-linux"
if [[ ! -f "$VCPKG_INSTALLED/include/box2d/box2d.h" ]]; then
	echo "Install Box2D with: vcpkg install box2d:x64-linux" >&2
	exit 1
fi

g++ -std=c++17 $(find src -name "*.cpp") -Isrc -I"$VCPKG_INSTALLED/include" -L"$VCPKG_INSTALLED/lib" -lraylib -lbox2d -o game
./game
