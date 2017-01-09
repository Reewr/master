# Master Thesis

This repository contains the master thesis for Alexander Imenes and Svein Inge Albrigtsen. The master thesis was performed at [University of Agder](http://uia.no) in the Spring semester of 2017. The project itself bases itself on a game engine that was created during the course Advanced Programming with C++ in the fall semester of 2013. The engine was later expanded with Dynamic Animations and Procedural Terrain Generations during the bachelor thesis done in the Spring semester of 2014 by the same authors.

The repository contains several folders, most of which are essential to the code itself. Below is a short explanation of each of the directories

**cmake**

This folder contains files that are needed for building the project with Cmake. You will find some specific module files that help to find the difference modules needed for the project

**config**

This folder contains the .ini file that is used to set the different options used by the game engine. It contains anything from graphical options to keybindings.

**deps**

This folder contains all the dependencies of the project as source files.

**media**

This folder contains all resources needed by the engine during runtime such as textures, models and XML files used for loading the GUI.

**shaders**

This directory contains all the GLSL shader files used by the game engine. They can be considered additional source files.

**src**

The main source folder. All code is in here.

**tmp**

Temporary directory that should be removed


# Licenses

This project utilizes seven (7) different dependencies for release builds and eight (8) different dependencies for development builds. The dependencies are as follows:

- [backward-cpp](https://github.com/bombela/backward-cpp)
- [FreeType](https://www.freetype.org/index.html)
- [GLFW](http://www.glfw.org)
- [OpenGL Loader](https://bitbucket.org/alfonse/glloadgen/wiki/Home)
- [SOIL](http://www.lonesock.net/soil.html)
- [TinyXML2](http://www.grinninglizard.com/tinyxml2)
- [Bullet3](http://bulletphysics.org/wordpress/)
- [Assimp](http://www.assimp.org)

The licenses for each dependency can be found in the [deps](./deps) folder, except for `assimp` that has to be installed globally on the system when building.




