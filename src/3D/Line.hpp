#pragma once

#include "../Drawable/Drawable3D.hpp"
#include "../Log.hpp"
#include <memory>

#include <mmm.hpp>

class GLLine;

class Texture;
class Program;

/**
 * @brief
 *   This represents a Line. It can be specified to be
 *   at a specified position
 */
class Line : public Drawable3D, public Logging::Log {
public:
  // Creates a cube of a specific size in meters. If weight is -1,
  // it is assumed that the cube contains water,
  // i.e 1m^3 (1,1,1) cube weighs 1000kg
  //     8m^3 (2,2,2) cube weighs 8000kg
  //
  // The position is where the box will start at
  Line(const mmm::vec3& start,
       const mmm::vec3& end,
       const mmm::vec4& color = mmm::vec4(-1));
  ~Line();

  // Update the element, physics etc
  void update(float deltaTime);

  // Draw it. Keep it separate from update
  void draw(std::shared_ptr<Program>& program, bool bindTexture = false);

  void draw(std::shared_ptr<Program>& program,
            mmm::vec3                 offset,
            bool                      bindTexture = false);

  // Input handler for cube
  void input(const Input::Event& event);

private:
  GLLine*                  mLine;
  std::shared_ptr<Texture> mTexture;
  bool mUsesColor;
  mmm::vec4 mColor;
};
