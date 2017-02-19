#pragma once

#include <string>

#include "../Log.hpp"
#include "GUI.hpp"

class Text;
class GLRectangle;
class CFG;

class Tooltip : public GUI, public Logging::Log {
public:
  Tooltip();

  //! Shows the tooltip with a specific text at a specific
  //! screen position
  void show(std::string, const mmm::vec2& pos);

  //! sets the offset of the tooltip and all its elements
  void setOffset(const mmm::vec2& offset);

  //! Draws
  void draw();

private:
  CFG*         mCFG;
  Text*        mActiveText;
  GLRectangle* mBackground;
};
