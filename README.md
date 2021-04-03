# Physics Engine
![Superlinter](https://github.com/deadVertex/physics-engine/actions/workflows/superlinter.yml/badge.svg)
![Build and Test](https://github.com/deadVertex/physics-engine/actions/workflows/build_and_test.yml/badge.svg)
[![Coverage Status](https://coveralls.io/repos/github/deadVertex/physics-engine/badge.svg?branch=master)](https://coveralls.io/github/deadVertex/physics-engine?branch=master)

My own experiments and attempt to write a physics engine to learn about how
rigid body dynamics are simulated in games.

## Dependencies
- Cmake 2.8 or later
- libsdl2-dev
- gcovr
- clang-format
- cmakelang and pyyaml

## Instructions
### Install build dependencies (for Ubuntu 20.04)
```shell
sudo apt install build-essential cmake libsdl2-dev
```

### Install gcovr for calculating code coverage
```shell
sudo apt install python3-pip python3-venv
python3 -m venv venv
source ./venv/bin/activate
pip install gcovr
```

### Build and run unit tests
```shell
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
cd build
ctest -C Debug --verbose
```

### Generate coverage report
```shell
gcovr -r . --exclude build
```

### Run the visualization application
```shell
./build/src/main
```

### Automatic formating
```shell
# Install clang-format
sudo apt install clang-format

# Install cmakelang
source ./venv/bin/activate
pip install cmakelang pyyaml

# Lint CMakeList files
./lint-cmake.sh

# Format all .cpp and .h files as well as all CMakeList.txt files
./format-code.sh
```

## Sources
- [Chris Hecker's articles on Rigid Body Dynamics](https://chrishecker.com/Rigid_Body_Dynamics)
