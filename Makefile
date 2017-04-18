SRC_DIR := src src/test

SRC := $(foreach d,$(SRC_DIR),$(wildcard $(d)/*.cpp))

BUILD_DIR := $(addprefix build/,$(SRC_DIR))
DEST := build/debug

OBJ := $(patsubst %.cpp,build/%.o,$(SRC))

OPTIM := -O2
FLAGS := -std=c++17 -g3 -Wall -Wextra -Wno-pmf-conversions
LIBS := 
MAKEFLAGS += --no-print-directory

all: build/debug

# Main build task
build/debug: $(BUILD_DIR) $(OBJ)
	g++ $(FLAGS) -o $(DEST) $(OBJ) $(LIBS)
	@echo "---------------"
	@echo "Build finished!"
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
