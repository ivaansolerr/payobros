#!/bin/bash
g++ -std=c++17 $(find src -name "*.cpp") -Isrc -lraylib -o game && ./game
