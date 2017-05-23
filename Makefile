SRC_DIR := src

SRC := $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.cpp))
SRC_MAIN := src/test/main.cpp

INCLUDES := src/logger.h

BUILD_DIR := $(addprefix build/,$(SRC_DIR))
BUILD_DIR := $(addprefix build/shared/,$(SRC_DIR))
BUILD_DIR += build/dest

DEST := build/dest/main

LIB_NAME := logger
LIB_FNAME := /usr/local/lib/lib$(LIB_NAME).so
VERSION := 3

OBJ := $(patsubst %.cpp,build/%.o,$(SRC))
OBJ += $(patsubst %.cpp,build/%.o,$(SRC_MAIN))
OBJ_SHARED := $(patsubst %.cpp,build/shared/%.o,$(SRC))

OPTIM := -O2
FLAGS := -std=c++17 -g3 -Wall -Wextra -Wno-pmf-conversions
LIBS := 
MAKEFLAGS += --no-print-directory

all: build/debug

# Main build task
build/debug: $(BUILD_DIR) $(OBJ)
	g++ $(FLAGS) -o $(DEST) $(OBJ) $(LIBS)
	@echo "---------------"
	@echo "Build Finished!"
	@echo "---------------"

build/%.o: %.cpp
	g++ -c $(OPTIM) $(FLAGS) -o "$@" "$<"

$(BUILD_DIR):
	@mkdir -p $@

# Clean every build files by destroying the build/ folder.
clean:
	rm -rf build
	@echo "----------------"
	@echo "Project  Cleaned"
	@echo "----------------"

again:
	@make clean
	@make build/debug

test: build/debug
	@clear
	@echo "----------------"
	@echo "    Run Test    "
	@echo "----------------"
	@$(DEST)
	@echo "----------------"
	@echo "      Stop      "
	@echo "----------------"

build/shared/%.o: %.cpp
	g++ $(OPTIM) $(FLAGS) -c -fPIC -o "$@" "$<"

install: $(BUILD_DIR) $(OBJ_SHARED)
	sudo g++ -shared -Wl,-soname,$(LIB_FNAME).$(VERSION) -o $(LIB_FNAME).$(VERSION) $(OBJ_SHARED)
	@sudo ln -sf $(LIB_FNAME).$(VERSION).0 $(LIB_FNAME)
	@sudo ln -sf $(LIB_FNAME).$(VERSION).0 $(LIB_FNAME).$(VERSION)
	@sudo cp $(INCLUDES) /usr/local/include
	@echo "----------------"
	@echo "  Lib Installed "
	@echo "----------------"
	@echo "Just include "$(INCLUDES)" in your projects and build with \"-l"$(LIB_NAME)"\""
