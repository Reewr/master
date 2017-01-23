#pragma once

#include <string>

#include "../Math/Math.hpp"
#include "GUI.hpp"

class Text;
class GLRectangle;

class Tooltip : public GUI {
public:
  Tooltip();

  //! Shows the tooltip with a specific text at a specific
  //! screen position
  void show(std::string, const vec2& pos);

  //! sets the offset of the tooltip and all its elements
  void setOffset(const vec2& offset);

  //! Draws
  void draw();

private:
  CFG*           mCFG;
  Text*          mActiveText;
  GLRectangle* mBackground;
};