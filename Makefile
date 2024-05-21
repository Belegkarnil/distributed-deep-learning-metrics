# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CPP = g++
CPPFLAGS = -Wall -g
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=%.o)

# ****************************************************
# Targets needed to bring the executable up to date
all: metrics

metrics: build_dir $(OBJ)
	$(CPP) $(CPPFLAGS) -o build/metrics.bin $(OBJ)

%.o:
	$(CPP) $(CPPFLAGS) -c "src/$(@:%.o=%.cpp)" -o "build/$@"

build_dir:
	mkdir -p build

clean:
	rm -rf build/*.o build/*.bin
