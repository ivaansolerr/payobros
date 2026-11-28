CXX      ?= g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2 -MMD -MP
LIBS     := -Lvendor/lib -lbox2d -lraylib -lGL -lm -lpthread -lrt -lX11

# Por el Box2D
INCLUDES := $(shell find src -type d -exec echo -I{} \;) -Ivendor/include

SRCS := $(shell find src -type f -name '*.cpp')
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)

-include $(DEPS)

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

PREFIX ?= /usr
DESTDIR ?=

.PHONY: install
install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/payobros
	install -d $(DESTDIR)$(PREFIX)/share/payobros
	cp -r src/assets $(DESTDIR)$(PREFIX)/share/payobros/
