#pragma once

#include "../GUI/Text.hpp"

class Camera;

class Text3D : public Text {
public:
  // Fixes warning about hiding functions
  using Text::setPosition;
  using GUI::draw;

  Text3D(const std::string& font,
         const std::string& text,
         const mmm::vec3&   position);

  // Returns the normalized size of the text (between -1 and 1)
  mmm::vec2 getNormalizedSize();

  void setPosition(const mmm::vec3& position);

  void draw(mmm::vec3 offset = mmm::vec3(0,0,0));

private:
  std::shared_ptr<Program> mFont3DProgram;
  mmm::vec3 mPosition;
};
