#!/bin/bash -e

find src test -name '*.cpp' -exec clang-format -i {} \;
find src test -name '*.h' -exec clang-format -i {} \;
