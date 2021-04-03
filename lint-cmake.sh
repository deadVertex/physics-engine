#!/bin/bash -e

find . -path ./build -prune -false -o -name "CMakeLists.txt" \
    -exec cmake-lint {} \;
