#pragma once

#include "../Drawable/Drawable.hpp"
#include "../OpenGLHeaders.hpp"

#include <mmm.hpp>
#include <memory>

class Texture;
class Phenotype;
class Program;

namespace NEAT {
  class NeuralNetwork;
}

/**
 * @brief
 *   This structure is used whenever you want to create a visual
 *   representation of the Phenotype.
 *
 *   The structure uses the network within the Phenotype to create
 *   a graph representation of the actual network.
 *
 *   It should be pointed out that this is just a snapshot of the network
 *   as it was when this structure was created.
 *
 *   It can either be drawn to the screen or saved to file.
 */
class DrawablePhenotype : Drawable {

  // For speed efficiency, both circles and lines use the same structure but
  // the variables mean different things.
  //
  // For instance, for a line, a Vertex structure would be:
  //
  // point.xy describes the start point of the line, where
  // point.zw describes teh end point of the line.
  //
  // For a circle, a Vertex structure would describe the following:
  //
  // point.xy is where the middle of the circle is
  // point.z is the radius of the circle
  // point.w is not used.
  //
  // The color just describes the color of the line/circle
  struct Vertex {
    mmm::vec4 point;
    mmm::vec4 color;
  };

  DrawablePhenotype();
  ~DrawablePhenotype();

  // These arent used but are defined due to inheritance
  void input(const Input::Event& event);
  void update(float deltaTime);

  void recreate(const NEAT::NeuralNetwork& network, mmm::vec2 size);

  void draw();

  void save(const std::string& filename);


private:
  mmm::vec2 mSize;

  int mNumLines;
  int mNumFilledCircles;
  int mNumOutlineCircles;

  GLuint mVBO;
  GLuint mVAO;

  std::shared_ptr<Program> mLinesProgram;
  std::shared_ptr<Program> mOutlineCircleProgram;
  std::shared_ptr<Program> mFilledCircleProgram;

  Texture* texture;
};
