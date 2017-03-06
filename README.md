# Master Thesis

This repository contains the master thesis for Alexander Imenes and Svein Inge Albrigtsen. The master thesis was performed at [University of Agder](http://uia.no) in the Spring semester of 2017. The project itself bases itself on a game engine that was created during the course Advanced Programming with C++ in the fall semester of 2013. The engine was later expanded with Dynamic Animations and Procedural Terrain Generations during the bachelor thesis done in the Spring semester of 2014 by the same authors.

The repository contains several folders, most of which are essential to the code itself. Below is a short explanation of each of the directories

**config**

This folder contains the .ini file that is used to set the different options used by the game engine. It contains anything from graphical options to keybindings.

**deps**

This folder contains all the dependencies of the project as source files. All of these, except for one, is git submodules. This means that these are retrieved from other repositories when you are cloning it and therefore need the `--recursive` flag.

The only dependency that is not a submodule is the `OGL` library, which are files that have been generated. For more information about the dependencies, please checkout [Dependencies](#dependencies)

**lua**

This folder contains LUA files that are loaded by the Engine at some point during runtime. The structure of these files are work-in-progress

**media**

This folder contains all resources needed by the engine during runtime such as textures, models and XML files used for loading the GUI.

**shaders**

This directory contains all the GLSL shader files used by the game engine. They can be considered additional source files.

**src**

The main source folder. All code is in here.

In addition to these folders, the build process will also create three additional folders, `build`, `bin` and `lib`. `build` contains all the cmake and object files that are needed for compiling. `bin` holds all the binary files, except for the main project one which will be located in the root of the folder. `lib` contains all the shared libraries that are built.

# Building and running

This project has only been tested on Linux. All the dependencies does however support cross-platform and it should therefore be possible to compile and run it on other systems.

The `ninja` system is preferred when building this project due to the speed increases it has. It can, however, be built with `make` as well. The build file showcased below will use `ninja` if available, otherwise it will use `make`.

```
# Clone and change dir
git clone --recursive git://github.com/reewr/master-thesis.git
cd master-thesis

# Build the project, followed by running the executable
./build.sh run
```

The [build.sh](./build.sh) file has several commands that can be useful for development, such as `run` for building the engine followed by starting it, `build` to just build it, `clean` to rebuild it. More commands and more information can be found by using `./build.sh -h`

# Documentation

The documentation of the code is slightly sparse, but it is getting better and better. The code documentation tool that this project has chosen is [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html). This is available for most Linux distributions and may also be on other systems such as Windows and Mac.

After this tool has been installed, you can run `./build.sh doc` to generate the documentation in folders under `doc`.

# Dependencies

This project utilizes 11 different dependencies for release builds and 12 different dependencies for development builds. What follows is a short explanation of each library. While all libraries are needed to build the engine, only those marked with a `*` was exclusively added to the engine to help with this project.

## [Bullet3*](http://bulletphysics.org/wordpress/)

Bullet3 is a real-time collision detection and multi-physics simulation that is used to enable physics on objects within the engine. It is used in this project for its simple setup and API.

It is licensed under a [zlib license](https://raw.githubusercontent.com/bulletphysics/bullet3/master/LICENSE.txt)

## [MultiNeat*](http://multineat.com/)

MultiNeat is a library for performing neuroevolution using the NEAT algorithm. The library also includes HyperNEAT among other concepts within neuroevolution.

It is licensed under a [LGPLv3 license](https://raw.githubusercontent.com/peter-ch/MultiNEAT/master/LICENSE) and as such, the library is dynamically linked with our engine in order to keep our license as MIT.

## [backward-cpp](https://github.com/bombela/backward-cpp)

Used for development builds only! This is a library that helps debugging thrown errors as well as segmentation faults. Whenever an unhandled crash happens, it will print a stack trace of the last executed code (and usually the culprit).

The library is licensed under a [MIT license](https://raw.githubusercontent.com/bombela/backward-cpp/master/LICENSE.txt).

## [FreeType](https://www.freetype.org/index.html)

Freetype is a rather large font library that is used to load .TTF fonts into bitmaps that can be rendered on the screen. This enables the engine to show text.

Freetype is available under two different licenses where one can choose which based on the needs of the application. This engine uses the [FreeType license](http://git.savannah.gnu.org/cgit/freetype/freetype2.git/tree/docs/FTL.TXT).

## [GLFW](http://www.glfw.org)

In order to limit the amount of code needed to create a window with OpenGL, this project has decided to use GLFW. GLFW is a open-source, multi-platform library for OpenGL, OpenGL Es and Vulkan. It has a simple API to create and initialize both Windows and OpenGL contexts.

The library is licensed under a [zlib/libpng license](http://www.glfw.org/license.html).

## [OpenGL Loader](https://bitbucket.org/alfonse/glloadgen/wiki/Home)

OpenGL Loader, or in the case of our includes, `OGL`, is a library that loads OpenGL functions. The link points to the page where one can download a Lua binary that can executed with different parameters to create header and source files that exposes the OpenGL functions. If you have worked with OpenGL before, you might have used `glew`. This replaces `glew`.

The library is licensed under a [MIT license](https://bitbucket.org/alfonse/glloadgen/raw/e48321572bf6c2cbbac310a52c4071471005e425/License.txt).

## [SOIL](http://www.lonesock.net/soil.html)

In order to minimize the amount of code in the engine, this project has chosen to use SOIL for its image loading needs. This library handles loading PNG, which is most of our textures. It also handles multiple other formats and can also save images.

The library is a [Public Domain Software](https://en.wikipedia.org/wiki/Public-domain_software)

## [TinyXML2](http://www.grinninglizard.com/tinyxml2)

TinyXML2 is a library that helps loading XML files. It is currently used in loading the different GUI menues. This may be replaced with Lua code, eventually.

TinyXML2 is licensed under a [zlib license](https://github.com/leethomason/tinyxml2) as stated in their readme file.

## [Lua](https://www.lua.org/)

Lua is a programming language that was specifically built to be integrated into other languages, primarily C. This engine uses is as a form of being able to interact with the engine while it is running. It is primarily used for the GUI elements as well as for the in-engine console.

Unlike all the other dependencies, this dependency is not located within the `deps` folder and is expected to be installed on the system. The version required of Lua is v5.2+.

Lua is licensed under a [MIT license](https://www.lua.org/license.html).

## [SOL2](https://github.com/ThePhD/sol2)

Sol2 is a wrapper around the previously mentioned Lua C library. It exposes a more C++-like interface using templating and C++ idioms. The library is as a replacement of the Lua C library in this engine, but the library itself requires the C Lua library in order to work.

The library is licensed under a [MIT license](https://raw.githubusercontent.com/ThePhD/sol2/develop/LICENSE.txt).


## [Assimp](http://www.assimp.org)

Assimp (Open Asset Import Library) is a portable Open-source library to import various well-known 3D model formats. It is used to import the spider model used in this project from file into a format that can be used with OpenGL.

Assimp is licensed under a [3-clause BSD license](http://assimp.org/main_license.html)

## [mmm](https://github.com/Thhethssmuz/mmm)

mmm is a reimplementation of the original math library that was used for this engine. The math library draws inspiration from the math implementations in GLSL and extends upon the methods found in the shading language. It is widely used in the engine, but due to its reimplementation, it was moved to a different repository.

mmm is licensed under a [MIT license](https://raw.githubusercontent.com/Thhethssmuz/mmm/master/LICENSE).

## [spdlog](https://github.com/gabime/spdlog)

spdlog is a logging library that is advertised as one of the faster ones out there. It supports multiple loggers, colours, file-logging and is thread-safe. It is used instead of our previous logging to be able to make more sense of where things are coming from without having to explicitly state this in each log message.

spdlog is licensed under a [MIT license](https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE) where some of the subfiles are licensed under an BSD-like license.




