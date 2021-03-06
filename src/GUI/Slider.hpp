#pragma once

#include <string>

#include "../OpenGLHeaders.hpp"

#include "../Log.hpp"
#include "GUI.hpp"

class Texture;
class Text;
class GLRectangle;

namespace tinyxml2 {
  class XMLElement;
}


class Slider : public GUI, public Logging::Log {
public:
  //! Loads textures and calls recalculatesGeometry
  Slider(const mmm::vec2&   pos,
         const float        scale   = 1,
         const std::string& valSign = "%");

  static Slider* fromXML(tinyxml2::XMLElement* element);

  //! Deletes textures
  ~Slider();

  //! Can be called to do the default actions by using
  //! setInputHandler. Is also called by default if setInputHandler
  //! is never called.
  void defaultInputHandler(const Input::Event& event);

  //! Changing the offset of all items
  void setOffset(const mmm::vec2& offset);

  float value() const;

  //! Moves the slider on the X axis
  bool moveSlider(const mmm::vec2& position);

  //! Sets the slider to a specific position based a value
  void setSlider(float s);

  //! :)
  void draw();

private:
  GLRectangle* mBackground;
  GLRectangle* mButton;
  Text*        mInfo;

  float       mScale;
  float       mValue;
  std::string mValSign;

  Rectangle mButtonRect;
  mmm::vec2 mButtonOffset;
};
