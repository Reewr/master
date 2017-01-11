#ifndef GUI_TOOLTIP_HPP
#define GUI_TOOLTIP_HPP

#include <string>

#include "../Math/Math.hpp"
#include "GUI.hpp"

class Text;
class Texture;

class Tooltip : public GUI {
public:
  Tooltip();

  //! Shows the tooltip with a specific text at a specific
  //! screen position
  void show(std::string, const vec2& pos);

  //! sets the offset of the tooltip and all its elements
  void setOffset(const vec2& offset);

  //! Draws
  void draw(float deltaTime);

private:
  Text*    mActiveText;
  Texture* mBackground;
};

#endif