#ifndef GUI_CHECKBOX_HPP
#define GUI_CHECKBOX_HPP

#include <string>

#include "GUI.hpp"
#include "../Math/MathCD.hpp"

class Text;
class Texture;

namespace tinyxml2 {
  class XMLElement;
}

class Checkbox : public GUI {
public:
  //! Creates a checkbox where box is the filename
  //! to the texture used
  Checkbox(const std::string box, const vec2& pos);

  //! Load the checkbox from XML
  static Checkbox* fromXML(tinyxml2::XMLElement* element);

  ~Checkbox();

  //! If position is inside the checkbox, it will
  //! either set it selected or unselected
  void setSelected(const vec2& pos);

  //! Sets the offset of the GUI
  void setOffset(const vec2& of);

  // Draws the box
  void draw(float deltaTime) const;

private:
  bool     mIsTicked;
  Text*    mTick;
  Texture* mSquare;
};

#endif