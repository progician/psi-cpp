#!/usr/bin/env sh
find . -name "*.hpp" -or -name "*.cpp" -or -name "*.h" | xargs ~/builds/llvm-master-2018-08-22/bin/clang-format -style=file -i -fallback-style=none
