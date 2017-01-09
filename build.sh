#!/bin/bash
if [ $# -eq 1 ] && [ "$1" == "clean" ]; then
  rm -r build bin lib DDDGP
else
  mkdir -p build bin lib
  cd build || exit
  cmake .. -GNinja -Wno-dev
  ninja
fi
