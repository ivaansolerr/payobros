#!/bin/bash
set -e

# Asegurar que nos situamos en la raíz del proyecto independientemente de desde dónde se ejecute
cd "$(dirname "$0")"

INCLUDE_FLAGS=""
LIB_FLAGS=""

# 1. Priorizar Box2D del repositorio (carpeta vendor)
if [[ -f "vendor/include/box2d/box2d.h" && -f "vendor/lib/libbox2d.a" ]]; then
    INCLUDE_FLAGS="-Ivendor/include"
    LIB_FLAGS="-Lvendor/lib"
# 2. Comprobar si está disponible mediante vcpkg
elif [[ -n "${VCPKG_ROOT:-}" && -f "$VCPKG_ROOT/installed/x64-linux/include/box2d/box2d.h" ]]; then
    VCPKG_INSTALLED="$VCPKG_ROOT/installed/x64-linux"
    INCLUDE_FLAGS="-I$VCPKG_INSTALLED/include"
    LIB_FLAGS="-L$VCPKG_INSTALLED/lib"
# 3. Comprobar si está instalado en el sistema
elif [[ -f "/usr/include/box2d/box2d.h" || -f "/usr/local/include/box2d/box2d.h" ]] || pkg-config --exists box2d 2>/dev/null; then
    INCLUDE_FLAGS=""
    LIB_FLAGS=""
else
    echo "Error: No se encontró Box2D." >&2
    echo "Asegúrate de contar con la carpeta vendor/ o instalar Box2D (en el sistema o mediante vcpkg)." >&2
    exit 1
fi

# Detectar flags para raylib (pkg-config si está instalado, o -lraylib por defecto)
if pkg-config --exists raylib 2>/dev/null; then
    RAYLIB_FLAGS="$(pkg-config --cflags --libs raylib)"
else
    RAYLIB_FLAGS="-lraylib"
fi

echo "Compilando proyecto..."
g++ -std=c++17 $(find src -name "*.cpp") -Isrc $INCLUDE_FLAGS $LIB_FLAGS $RAYLIB_FLAGS -lbox2d -o game

echo "Ejecutando..."
./game "$@"

