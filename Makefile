CXX      ?= g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2
LIBS     := -Lvendor/lib -lbox2d -lraylib -lGL -lm -lpthread -lrt -lX11

INCLUDES := $(shell find src -type d -exec echo -I{} \;) -Ivendor/include

SRCS := $(shell find src -type f -name '*.cpp')
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))

TARGET := game

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LIBS)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf build $(TARGET)
