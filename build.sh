#!/bin/bash
# This script is purely to simplify the build procedure
# for those who do not have tools to do this otherwise.
#
# It supports both ninja and make as the makefile generator,
# preferring ninja when available.

# Builds ddgp by making the required folders.
# This will automatically use the make-program `ninja`
# if it exists
build_dddgp() {
  mkdir -p build bin lib
  cd build || exit

  # Currently our project only runs with g++, so we force
  # cmake to use g++ / gcc by setting export flags.
  # TODO: Remove these lines once project compiles with clang
  local tmpcc tmpcxx
  tmpcc=$CC
  tmpcxx=$CXX

  export CC="gcc"
  export CXX="g++"

  # Run different command based on
  # whether ninja is available.
  #
  # This could have been shorted down significantly
  # by the use of `eval`, but CMake is not a fan of that.
  if ! hash ninja 2/dev/null; then
    cmake .. -Wno-dev

    if [ "$1" == "run" ]; then
      make run
    else
      make
    fi
  else
    cmake .. -GNinja -Wno-dev

    if [ "$1" == "run" ]; then
      ninja run
    else
      ninja
    fi
  fi

  # Due to forcing the environmental variables we have
  # to set them back to normal
  # TODO: Something Something, remove this when clang++ compiles
  export CC=$tmpcc
  export CXX=$tmpcxx
}

# Prints the usage of the binary
function usage() {
  cat <<EOF
usage: ./build.sh <command> [-h|--help]

The following commands are available:

  -h, --help
    Shows this help menu

  clean
    Cleans the build by removing build, bin and lib folders
    as well as the binary files generated in root folder

  run
    Builds the executable by using normal build procedure
    followed by executing the executable. This is equivalent
    to running './build.sh build && ./DDDGP'

  build
    Builds the executable, creating needed directories if
    they don't already exist.
EOF
}

# Exiting, showing usage if no arguments
if [ $# -eq 0 ]; then
  usage
  exit
fi

# Parse the arguments and perform the actions
# asked for, printing help and exiting on invalid arguments
while [ $# -gt 0 ]; do
  PARAM=$(echo "$1" | awk -F= '{print $1}')
  case $PARAM in
    -h | --help)
      usage
      exit
      ;;
    clean)
      rm -rf build bin lib DDDGP
      ;;
    run)
      build_dddgp "run"
      ;;
    build)
      build_dddgp
      ;;
    *)
      echo "ERROR: unknown parameter \"$PARAM\""
      usage
      exit 1
      ;;
  esac
  shift
done
