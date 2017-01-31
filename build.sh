#!/bin/bash
# This script is purely to simplify the build procedure
# for those who do not have tools to do this otherwise.
#
# It supports both ninja and make as the makefile generator,
# preferring ninja when available.

# Builds ddgp by making the required folders.
# This will automatically use the make-program `ninja`
# if it exists
build_project() {
  local build_type
  mkdir -p build bin lib
  cd build || exit

  # Set build type
  if [ "$2" == "release" ]; then
    build_type="Release"
  else
    build_type="Debug"
  fi

  # Run different command based on
  # whether ninja is available.
  #
  # This could have been shorted down significantly
  # by the use of `eval`, but CMake is not a fan of that.
  if hash ninja 2>/dev/null; then
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

  clang-format
    Runs clang-format on every file located under src/*. Will fail if there
    is not a .clang-format file in the directory. Will show changes made
    to files. It will only consider '.cpp', '.hpp', or '.tpp' files.

The following flags are available:

  -h, --help
    Shows this help menu

  -r, --release
    Sets CMake up to build a release build. Using this
    will force a total rebuild nomatter with optimizations.

  -d, --debug
    Sets CMake up to build a debug build. This is set by
    default and you should not need to specify it.

  -i, --interactive
    This flag only works with the command 'clang-format'. It will, before
    changing every file, ask you if you want to replace it with the new
    format. It will also show you the difference between the original and
    the newly formatted file.

    The newly formatted file with have the extension '.formatted' in addition
    to the '.cpp', '.hpp', or '.tpp' format that is supported.

EOF
}

# Runs clang-format on all the files one by one, outputting only
# if a file was changed.
# Also supports an interactive mode that shows the changes
# that are to be done and asks for confirmation before
# performing the said changes
run_clang() {
  local files=()
  local diff
  local use_color_diff=1

  # Throw error if we cant find a .clang-format file since
  # we dont trust the clang-format to do the correct formatting
  # without it
  if [ ! -f "./.clang-format" ]; then
    echo "Error: No .clang-format file found in directory"
    exit
  fi

  # Recommend to use colordiff over diff. So much better
  if ! hash colordiff 2>/dev/null; then
   if [ "$1" == "interactive" ]; then
      echo -en "\e[1m\e[31m=>\e[0m colordiff not installed. Recommend installing"
      echo -e " it for syntax highlighting when viewing diffs"
      use_color_diff=0
    fi
  fi

  echo -e "\e[1m\e[31m=>\e[0m Grabbing all source files"

  # Adds all the files that has been found to a list of files by
  # splitting by space and processing each entry
  while IFS=  read -r -d $'\0'; do
    files+=("$REPLY")
  done < <(find "src/" \( -iname "*.hpp" -o -iname "*.cpp" -o -iname "*.tpp" \) -print0)

  echo -e "\e[1m\e[31m=>\e[0m Formatting files"

  # Start processing file by file. Adding progress as we go along
  for file in "${files[@]}"; do
    clang-format -style="file" "$file" > "$file.formatted"
    diff=$(diff "$file" "$file.formatted")

    # No changes == continue
    if [ -z "$diff" ]; then
      rm "$file.formatted"
      continue
    fi

    echo -en "\e[1m\e[31m=>\e[0m Found difference in '$file'. "

    # If interactive is enabled, show diff and then prompt for change
    if [ "$1" == "interactive" ]; then

      # If colordiff is available, pipe that to less with the RAW tag
      if [[ use_color_diff -eq 1 ]]; then
        colordiff --unified=2 "$file" "$file.formatted" | less -R
      else
        diff --unified=2 "$file" "$file.formatted" | less
      fi

      # Ask user for whether or not this is okay. if not we ignore and continue
      read -p "Do you agree with the changes? (y/n) " -n 1 -r

      if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo -e "\e[31m  Ignoring...\e[0m"
        rm "$file.formatted"
        continue
      fi
    fi

    # Go signal is given, replace file
    echo -e "\e[32m  Replacing...\e[0m"
    mv "$file.formatted" "$file"
  done
}

# Removes the build directory used by cmake, as well
# as other directories and files that are produced
# when building
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

    # If going from release to debug builds, clear it
    if [ "$result" == "Release" ] && [ "$1" != "release" ]; then
      clean_build_dir
    fi
  fi
}

# If executed outside of root folder, we fail
if [ ! -f "CMakeLists.txt" ] || [ ! -f ".gitignore" ]; then
  echo "Error: Script must be run in root of project folder"
  exit
fi

# Exiting, showing usage if no arguments
if [ $# -eq 0 ]; then
  usage
  exit
fi

COMMAND=""
FLAG=""
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
    clang-format)
      COMMAND="clang-format"
      ;;
    -r | --release)
      clean_build_dir
      FLAG="release"
      ;;
    -i | --interactive)
      FLAG="interactive"
      ;;
    *)
      echo "ERROR: unknown parameter \"$PARAM\""
      usage
      exit 1
      ;;
  esac
  shift
done

# Exit if user tries to do interactive build or something
if [ "$FLAG" == "interactive" ] && [ "$COMMAND" != "clang-format" ]; then
  echo "Error: -i, --interactive can only be used with command 'clang-format'"
  exit
fi

# Finally perform the action needed based on the arguments
# that we got
case "$COMMAND" in
  build | run)
    different_build_type "$FLAG"
    build_project "$COMMAND" "$FLAG"
    ;;
  clang-format)
    run_clang "$FLAG"
    ;;
esac
