cmake_minimum_required(VERSION 3.3)
project(MultiNEAT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS date_time system filesystem serialization)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(MultiNEAT/src)

set(SOURCE_FILES
  MultiNEAT/src/Assert.h
  MultiNEAT/src/Genes.h
  MultiNEAT/src/Genome.cpp
  MultiNEAT/src/Genome.h
  MultiNEAT/src/Innovation.cpp
  MultiNEAT/src/Innovation.h
  MultiNEAT/src/NeuralNetwork.cpp
  MultiNEAT/src/NeuralNetwork.h
  MultiNEAT/src/Parameters.cpp
  MultiNEAT/src/Parameters.h
  MultiNEAT/src/PhenotypeBehavior.cpp
  MultiNEAT/src/PhenotypeBehavior.h
  MultiNEAT/src/Population.cpp
  MultiNEAT/src/Population.h
  MultiNEAT/src/Random.cpp
  MultiNEAT/src/Random.h
  MultiNEAT/src/Species.cpp
  MultiNEAT/src/Species.h
  MultiNEAT/src/Substrate.cpp
  MultiNEAT/src/Substrate.h
  MultiNEAT/src/Utils.cpp
  MultiNEAT/src/Utils.h)

add_library(MultiNEAT SHARED ${SOURCE_FILES})
