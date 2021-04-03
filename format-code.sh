#!/bin/bash -e

find src test -name '*.cpp' -exec clang-format -i {} \;
find src test -name '*.h' -exec clang-format -i {} \;

find . -path ./build -prune -false -o -name "CMakeLists.txt" \
    -exec cmake-format -c .cmake-format.yaml -i {} \;
