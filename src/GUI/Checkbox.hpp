#pragma once

#include <string>

#include "../Math/Math.hpp"
#include "GUI.hpp"

class Text;
class Texture;
class GLRectangle;

namespace tinyxml2 {
  class XMLElement;
}

namespace Input {
  class Input;
}


class Checkbox : public GUI {
public:
  //! Creates a checkbox where box is the filename
  //! to the texture used
  Checkbox(const vec2& pos);

  //! Load the checkbox from XML
  static Checkbox* fromXML(tinyxml2::XMLElement* element);

  ~Checkbox();

  //! If position is inside the checkbox, it will
  //! either set it selected or unselected
  bool setSelected(const vec2& pos);

  //! Sets the offset of the GUI
  void setOffset(const vec2& of);

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  // Draws the box
  void draw();

private:
  bool           mIsTicked;
  Text*          mTick;
  GLRectangle* mSquare;
};
