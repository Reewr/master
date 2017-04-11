#pragma once

#include "../GUI/Text.hpp"

class Camera;

class Text3D : public Text {
public:

  Text3D(const std::string& font,
         const std::string& text,
         const mmm::vec3&   position);

  void draw();

  mmm::vec2 getNormalizedSize();

private:
  std::shared_ptr<Program> mFont3DProgram;
  mmm::vec3 mPosition;
};
