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
  local build_type
  mkdir -p build bin lib
  cd build || exit

  # Set build type
  if [ "$2" == "release" ]; then
    build_type="Release"
  else
    build_type="Debug"
  fi

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
  if hash ninja 2/dev/null; then
    if [ ! -f "./build.ninja" ]; then
      cmake -GNinja -Wno-dev -D CMAKE_BUILD_TYPE="$build_type" ..
    fi

    if [ "$1" == "run" ]; then
      ninja run
    else
      ninja
    fi
  else
    if [ ! -f "./Makefile" ]; then
      cmake -Wno-dev -D CMAKE_BUILD_TYPE="$build_type" ..
    fi

    if [ "$1" == "run" ]; then
      make run
    else
      make
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
usage: ./build.sh <command> [<flags>]

The following commands are available:

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

The following flags are available:

  -h, --help
    Shows this help menu

  -r, --release
    Sets CMake up to build a release build. Using this
    will force a total rebuild nomatter with optimizations.

  -d, --debug
    Sets CMake up to build a debug build. This is set by
    default and you should not need to specify it.

EOF
}

clean_build_dir() {
  rm -rf build bin lib DDDGP
}

# If we try to run a debug build with a release build
# within the build directory, we clean the build dir
different_build_type() {
  # Just test that Cmake has been executed
  if [ -d "./build" ] && [ -f "./build/CMakeCache.txt" ]; then
    local result

    cd ./build || exit
    if hash ninja 2/dev/null; then
      result=$(ninja print_build_type | awk 'NR!=1')
    else
      result=$(make --silent print_build_type)
    fi

    cd ..

    if [ "$result" == "Release" ] && [ "$1" != "release" ]; then
      clean_build_dir
    fi
  fi
}

# Exiting, showing usage if no arguments
if [ $# -eq 0 ]; then
  usage
  exit
fi

COMMAND=""
BUILD_TYPE=""
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
      clean_build_dir
      exit
      ;;
    run)
      COMMAND="run"
      ;;
    build)
      COMMAND="build"
      ;;
    -r | --release)
      clean_build_dir
      BUILD_TYPE="release"
      ;;
    *)
      echo "ERROR: unknown parameter \"$PARAM\""
      usage
      exit 1
      ;;
  esac
  shift
done

# Finally perform the action needed based on the arguments
# that we got
different_build_type "$BUILD_TYPE"
build_dddgp "$COMMAND" "$BUILD_TYPE"
